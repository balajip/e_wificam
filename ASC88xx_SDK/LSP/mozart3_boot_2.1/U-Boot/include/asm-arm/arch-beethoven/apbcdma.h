#ifndef __BEETHOVEN_APBCDMA_H__
#define __BEETHOVEN_APBCDMA_H__

#include <asm/hardware.h>
#include <malloc.h>

#define VPL_APBC_MMR_BASE 0xCE000000

#define APBC_MMR(r)   (VPL_APBC_MMR_BASE + (r))

#define APBC_DMA_STAT                  APBC_MMR(0x88)
#define APBC_DMA_CHN_MONITER           APBC_MMR(0x8C)
#define APBC_DMA_n_SRC_ADDR(n)         (APBC_MMR(0x90) + (n)*16)
#define APBC_DMA_n_DES_ADDR(n)         (APBC_MMR(0x94) + (n)*16)
#define APBC_DMA_n_LLP(n)              (APBC_MMR(0x98) + (n)*16)
#define APBC_DMA_n_CTRL(n)             (APBC_MMR(0x9C) + (n)*16)
#define     DMA_CTRL_CYC(x)            ((x) << 20)
#define     DMA_CTRL_DATA_SZ(x)        ((x) << 18)
#define         SZ_8_BITS              (0)
#define         SZ_16_BITS             (1)
#define         SZ_32_BITS             (2)
#define         SZ_64_BITS             (3)
#define     DMA_CTRL_BYTE_SWAP(x)      ((x) << 17)
#define         SWAP_NO                (0)
#define         SWAP_YES               (1)
#define     DMA_CTRL_REQ_TYPE(x)       ((x) << 16)
#define     DMA_CTRL_REQ_SEL(x)        ((x) << 12)
#define     DMA_CTRL_DES_ADDR_INC(x)   ((x) << 10)
#define     DMA_CTRL_SRC_ADDR_INC(x)   ((x) <<  8)
#define         ADDR_NO_INC            (0)
#define         ADDR_POSITIVE_INC      (1)
#define         ADDR_NEGATIVE_INC      (2)
#define     DMA_CTRL_TRANS_TYPE(x)     ((x) <<  6)
#define         TRANS_AHB2AHB          (0)
#define         TRANS_AHB2APB          (1)
#define         TRANS_APB2AHB          (2)
#define         TRANS_APB2APB          (3)
#define     DMA_CTRL_BURST_MODE(x)     ((x) <<  4)
#define         BURST_1                (0)
#define         BURST_4                (1)
#define         BURST_8                (2)
#define         BURST_16               (3)
#define     DMA_CTRL_INTR_LL_EN        BIT_MASK(3)
#define     DMA_CTRL_INTR_CMPT_EN      BIT_MASK(2)
#define     DMA_CTRL_INTR_CMPT         BIT_MASK(1)
#define     DMA_CTRL_INTR_OP_EN        BIT_MASK(0)



#define DMA_CYC_MAX   (4095)

#define APBC_DMA_CHANNEL_SSIC_RX      (2)
#define APBC_DMA_CHANNEL_SSIC_TX      (3)


struct apb_dma_desc
{
	void *   src;
	void *   des;
	struct apb_dma_desc *   llp;
	uint32_t   ctrl;
};


static inline void
apb_free_dmadesc(struct apb_dma_desc *desc1)
{
	struct apb_dma_desc *   desc2;

	while (desc1)
	{
		desc2 = desc1->llp;
		free(desc1);
		desc1 = desc2;
	}
}

#endif
