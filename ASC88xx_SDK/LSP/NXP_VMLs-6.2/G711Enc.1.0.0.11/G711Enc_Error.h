/*
 * $Header: /rd_2/project/SoC/Linux_Libraries/G711Enc/G711Enc_Lib/G711Enc_Error.h 2     13/01/15 4:45p Aphon.wu $
 *
 * Copyright 2009 ______, Inc. All rights reserved.
 *
 * Description:
 *
 * $History: G711Enc_Error.h $
 * 
 * *****************  Version 2  *****************
 * User: Aphon.wu     Date: 13/01/15   Time: 4:45p
 * Updated in $/rd_2/project/SoC/Linux_Libraries/G711Enc/G711Enc_Lib
 * 
 * *****************  Version 1  *****************
 * User: Aphon.wu     Date: 13/01/15   Time: 3:46p
 * Created in $/rd_2/project/SoC/Linux_Libraries/G711Enc/G711Enc_Lib
 * 
 * 
 */
/* ===============================================================================================*/
#ifndef __G711ENC_ERROR_H__
#define __G711ENC_ERROR_H__
/* ===============================================================================================*/

#define _______G711ENC_ERR_INVALIDE_VERSION				0x80060001
#define _______G711ENC_ERR_INVALIDE_ARG					0x80060002
#define _______G711ENC_ERR_UNSUPPORT_SOC_PLATFORM 		0x80060003		//: Some features are only supported on specific SoC platforms.
#define _______G711ENC_ERR_OUT_OF_SYSTEM_MEMORY			0x80060004		//: Used when calloc or malloc failed.

/* ===============================================================================================*/
#endif //__G711ENC_ERROR_H__