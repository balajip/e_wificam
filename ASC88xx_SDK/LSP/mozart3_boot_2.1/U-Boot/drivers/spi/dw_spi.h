#ifndef _DW_SPI_H_
#define _DW_SPI_H_

#include <linux/bitops.h>

/* CTRLR0 */
#define SSIC_CTRLR0_TMOD(x)      ((x) << 8)
#define     CTRLR0_TX_ONLY       (0x1)
#define     CTRLR0_EEPROM_READ   (0x3)
#define SSIC_CTRLR0_SCPOL        BIT_MASK(7)
#define SSIC_CTRLR0_SCPH         BIT_MASK(6)
#define SSIC_CTRLR0_MODE(x)      ((x) << 6)
#define SSIC_CTRLR0_DFS(x)       (((x) - 1) << 0)
#define SSIC_CTRLR0_CFS(x)       (((x) - 1) << 12)

/* CTRLR1 */
#define SSIC_CTRLR1_NDF_MAX      (0x10000)

/* SR */
#define SSIC_SR_TXE              BIT_MASK(5)
#define SSIC_SR_RFF              BIT_MASK(4)
#define SSIC_SR_RFNE             BIT_MASK(3)
#define SSIC_SR_TFE              BIT_MASK(2)
#define SSIC_SR_TFNF             BIT_MASK(1)
#define SSIC_SR_BUSY             BIT_MASK(0)

/* IMR */
#define SSIC_IMR_RXFIM           BIT_MASK(4)
#define SSIC_IMR_RXOIM           BIT_MASK(3)
#define SSIC_IMR_RXUIM           BIT_MASK(2)
#define SSIC_IMR_TXOIM           BIT_MASK(1)
#define SSIC_IMR_TXEIM           BIT_MASK(0)

/* DMACR */
#define SSIC_DMACR_TDMAE         BIT_MASK(1)
#define SSIC_DMACR_RDMAE         BIT_MASK(0)

#endif /* !_DW_SPI_H_ */
