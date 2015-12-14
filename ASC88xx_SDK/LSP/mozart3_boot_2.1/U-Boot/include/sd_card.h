#ifndef __SD_CARD_H__
#define __SD_CARD_H__

extern int SD_Init(void) ;
extern int SD_Read(unsigned long sd_addr, unsigned long cnt, unsigned long memory_addr);
extern int SD_Card_Detect(int print_msg);

#define MAGIC_SD_ADDR   0x0
#define MAGIC_DRAM_ADDR 0xa00000
#define MAGIC_DATA_SIZE 0X200
#define MAGIC_NUM0 0x82451282
#define MAGIC_NUM1 0x28791166

#endif //__SD_CARD_H__

