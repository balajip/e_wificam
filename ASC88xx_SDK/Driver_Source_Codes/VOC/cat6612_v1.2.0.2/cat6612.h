/*
 * cat6612
 * Driver for CAT6612 HDMI Tx
 *
 * Copyright (C) 2010  VN Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __CAT6612_H__
#define __CAT6612_H__


#define _01_VendorID        0x01
#define _02_DevID0          0x02
#define _03_DevID1          0x03
#define _04_RstCtrl         0x04
#define _05_IntrMode        0x05
#define _08_VIOCtrl         0x08
#define _09_IntrMask0       0x09
#define _0A_IntrMask1       0x0A
#define _0B_IntrMask2       0x0B
#define _0E_SysStat         0x0E
#define _0F_BankSel         0x0F
#define _10_MasterSel       0x10
#define _11_RDDCHeader      0x11
#define _12_RDDCOffSet      0x12
#define _13_RDDCByte        0x13
#define _14_RDDCSeg         0x14
#define _15_RDDCReq         0x15
#define _16_RDDCStat        0x16
#define _17_RDDCReadFIFO    0x17
#define _20_HDCPEn          0x20
#define _22_KSVList         0x22
#define _58_ClkCtrl0        0x58
#define _59_ClkCtrl1        0x59
#define _61_AFEDrvCtrl      0x61
#define _62_AFEXPCtrl       0x62
#define _63_AFEXPCtrl2      0x63
#define _64_AFEIPCtrl       0x64
#define _65_AFERing         0x65
#define _70_InputMode       0x70
#define _72_CSCCtrl         0x72

#define _90_HVPol           0x90
#define _91_HFPixel         0x91 //Horizontal Total
#define _95_HSSL            0x95 //Horizontal Sync Start; Low Byte
#define _96_HSEL            0x96 //Horizontal Sync End; Low Byte
#define _97_HSH             0x97 //[7:4]Horizontal Sync End; High Byte;[3:0]Horizontal Sync Start; High Byte
#define _A0_VSS1            0xA0 //Vertical Sync Start; Low Byte
#define _A1_VSE1            0xA1 //[7:4]Vertical Sync End; [2:0]Vertical Sync Start; High Byte
#define _A2_VSS2            0xA2 //2nd Field Vertical Sync Start; Low Byte
#define _A3_VSE2            0xA3 //2nd Field Vertical Sync End

#define _C0_HDMIMode        0xC0
#define _C1_Mute            0xC1
#define _C2_HDMICtrl        0xC2
#define _C3_OESS            0xC3
#define _C6_GenCtrlPkt      0xC6
#define _C9_NullPkt         0xC9
#define _CA_ACPPkt          0xCA
#define _CB_ISRC1Pkt        0xCB
#define _CC_ISRC2Pkt        0xCC
#define _CD_AVIInfoPkt      0xCD
#define _CE_AudioInfoPkt    0xCE
#define _CF_SPDInfoPkt      0xCF
#define _D0_MpgInfoPkt      0xD0
#define _F3_Debug0          0xF3
#define _F4_Debug1          0xF4

/* Bank 1 Registers*/
#define _B1_58_AVIINFO_DB1  0x58
#define _B1_59_AVIINFO_DB2  0x59
#define _B1_5A_AVIINFO_DB3  0x5A
#define _B1_5B_AVIINFO_DB4  0x5B
#define _B1_5C_AVIINFO_DB5  0x5C
#define _B1_5E_AVIINFO_DB6  0x5E
#define _B1_5F_AVIINFO_DB7  0x5F
#define _B1_60_AVIINFO_DB8  0x60
#define _B1_61_AVIINFO_DB9  0x61
#define _B1_62_AVIINFO_DB10 0x62
#define _B1_63_AVIINFO_DB11 0x63
#define _B1_64_AVIINFO_DB12 0x64
#define _B1_65_AVIINFO_DB13 0x65
#define _B1_5D_AVIINFO_SUM  0x5D

typedef unsigned char BYTE;

struct SyncEmbeddedSetting {
    BYTE fmt;
    BYTE RegHVPol;      // Reg90
    BYTE RegHfPixel;    // Reg91
    BYTE RegHSSL;       // Reg95
    BYTE RegHSEL;       // Reg96
    BYTE RegHSH;        // Reg97
    BYTE RegVSS1;       // RegA0
    BYTE RegVSE1;       // RegA1
    BYTE RegVSS2;       // RegA2
    BYTE RegVSE2;       // RegA3
    BYTE REGHVPol656;   // Reg90 when CCIR656
    BYTE REGHfPixel656; // Reg91 when CCIR656
    BYTE RegHSSL656;    // Reg95 when CCIR656
    BYTE RegHSEL656;    // Reg96 when CCIR656
    BYTE RegHSH656;     // Reg97 when CCIR656
    unsigned long PCLK ;
    BYTE VFreq ;
} ;

#endif //__CAT6612_H__
