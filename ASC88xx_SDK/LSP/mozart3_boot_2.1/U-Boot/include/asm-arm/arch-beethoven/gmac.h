#ifndef __BEETHOVEN_GMAC_H__
#define __BEETHOVEN_GMAC_H__

#define GMAC0_MMR_BASE 0xED000000
#define GMAC1_MMR_BASE 0xEE000000

#define GMAC_MMR_BASE GMAC0_MMR_BASE

//Because this file has many same settings on Haydn and Mozart, 
//so we only keep one file for sharing.
//We include this header file here to meet the requests of gmac_test.c
#include "../../../drivers/net/gmac.h" 

#define GMAC_10_100Mbps 1
#define GMAC_1000Mbps 2
#define GMAC_SPEED GMAC_1000Mbps

//#ifdef CONFIG_SOC_MOZART
//#define GMAC_RGMII 1
//#endif

#define ICPLUS_IP1001_ID_1  0x0243
#define ICPLUS_IP1001_ID_2  0x0d90
#define REALTEK_ID_1 0x001c
#define REALTEK_ID_2 0xc910
#define ICPLUS_IP101_ID_1 0x0243
#define ICPLUS_IP101_ID_2 0X0C54
#define BROADCOM_BCM54616_ID_1	0x0362
#define BROADCOM_BCM54616_ID_2	0x5D12
#define MICREL_KSZ8051_ID_1	0x0022
#define MICREL_KSZ8051_ID_2	0x1550

#endif
