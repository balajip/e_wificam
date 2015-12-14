#ifndef __MOZART_DCE_H__
#define __MOZART_DCE_H__

#define SYSC_CTRL0_DCE	(0x80)


#define EVM_DCE_BASE		VMA_DCE_MMR_BASE
#define DCE_CTRL				(EVM_DCE_BASE + 0x08)
#define DCE_STAT				(EVM_DCE_BASE + 0x0c)
#define DCE_SIZE				(EVM_DCE_BASE + 0x10)
#define DCE_SRC 				(EVM_DCE_BASE + 0x14)
#define DCE_IV  				(EVM_DCE_BASE + 0x18)
#define DCE_IK	  			(EVM_DCE_BASE + 0x1c)
#define DCE_DEST				(EVM_DCE_BASE + 0x20)

#define DCE_CTRL_KEY_SIZE_SHIFT		(17)
#define DCE_CTRL_OPMODE_SHIFT			(15)
#define DCE_CTRL_ENCTYPE_SHIFT		(13)
#define DCE_CTRL_ENCMODE_SHIFT		(10)
#define DCE_CTRL_OPSTART_SHIFT		(2)
#define DCE_CTRL_OP_CMPT_ACK_EN_SHIFT		(1)
#define DCE_CTRL_OP_CMPT_ACK_SHIFT		(0)
#define DCE_CTRL_KEY_SIZE(x)	    ((x)<<(DCE_CTRL_KEY_SIZE_SHIFT))
#define DCE_CTRL_OPMODE(x)				((x)<<(DCE_CTRL_OPMODE_SHIFT))
#define DCE_CTRL_OPSTART(x)				((x)<<(DCE_CTRL_OPSTART_SHIFT))
#define DCE_CTRL_ENCTYPE(x)				((x)<<(DCE_CTRL_ENCTYPE_SHIFT))
#define DCE_CTRL_ENCMODE(x)				((x)<<(DCE_CTRL_ENCMODE_SHIFT))
#define DCE_CTRL_OP_CMPT_ACK_EN(x)((x)<<DCE_CTRL_OP_CMPT_ACK_EN_SHIFT)
#define DCE_CTRL_OP_CMPT_ACK(x)		((x)<<DCE_CTRL_OP_CMPT_ACK_SHIFT)


#define OPMODE_DEC				(0x1)
#define ENCTYPE_CFB				(0x2)

#define OP_START					(0x4)
#define OP_CMPT_ACK_EN 		(0x1)
#define OP_CMPT_ACK 			(0x1)

#define DCE_STAT_OP_CMPT	(0x1)

int DCE_OperationStart(unsigned int input, unsigned int len, unsigned int output, unsigned char bDecrypt );
int DCE_init( void );

#endif
