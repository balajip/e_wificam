/*
 * asm/arch-bach/dma.h
 */

#ifndef __ASM_ARCH_DMA_H__
#define __ASM_ARCH_DMA_H__

//#define MAX_DMA_ADDRESS		0xffffffff	
//#define MAX_DMA_CHANNELS        8
//#define MAX_DMA_CHANNELS        0
#define CPE_APB_DMA_MAX_CHANNEL         4       //paul
#define CPE_AHB_DMA_MAX_CHANNEL         8       //paul

#define MAX_DMA_ADDRESS                 DMA_SIZE
//#define MAX_DMA_CHANNELS        (CPE_APB_DMA_MAX_CHANNEL+CPE_AHB_DMA_MAX_CHANNEL)
//ivan for audio test
#define MAX_DMA_CHANNELS 0
//#define MAX_DMA_CHANNELS 4	// aren 20041201
                                //Paul: 0~3 APB, 4~12 AHB
                                

#endif /* _ASM_ARCH_DMA_H__ */
