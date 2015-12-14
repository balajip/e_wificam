/*

/* ============================================================================================== */
#ifndef __VIDEOCAP_TABLES_H__
#define __VIDEOCAP_TABLES_H__

/* ============================================================================================== */
extern const BYTE abyRGB2YCbCrBT601Full[10];
extern const BYTE abyRGB2YCbCrBT601[10];
extern const BYTE abyRGB2YCbCrBT709Full[10];
extern const BYTE abyRGB2YCbCrBT709[10];
extern const BYTE abyGammaTbl[256];

/* ======================================== */
/* Version 3.1.0.6 modification, 2008.03.19 */
extern const BYTE abySmallExpTbl[2000];
extern const DWORD adwInvTbl[480];

extern const DWORD adwExpTblK07[1024];
extern const DWORD adwExpTblK08[1024];
extern const DWORD adwExpTblK09[1024];
extern const DWORD adwExpTblK10[1024];
extern const DWORD adwExpTblK11[1024];
extern const DWORD adwExpTblK12[1024];

// Table Make it Darker
extern const BYTE abyIPDarkTblE0[256];
extern const BYTE abyIPDarkTblE1[256];
extern const BYTE abyIPDarkTblE2[256];
extern const BYTE abyIPDarkTblE3[256];
// Table Make it Darker with D-lighting
extern const BYTE abyIPDarkTblE0DL[256];
extern const BYTE abyIPDarkTblE1DL[256];
extern const BYTE abyIPDarkTblE2DL[256];
extern const BYTE abyIPDarkTblE3DL[256];

// Table without D-Lighting
// Table with D-Lighting

//Table Make contrast larger
extern const BYTE abyIPContrastE0[256];
extern const BYTE abyIPContrastE1[256];
extern const BYTE abyIPContrastE2[256];
extern const BYTE abyIPContrastE3[256];

/* Version 4.0.0.2 modification, 2008.10.06 */
extern const DWORD adwCETbl0[256]; 
extern const DWORD adwCETbl1[256]; 
extern const DWORD adwCETbl2[256]; 
extern const DWORD adwCETbl3[256]; 
/* ======================================== */
/* ======================================== */

/* ============================================================================================== */
#endif //__VIDEOCAP_TABLES_H__

/* ============================================================================================== */
