/*
 * $Header: /rd_2/project/SoC/Linux_Libraries/G711Dec/G711Dec_Lib/G711Dec_Error.h 1     13/01/15 1:51p Aphon.wu $
 *
 * Copyright 2009 ______, Inc. All rights reserved.
 *
 * Description:
 *
 * $History: G711Dec_Error.h $
 * 
 * *****************  Version 1  *****************
 * User: Aphon.wu     Date: 13/01/15   Time: 1:51p
 * Created in $/rd_2/project/SoC/Linux_Libraries/G711Dec/G711Dec_Lib
 * 
 * 
 */
/* ===============================================================================================*/
#ifndef __G711DEC_ERROR_H__
#define __G711DEC_ERROR_H__
/* ===============================================================================================*/

#define _______G711DEC_ERR_INVALIDE_VERSION				0x80040001
#define _______G711DEC_ERR_INVALIDE_ARG					0x80040002
#define _______G711DEC_ERR_UNSUPPORT_SOC_PLATFORM 		0x80040003		//: Some features are only supported on specific SoC platforms.
#define _______G711DEC_ERR_OUT_OF_SYSTEM_MEMORY			0x80040004		//: Used when calloc or malloc failed.

/* ===============================================================================================*/
#endif //__G711DEC_ERROR_H__