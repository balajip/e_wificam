/*
 * Command for DCE processing.
 *
 * Copyright (C) 2013 VN Inc
 */
#include <common.h>
#include <malloc.h>
#include <asm/io.h>

#include <asm/arch/platform.h>

char SecurityKey[]__attribute__((aligned(128))) = {	0xBE, 0x61, 0xb7, 0x99, 0x41, 0xa2, 0x48, 0x41, 0x14, 0x9e, 0x88, 0x66, 0xeb, 0x5d, 0x77, 0xbe, 
												0x88, 0x48, 0xa2, 0x14, 0xbe, 0x5d, 0xb7, 0xbe, 0xeb, 0x61, 0x77, 0x99, 0x41, 0x66, 0x9e, 0x41};
												// 419e_6641_9977_61eb_beb7_5dbe_14a2_4888_be77_5deb_6688_9e14_4148_a241_99b7_61be
char InitialVector[]__attribute__((aligned(16))) = {0x13, 0x20, 0x50, 0x88, 0x87, 0x26, 0x2b, 0x1E, 0x2A, 0x5c, 0x50, 0x4c, 0xe5, 0x2f, 0x10, 0x83};
												//8310_2FE5_4C50_5C2A_1E2B_2687_8850_2013

void DCE_WaitOperationDone (void)
{
	while ( (v_inl(DCE_CTRL)&DCE_STAT_OP_CMPT) == 0 );
}

int DCE_OperationStart(unsigned int input, unsigned int len, unsigned int output, unsigned char bDecrypt )
{
	int iRet = 0;
	int pInitialKey = 0;
	int pSecureKey = 0;
	int pKeyPtrAlignAddress = 0;
	
	pInitialKey = (int)mALLOc(sizeof(InitialVector) + 0x80) ;
	pSecureKey = (int)mALLOc(sizeof(SecurityKey) + 0x80) ;	
	
	if ( pInitialKey & 0x7F) {
		pKeyPtrAlignAddress = (pInitialKey + 0x80) & 0x80;
	}
	memcpy( (char*)pKeyPtrAlignAddress, InitialVector, sizeof(InitialVector));
	v_outl(DCE_IV, pKeyPtrAlignAddress); //VPL_BRC_ROM_BASE_REMAPPED ) ;
	printf("IV:%x\n", pKeyPtrAlignAddress);

	if ( pSecureKey & 0x7F) {
		pKeyPtrAlignAddress = (pSecureKey + 0x80) & 0x80;
	}
	memcpy( (char*)pKeyPtrAlignAddress, SecurityKey, sizeof(SecurityKey));
	v_outl(DCE_IK, pKeyPtrAlignAddress); //VPL_BRC_ROM_BASE_REMAPPED+0x20 ) ;
	printf("KEY:%x\n", pKeyPtrAlignAddress);
	v_outl(DCE_SRC, input ) ;
	v_outl(DCE_DEST, output ) ;
	v_outl(DCE_SIZE, len ) ;
	
	iRet = v_inl(DCE_CTRL);
	iRet &= ~DCE_CTRL_OPMODE(OPMODE_DEC);
	iRet |= (DCE_CTRL_OPSTART(1) | DCE_CTRL_OPMODE(bDecrypt));
	v_outl(DCE_CTRL, iRet ) ;
	
	DCE_WaitOperationDone();
		
	iRet = v_inl(DCE_CTRL);
	iRet &= ~DCE_STAT_OP_CMPT;
	v_outl(DCE_CTRL, iRet ) ;
	iRet = 0;
	
  if((char*)pInitialKey != NULL)
     fREe((char*)pInitialKey) ;
  if((char*)pSecureKey != NULL)
     fREe((char*)pSecureKey) ;
     
	return iRet;
}

int DCE_init( void )
{
	unsigned int	reg;

	// sysc enable
	reg = v_inl(EVM_SYSC_BASE|SYSC_MMR_CLKEN_CTRL0 ) ;
	reg |= SYSC_CTRL0_DCE;
	v_outl(EVM_SYSC_BASE|SYSC_MMR_CLKEN_CTRL0, reg ) ;
	
	v_outl(DCE_CTRL, DCE_CTRL_KEY_SIZE(0x120)|DCE_CTRL_ENCMODE(ENCTYPE_CFB)|DCE_CTRL_OP_CMPT_ACK_EN(OP_CMPT_ACK_EN));	//0x240800 ) ;
	v_outl(DCE_IV, (int)InitialVector); //VPL_BRC_ROM_BASE_REMAPPED ) ;
	v_outl(DCE_IK, (int)SecurityKey); //VPL_BRC_ROM_BASE_REMAPPED+0x20 ) ;

	return 1;
}

