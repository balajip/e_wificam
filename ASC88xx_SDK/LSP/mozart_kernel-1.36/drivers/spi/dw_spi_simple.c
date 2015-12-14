

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/spi/spi.h>
#include <linux/string.h>
#include <asm/uaccess.h>
#include <linux/delay.h> // msleep
//FIFO Width (bits)
#define SPI_BUFF_SIZE	16 
#define USER_BUFF_SIZE	2048

#define SPI_BUS 0
#define SPI_BUS_CS0 0
#define SPI_BUS_CS1 1

/*The max clk of  spi flash on the mozart  50M Hz but we just use the 25M Hz as the clk*/
#define SPI_BUS_SPEED (50*1000000) 


#define TEST_MN34041PL 1



const char this_driver_name[] = "dw-spi-simple";

struct spike_control {
	struct spi_message msg;
	struct spi_transfer transfer;
	u8 *tx_buff; 
	u8 *rx_buff;
};

static struct spike_control spike_ctl;

struct spike_dev {
	struct semaphore spi_sem;
	struct semaphore fop_sem;
	dev_t devt;
	struct cdev cdev;
	struct class *class;
	struct spi_device *spi_device;
	char *user_buff;
	u8 test_data;	
};

static struct spike_dev spike_dev;

 enum{
		  //Instruction set
		  SPI_FLASH_INS_WREN		= 0x06,		// write enable
		  SPI_FLASH_INS_WRDI		= 0x04,		// write disable
		  SPI_FLASH_INS_RDSR		= 0x05,		// read status register
		  SPI_FLASH_INS_WRSR		= 0x01,		// write status register
		  SPI_FLASH_INS_READ		= 0x03,		// read data bytes
		  SPI_FLASH_INS_FAST_READ	= 0x0B,		// read data bytes at higher speed
		  SPI_FLASH_INS_PP			= 0x02,		// page program
		  SPI_FLASH_INS_SE			= 0xD8,		// sector erase
		  SPI_FLASH_INS_BE			= 0xC7		// bulk erase
};

int dw_spi_status(struct spi_device *spi_device,u8 *ptstatus, int busnum);
int dw_spi_flash_read(struct spi_device *spi_device,u32 offset,u32 len,u8 *buf,int busnum);
int dw_spi_flash_write(struct spi_device *spi_device,u32 offset,u32 len,u8 *buf,int busnum);
int dw_spi_writeen(struct spi_device *spi_device);
int dw_spi_readID(struct spi_device *spi_device,u8 *id,int busnum);

int dw_spi_flash_read(struct spi_device *spi_device,u32 offset,u32 len,u8 *buf,int busnum)
{
    int status=0;
  
    
    
	    //Flash read test
	   
	    unsigned long read_addr=offset;//0x0;
	    struct spi_transfer t[2];
	    struct spi_message m;
	    unsigned short tmpdatabuf[2];
	    unsigned long nSize;

	    u8 *buff=buf;
	    
	    spi_message_init(&m);
	    memset(t, 0, (sizeof t));
	    memset(tmpdatabuf,0x0,sizeof(tmpdatabuf));
	   
	    nSize=len;
	    

	    
	    tmpdatabuf[0]= (SPI_FLASH_INS_READ<<8) | ((read_addr & 0x00FF0000)>>16);
	    tmpdatabuf[1]=  read_addr & 0x0000FFFF;
	    //tmpdatabuf[2]=nSize;
	    
	    t[0].tx_buf = tmpdatabuf;
	    t[0].rx_buf = NULL;
	    t[0].len =4;
	    t[0].speed_hz=(25*1000000);
	    t[0].bits_per_word=16;
	    t[0].cr0=0x73cf;
	    if(nSize&0x1)
	    {
	      
	      t[0].cr1=((nSize+1)>>1)-1;
	    }
	    else
	    {
	      t[0].cr1=(nSize>>1)-1;

	      
	    }
	    t[0].ser=1 << busnum;
	    t[0].ssienr=0x1;
	    //t[0].cs_change=1;
	    spi_message_add_tail(&t[0], &m);

	    t[1].tx_buf = NULL;
	    t[1].rx_buf = buff;
	    if(nSize&0x1)
	    {
	      t[1].len = nSize+1;
	    }
	    else
	    {
	      t[1].len = nSize;
	    }
	    t[1].speed_hz=(25*1000000);
	    //t[1].cs_change=1;
	    t[1].bits_per_word=16;
	    t[1].cr0=0x73cf;
	    if(nSize&0x1)
	    {

	      t[1].cr1=((nSize+1)>>1)-1;
	    }
	    else
	    {
	      t[1].cr1=(nSize>>1)-1;

	    }
	    t[1].ser=0x0;
	    t[1].ssienr=0x0;

	    
	    
	    spi_message_add_tail(&t[1], &m);
	    
			  
	    status=spi_sync(spi_device, &m);
	    {
		    
		    u16 temp=0;
		    u16 *pTemp=(u16 *)buff;
		    u32 i=0;
		  
		    
		    if(nSize&0x1)
		    {
		      for (i = 0; i<((nSize+1)/2); i++)
		      {	
			temp=*pTemp;
			*pTemp= ((temp&0xFF)<<8) | ((temp&0xFF00)>>8);
			//printk("%04x\n",*pTemp);
			pTemp++;
		      }
		    }
		    else
		    {
		      for (i = 0; i<(nSize/2); i++)
		      {	
			temp=*pTemp;
			*pTemp= ((temp&0xFF)<<8) | ((temp&0xFF00)>>8);
			//printk("%04x\n",*pTemp);
			pTemp++;
		      }
		      
		    }
		    
	
	      
	    }
  
  
    return status;
}

int dw_spi_flash_write(struct spi_device *spi_device,u32 offset,u32 len,u8 *buff,int busnum)
{
  


      int st=0;
//      volatile unsigned long status=0;
      u32	to=offset;
      u32	nSize =len;
      struct spi_transfer t[1];
      u8 *buf=buff;
      struct spi_message m;
      // struct spi_message m_u8[nSize];
      u16 writecmd_u16[16];
      u16 tmpdata_u16=0x00;
      
      u8  writecmd_u8[32];
      u8 tmpdata_u8=0x0;
      u32 count=0;
      
      int i=0;
      	 memset(writecmd_u16,0x0,sizeof(writecmd_u16));   
	 memset(writecmd_u8,0x0,sizeof(writecmd_u8));
     
    spi_message_init(&m);
    memset(t, 0, (sizeof t));
 
      
	  if(nSize&0x1)
	  {
		 
	




	
		
		i=0;
		count=nSize;
		
		while(count)
		{
		    
		    writecmd_u8[0]=SPI_FLASH_INS_PP; 
		    writecmd_u8[1]= (to>>16)&0xff;
		    writecmd_u8[2]=(to>>8)&0xff;
		    writecmd_u8[3]=to&0xff;
		    tmpdata_u8=*(buf++);
		    
		      writecmd_u8[4]=tmpdata_u8;
		   
		      memset(t, 0, (sizeof t));
		    
		     
		    
		      t[0].tx_buf =writecmd_u8;
		      t[0].rx_buf = NULL;
		      t[0].len =5;
		      t[0].bits_per_word=8;
		      t[0].speed_hz=(25*1000000);
		      t[0].cr0=0x71c7;
		      t[0].cr1=0x0;
		      t[0].ser=0x0;
		      t[0].ssienr=0x0;
		      //while(haydn_spiflash_status() & 0x1);  			// wait while WriteInProgress
		      //haydn_write_enable();
		      
		      dw_spi_writeen(spi_device);  
		      {
			// wait while WriteInProgress
			volatile u8 status;
			 dw_spi_status(spi_device,&status, busnum);
			 while(status&0x1){dw_spi_status(spi_device,&status, busnum);}
		      }
		      spi_message_add_tail(&t[0], &m);
		      st=spi_sync(spi_device, &m);
		      if(st!=0) goto exit;
		      i++;
		      count--;
		      to++;
		     {
		       // wait while WriteInProgress
			volatile u8 status;
			 dw_spi_status(spi_device,&status, busnum);
			 while(status&0x1){dw_spi_status(spi_device,&status, busnum);}
		      }
		
		       

		     
		      
		      
		}
	
		    
	  }
	  else
	  {
		     	
		         
		      writecmd_u16[0]=(SPI_FLASH_INS_PP<<8) | ((to & 0x00FF0000) >>16);
		      writecmd_u16[1]=to & 0x0000FFFF;

		       count=nSize;
		     
		      i=2;
		      while(count)
		      {
			tmpdata_u16=*((u16 *)buf);
		
			writecmd_u16[i]=((tmpdata_u16&0xFF)<<8) | ((tmpdata_u16&0xFF00)>>8);
			i++;
			count-=2;
			buf+=2;
			
		      }
		    
		     memset(t, 0, (sizeof t));
		       
		      t[0].tx_buf =writecmd_u16;
		      t[0].rx_buf = NULL;
		      t[0].len =nSize+4;
		      t[0].bits_per_word=16;
		      t[0].speed_hz=(25*1000000);
		      t[0].cr0=0x71cf;
		      t[0].cr1=0x0;
		      t[0].ser=0x0;
		      t[0].ssienr=0x0;
		      {
		       // wait while WriteInProgress
			volatile u8 status;
			 dw_spi_status(spi_device,&status, busnum);
			 while(status&0x1){dw_spi_status(spi_device,&status, busnum);}
		      }
		       dw_spi_writeen(spi_device);  
		        spi_message_add_tail(&t[0], &m);
		      st=spi_sync(spi_device, &m);
		       if(st!=0) goto exit;
		       
		 {
		       // wait while WriteInProgress
			volatile u8 status;
			 dw_spi_status(spi_device,&status, busnum);
			 while(status&0x1){dw_spi_status(spi_device,&status, busnum);}
		      }
		
		       
		       
		   
		

	    
	  }
	   
	    
exit:
if(st==-1)
{
printk("[haydn_spiflash](%d) spi_sync(): status==-1\n",__LINE__);
}   
return st;
    
  
}

#if 0

int dw_spi_flash_write(struct spi_device *spi_device,u32 offset,u32 len,u8 *buff,int busnum)
{
  


      int status=0;
      u32	to=offset;
      u32	nSize =len;
      struct spi_transfer t[1];
      u8 *buf=buff;
      struct spi_message m;
       struct spi_message m_u8[nSize];
      u16 writecmd_u16[16];
      u16 tmpdata_u16=0x00;
      
      u8  writecmd_u8[32];
      u8 tmpdata_u8=0x0;
      u32 count=0;
      
      int i=0;
      	    
      if(nSize > (32-4) )
      {
	  nSize=(32-4);
			
      }
      
      if ((nSize+(to&255)) > 256)
		nSize = 256-(to&255);
      
	//printk("write nSize=%d\n",nSize);
      
      
   
        dw_spi_writeen(spi_device);
     
      
	  if(nSize&0x1)
	  {
		 
	

		for(i=0;i<nSize;i++)
		{
		  spi_message_init(&m_u8[i]);
		  
		}
		
		i=0;
		count=nSize;
		while(count)
		{
		    memset(writecmd_u8,0x0,sizeof(writecmd_u8));
		    writecmd_u8[0]=SPI_FLASH_INS_PP; 
		    writecmd_u8[1]= (to>>16)&0xff;
		    writecmd_u8[2]=(to>>8)&0xff;
		    writecmd_u8[3]=to&0xff;
		    tmpdata_u8=*(buff++);
		    
		      writecmd_u8[4]=tmpdata_u8;
		   
		    memset(t, 0, (sizeof t));
		    
		      
		    
		      t[0].tx_buf =writecmd_u8;
		      t[0].rx_buf = NULL;
		      t[0].len =5;
		      t[0].bits_per_word=8;
		      t[0].speed_hz=(25*1000000);
		      t[0].cr0=0x71c7;
		      t[0].cr1=0;
		      t[0].ser=0;
		      t[0].ssienr=0x0;
		      spi_message_add_tail(&t[0], &m_u8[i]);
		      status=spi_sync(spi_device, &m_u8[i]);
		      if(status!=0) goto exit;
		      i++;
		      count--;
		      to++;
		      {
			 u8 status;
			 dw_spi_status(spi_device,&status, busnum);
			 while(status&0x1){dw_spi_status(spi_device,&status, busnum);}
		      }
		      dw_spi_writeen(spi_device);
		      
		      
		      
		}
	
		    
	  }
	  else
	  {
		       spi_message_init(&m);
		       memset(t, 0, (sizeof t));	
		          memset(writecmd_u16,0x0,sizeof(writecmd_u16));
		      writecmd_u16[0]=(SPI_FLASH_INS_PP<<8) | ((to & 0x00FF0000) >>16);
		      writecmd_u16[1]=to & 0x0000FFFF;

		       count=nSize;
		     
		      i=2;
		      while(count)
		      {
			tmpdata_u16=*((u16 *)buf);
			//printk("tmpdata_u16=%04x\n",tmpdata_u16);
			writecmd_u16[i]=((tmpdata_u16&0xFF)<<8) | ((tmpdata_u16&0xFF00)>>8);
			i++;
			count-=2;
			buf+=2;
			
		      }
		    
		
		      
		      t[0].tx_buf =writecmd_u16;
		      t[0].rx_buf = NULL;
		      t[0].len =nSize+4;
		      t[0].bits_per_word=16;
		      t[0].speed_hz=(25*1000000);
		      t[0].cr0=0x71cf;
		      t[0].cr1=0;
		      t[0].ser=0;
		      t[0].ssienr=0x0;
		      spi_message_add_tail(&t[0], &m);
		      status=spi_sync(spi_device, &m);
	  }
	   {
			 u8 status;
			 dw_spi_status(spi_device,&status, busnum);
			 while(status&0x1){dw_spi_status(spi_device,&status, busnum);}
	    }
	    
exit:
    return status;
    
  
}
#endif
int dw_spi_flash_erase(struct spi_device *spi_device,u32 offset,int busnum)
{
  int status=0;
  //Flash erase
  struct spi_transfer t[1];
  struct spi_message m;
  u8 erasecmd[4];
  u32 erase_addr=offset;
  
  spi_message_init(&m);
  memset(t, 0, (sizeof t));
  memset(erasecmd,0x0,sizeof(erasecmd));
     
  dw_spi_writeen(spi_device);
  erasecmd[0]= SPI_FLASH_INS_SE; //sector erase command  
  erasecmd[1]= (erase_addr>>16)&0xff;
  erasecmd[2]=(erase_addr>>8)&0xff;
  erasecmd[3]=erase_addr&0xff;
  t[0].tx_buf = erasecmd;
  t[0].rx_buf = NULL;
  t[0].len = sizeof(erasecmd);
  t[0].bits_per_word=8;
  t[0].speed_hz=(25*1000000);
  t[0].cr0=0x71c7;
  t[0].cr1=0;
  t[0].ser=0;
  t[0].ssienr=0x0;
  
  spi_message_add_tail(&t[0], &m);
  
  status=spi_sync(spi_device, &m);
  {
    volatile u8 status;
    dw_spi_status(spi_device,&status, busnum);
    while(status&0x1){msleep(25);dw_spi_status(spi_device,&status, busnum);}
    
  }
  return status;
}
int dw_spi_writeen(struct spi_device *spi_device)
{
      //Flash writeen
      u8 writeencmd[1];
      int status=0;
      
      struct spi_transfer t[1];
      struct spi_message m;
      
      spi_message_init(&m);
      memset(writeencmd,0x0,sizeof(writeencmd));
      memset(t, 0, (sizeof t));
      
      writeencmd[0]= SPI_FLASH_INS_WREN;
      t[0].tx_buf = writeencmd;
      t[0].rx_buf = NULL;
      t[0].len = 1;
      t[0].bits_per_word=8;
      t[0].speed_hz=(25*1000000);
      t[0].cr0=0x71c7;
      t[0].cr1=0;
      t[0].ser=0;
      t[0].ssienr=0x0;
	
      spi_message_add_tail(&t[0], &m);
      status=spi_sync(spi_device, &m);
      return status;
  
}

int dw_spi_status(struct spi_device *spi_device,u8 *ptstatus, int busnum)
{
    
      
       //flash read status
      u8 statuscmd[1];
      u8 status[1];
      int st=0;
      struct spi_transfer t[2];
      struct spi_message m;
   
      spi_message_init(&m);
      memset(statuscmd,0x0,sizeof(statuscmd));
      memset(status,0x0,sizeof(status));
      memset(t, 0, (sizeof t));

    
      statuscmd[0]= SPI_FLASH_INS_RDSR;
      t[0].tx_buf = statuscmd;
      t[0].rx_buf = NULL;
      t[0].len = 1;
      t[0].bits_per_word=8;
      t[0].speed_hz=(25*1000000);
      t[0].cr0=0x73c7;
      t[0].cr1=sizeof(status)-1;
      t[0].ser=1 << busnum;
      t[0].ssienr=0x1;
	
      spi_message_add_tail(&t[0], &m);
       
      
      t[1].tx_buf = NULL;
      t[1].rx_buf = status;
	t[1].len = sizeof(status);
	 t[1].bits_per_word=8;
	t[1].speed_hz=(25*1000000);
	 t[1].cr0=0x73c7;
	t[1].cr1=sizeof(status)-1;
	t[1].ser=0x0;
	t[1].ssienr=0x0;
	spi_message_add_tail(&t[1], &m);
      
      
      
      
      st=spi_sync(spi_device, &m);
      
      *ptstatus=status[0];
      
      return st;
      
}






static unsigned char BitReverse(unsigned char byData)
{
	unsigned char byOut;

	byOut = ((byData<<7)&0x80) | ((byData<<5)&0x40) | ((byData<<3)&0x20) | ((byData<<1)&0x10) |
			((byData>>1)&0x08) | ((byData>>3)&0x04) | ((byData>>5)&0x02) | ((byData>>7)&0x01);

	return byOut;
}




int dw_spi_readID(struct spi_device *spi_device,u8 *id,int busnum)
{
   
      //Flash ReadID
      u8 idcode[3];
      u8 readidcmd[1];

       int st=0;
      struct spi_transfer t[2];
      struct spi_message m;  
      
      spi_message_init(&m);
      memset(idcode,0x0,sizeof(idcode));
      memset(t, 0, (sizeof t));
      
      readidcmd[0]=0x9f;
      t[0].tx_buf = readidcmd;
      t[0].rx_buf = NULL;
      t[0].len = 1;
      t[0].bits_per_word=8;
      t[0].speed_hz=(25*1000000);
      t[0].cr0=0x73c7;
      t[0].cr1=sizeof(idcode)-1;
      t[0].ser=1 <<  busnum;
      t[0].ssienr=0x1;
	
      spi_message_add_tail(&t[0], &m);
	
    
      t[1].tx_buf = NULL;
      t[1].rx_buf = idcode;
	t[1].len = sizeof(idcode);
	 t[1].bits_per_word=8;
	t[1].speed_hz=(25*1000000);
	 t[1].cr0=0x73c7;
	t[1].cr1=sizeof(idcode)-1;
	t[1].ser=0x0;
	t[1].ssienr=0x0;
	spi_message_add_tail(&t[1], &m);
	
	st=spi_sync(spi_device, &m);
	memset(id,0x0,sizeof(idcode));
	memcpy(id,idcode,sizeof(idcode));
	return st;
      
   
  
}

void Test_Flash_RW(void)
{
  
 /*
  1. Erase 1-blk (64 KB)
  2. Write 127 bytes from 0x00900000. The data value is 0x5a.
  3. Read 127 bytes from 0x00900000.
  */ 

 u8* tmpbuf_w;
 u8* tmpbuf_r;
 size_t retlen;
 loff_t to=0x00900000;
 loff_t from=to;
 u32 erase_addr=0;
 size_t len=127;
 int tmp_erase_len=0;
 size_t tmplen=0;
 u8 buf[USER_BUFF_SIZE];
 u8 readbuf[USER_BUFF_SIZE];
 int i=0;
 u32 nSize=0;
 size_t blkSize=64*1024;
 int odd_len=0;
 
 memset(readbuf,0x0,sizeof(readbuf));
 memset(buf,0x0,sizeof(buf));
 for(i=0;i<sizeof(buf);i++)
 {
      buf[i]=0x5a;
 }
 tmpbuf_w=buf;
 tmpbuf_r=readbuf;
 //Erase 64k Byte (1-block)
 
 erase_addr=to;
 tmp_erase_len=(int)len;
 
 while(tmp_erase_len > 0)
 {
 
  printk("erase_addr=%08x erase_len=%d\n",erase_addr,tmp_erase_len);
  dw_spi_flash_erase(spike_dev.spi_device,erase_addr,SPI_BUS_CS0);
  erase_addr+=blkSize;
  tmp_erase_len-=blkSize;
  
  
 }
 
 tmplen=len;
  do {
	nSize = (len > 14*2) ? 14*2 : len;
	
	if ((nSize+(to&255)) > 256)
			nSize = 256-(to&255);
	odd_len = (nSize&0x1) ? 1 : 0;
	if (odd_len) 
	{
	  nSize=1;
	
	}
    	dw_spi_flash_write(spike_dev.spi_device,to,nSize,tmpbuf_w,SPI_BUS_CS0);
	
	//dw_spi_flash_read(spike_dev.spi_device,to,nSize,tmpbuf_r,SPI_BUS_CS0);
	
	tmpbuf_w+=nSize;
	//tmpbuf_r+=nSize;
	to += nSize;
	len -= nSize;
	retlen += nSize;
  } while (len > 0);
  
  
  from=0x00900000;
  len=127;
  retlen=0;
  
  do{	
		nSize = (len > 16*2) ? 16*2 : len; // FIFO depth is 16. FIFO width is 16 bits (2 byte).

       
        
 

			
	
	    dw_spi_flash_read(spike_dev.spi_device,from,nSize,tmpbuf_r,SPI_BUS_CS0);
	    tmpbuf_r+=nSize;
	    len -= nSize;
	    from += nSize;
	    retlen += nSize;
	}while (len > 0);
  
  
  
  
  
#if 1
    for(i=0;i<tmplen;i++)
    {
      if((i%16==0) && (i!=0))
      {
	printk("\n");
      }
      printk("%02x, ",readbuf[i]);
      
    }
    printk("\n");
#endif  
    

    
  return;
}


int dw_spi_mn34031pl_write(struct spi_device *spi_device,u16 address,u16 data,int busnum)
{ 


 int st=0;
     struct spi_transfer t[2];
     struct spi_message m;  
      u8 writecmd[4];
     
    
      
     spi_message_init(&m);
     memset(t, 0, (sizeof t));
     
  
      memset(writecmd,0x0,sizeof(writecmd));
      writecmd[0]=BitReverse((u8)(address & 0xff));
      writecmd[1] =BitReverse((u8)((address >> 8) & 0xff));
      writecmd[2] =BitReverse(data&0xff);
      writecmd[3] =BitReverse((data>>8)&0xff);
      //printk("writecmd[0]=%02x,writecmd[1]=%02x,writecmd[2]=%02x,writecmd[3]=%02x\n",writecmd[0],writecmd[1],writecmd[2],writecmd[3]);
      t[0].tx_buf = writecmd;
      t[0].rx_buf = NULL;
      t[0].len = 4;
      t[0].bits_per_word=8;
      #if defined(TEST_MN34041PL)
	t[0].speed_hz=1000000;
      #else
	t[0].speed_hz=9000000;
      #endif
      t[0].cr0=0x71c7;
      t[0].cr1=0x0;
      t[0].ser=1 <<  busnum;
      t[0].ssienr=0x0;
     spi_message_add_tail(&t[0], &m);
  st=spi_sync(spi_device, &m);
  return st;

  
}

int dw_spi_mn34031pl_read(struct spi_device *spi_device,u16 address,u16 *data,int busnum)
{
   int st=0;
   struct spi_transfer t[2];
   struct spi_message m;  
   u8 readcmd[2];
   u8 readdata[2];
  
        
     spi_message_init(&m);
     memset(t, 0, (sizeof t));
      
        memset(readdata,0x0,sizeof(readdata));
      memset(readcmd,0x0,sizeof(readcmd));


	readcmd[0] =BitReverse((address|0x8000) & 0xff);
	readcmd[1] =BitReverse(((address|0x8000)>>8) & 0xff);
  
      t[0].tx_buf = readcmd;
      t[0].rx_buf = readdata;
      t[0].len = 2;
      t[0].bits_per_word=8;
      #if defined(TEST_MN34041PL)
	t[0].speed_hz=(1000000);
      #else
	t[0].speed_hz=(9000000);
      #endif
      t[0].cr0=0x73c7;
      t[0].cr1=2-1;
      t[0].ser=1 <<  busnum;
      t[0].ssienr=0x0;
     spi_message_add_tail(&t[0], &m);
     
      st=spi_sync(spi_device, &m);
      *(u8 *)data= BitReverse(readdata[0]);
      *((u8 *)data+1)=BitReverse(readdata[1]);
     
  return st;
}

#define RW_LEN 28
#if defined(TEST_MN34041PL)
#define MN34041PL_SERIAL_TBL_SIZE 382
typedef enum MN34041PL_registers
{
	_0000_	=	0x0000,
	_0001_	=	0x0001,
	_0002_	=	0x0002,
	_0003_	=	0x0003,
	_0004_	=	0x0004,
	_0005_	=	0x0005,
	_0006_	=	0x0006,
	_0020_	=	0x0020,
	_0021_	=	0x0021,
	_0022_	=	0x0022,
	_0023_	=	0x0023,
	_0024_	=	0x0024,
	_0025_	=	0x0025,
	_0026_	=	0x0026,
	_0027_	=	0x0027,
	_0028_	=	0x0028,
	_0030_	=	0x0030,
	_0031_	=	0x0031,
	_0032_	=	0x0032,
	_0033_	=	0x0033,
	_0034_	=	0x0034,
	_0035_	=	0x0035,
	_0036_	=	0x0036,
	_0037_	=	0x0037,
	_0038_	=	0x0038,
	_0039_	=	0x0039,
	_003A_	=	0x003A,
	_003B_	=	0x003B,
	_003F_	=	0x003F,
	_0040_	=	0x0040,
	_0041_	=	0x0041,
	_0042_	=	0x0042,
	_0043_	=	0x0043,
	_0044_	=	0x0044,
	_0045_	=	0x0045,
	_0046_	=	0x0046,
	_0047_	=	0x0047,
	_0048_	=	0x0048,
	_0049_	=	0x0049,
	_004A_	=	0x004A,
	_004B_	=	0x004B,
	_0056_	=	0x0056,
	_0057_	=	0x0057,
	_0058_	=	0x0058,
	_0059_	=	0x0059,
	_005A_	=	0x005A,
	_0070_	=	0x0070,
	_0071_	=	0x0071,
	_0072_	=	0x0072,
	_0073_	=	0x0073,
	_0074_	=	0x0074,
	_0075_	=	0x0075,
	_0076_	=	0x0076,
	_0077_	=	0x0077,
	_0078_	=	0x0078,
	_0079_	=	0x0079,
	_0080_	=	0x0080,
	_0081_	=	0x0081,
	_0082_	=	0x0082,
	_0083_	=	0x0083,
	_0084_	=	0x0084,
	_0085_	=	0x0085,
	_0086_	=	0x0086,
	_0087_	=	0x0087,
	_0088_	=	0x0088,
	_0089_	=	0x0089,
	_008A_	=	0x008A,
	_008B_	=	0x008B,
	_008C_	=	0x008C,
	_008D_	=	0x008D,
	_008E_	=	0x008E,
	_008F_	=	0x008F,
	_0090_	=	0x0090,
	_0091_	=	0x0091,
	_0092_	=	0x0092,
	_0093_	=	0x0093,
	_0094_	=	0x0094,
	_0095_	=	0x0095,
	_0096_	=	0x0096,
	_0097_	=	0x0097,
	_00A0_	=	0x00A0,
	_00A1_	=	0x00A1,
	_00A2_	=	0x00A2,
	_00A3_	=	0x00A3,
	_00A4_	=	0x00A4,
	_00A5_	=	0x00A5,
	_00A6_	=	0x00A6,
	_00A7_	=	0x00A7,
	_00A8_	=	0x00A8,
	_00A9_	=	0x00A9,
	_00C0_	=	0x00C0,
	_00C1_	=	0x00C1,
	_00C2_	=	0x00C2,
	_00C3_	=	0x00C3,
	_00C4_	=	0x00C4,
	_00C5_	=	0x00C5,
	_00C6_	=	0x00C6,
	_00C7_	=	0x00C7,
	_00CA_	=	0x00CA,
	_00CB_	=	0x00CB,
	_00CC_	=	0x00CC,
	_00CD_	=	0x00CD,
	_00CE_	=	0x00CE,
	_0100_	=	0x0100,
	_0101_	=	0x0101,
	_0102_	=	0x0102,
	_0103_	=	0x0103,
	_0104_	=	0x0104,
	_0105_	=	0x0105,
	_0106_	=	0x0106,
	_0108_	=	0x0108,
	_0109_	=	0x0109,
	_010A_	=	0x010A,
	_010B_	=	0x010B,
	_010C_	=	0x010C,
	_010D_	=	0x010D,
	_010E_	=	0x010E,
	_010F_	=	0x010F,
	_0110_	=	0x0110,
	_0111_	=	0x0111,
	_0112_	=	0x0112,
	_0113_	=	0x0113,
	_0114_	=	0x0114,
	_0115_	=	0x0115,
	_0116_	=	0x0116,
	_0117_	=	0x0117,
	_0118_	=	0x0118,
	_0119_	=	0x0119,
	_011A_	=	0x011A,
	_011B_	=	0x011B,
	_011C_	=	0x011C,
	_011D_	=	0x011D,
	_011E_	=	0x011E,
	_011F_	=	0x011F,
	_0120_	=	0x0120,
	_0121_	=	0x0121,
	_0122_	=	0x0122,
	_0123_	=	0x0123,
	_0124_	=	0x0124,
	_0125_	=	0x0125,
	_0126_	=	0x0126,
	_0127_	=	0x0127,
	_0128_	=	0x0128,
	_0129_	=	0x0129,
	_012A_	=	0x012A,
	_012B_	=	0x012B,
	_012C_	=	0x012C,
	_012D_	=	0x012D,
	_012E_	=	0x012E,
	_012F_	=	0x012F,
	_0130_	=	0x0130,
	_0131_	=	0x0131,
	_0132_	=	0x0132,
	_0133_	=	0x0133,
	_0134_	=	0x0134,
	_0135_	=	0x0135,
	_0136_	=	0x0136,
	_0137_	=	0x0137,
	_0138_	=	0x0138,
	_0139_	=	0x0139,
	_013A_	=	0x013A,
	_0140_	=	0x0140,
	_0141_	=	0x0141,
	_0142_	=	0x0142,
	_0143_	=	0x0143,
	_0144_	=	0x0144,
	_0145_	=	0x0145,
	_0146_	=	0x0146,
	_0150_	=	0x0150,
	_0151_	=	0x0151,
	_0152_	=	0x0152,
	_0153_	=	0x0153,
	_0154_	=	0x0154,
	_0155_	=	0x0155,
	_0156_	=	0x0156,
	_0157_	=	0x0157,
	_0158_	=	0x0158,
	_0159_	=	0x0159,
	_015A_	=	0x015A,
	_015B_	=	0x015B,
	_015C_	=	0x015C,
	_015D_	=	0x015D,
	_015E_	=	0x015E,
	_015F_	=	0x015F,
	_0160_	=	0x0160,
	_0161_	=	0x0161,
	_0162_	=	0x0162,
	_0163_	=	0x0163,
	_0164_	=	0x0164,
	_0165_	=	0x0165,
	_0166_	=	0x0166,
	_0167_	=	0x0167,
	_0168_	=	0x0168,
	_0169_	=	0x0169,
	_016A_	=	0x016A,
	_016B_	=	0x016B,
	_0170_	=	0x0170,
	_0171_	=	0x0171,
	_0172_	=	0x0172,
	_0173_	=	0x0173,
	_0174_	=	0x0174,
	_0175_	=	0x0175,
	_0176_	=	0x0176,
	_0177_	=	0x0177,
	_0178_	=	0x0178,
	_0179_	=	0x0179,
	_017A_	=	0x017A,
	_017B_	=	0x017B,
	_017C_	=	0x017C,
	_0180_	=	0x0180,
	_0181_	=	0x0181,
	_0182_	=	0x0182,
	_0183_	=	0x0183,
	_0184_	=	0x0184,
	_0185_	=	0x0185,
	_0186_	=	0x0186,
	_0187_	=	0x0187,
	_0188_	=	0x0188,
	_0189_	=	0x0189,
	_018A_	=	0x018A,
	_018B_	=	0x018B,
	_018C_	=	0x018C,
	_018D_	=	0x018D,
	_0190_	=	0x0190,
	_0191_	=	0x0191,
	_0192_	=	0x0192,
	_0193_	=	0x0193,
	_0194_	=	0x0194,
	_0195_	=	0x0195,
	_0196_	=	0x0196,
	_0197_	=	0x0197,
	_0198_	=	0x0198,
	_0199_	=	0x0199,
	_019A_	=	0x019A,
	_019B_	=	0x019B,
	_019C_	=	0x019C,
	_019D_	=	0x019D,
	_019E_	=	0x019E,
	_01A0_	=	0x01A0,
	_01A1_	=	0x01A1,
	_01A2_	=	0x01A2,
	_01A3_	=	0x01A3,
	_01A4_	=	0x01A4,
	_01A5_	=	0x01A5,
	_01A6_	=	0x01A6,
	_01A7_	=	0x01A7,
	_01A8_	=	0x01A8,
	_01A9_	=	0x01A9,
	_01AA_	=	0x01AA,
	_01AB_	=	0x01AB,
	_01AC_	=	0x01AC,
	_01AD_	=	0x01AD,
	_01AE_	=	0x01AE,
	_01AF_	=	0x01AF,
	_01B0_	=	0x01B0,
	_01B1_	=	0x01B1,
	_01B2_	=	0x01B2,
	_01B3_	=	0x01B3,
	_01B4_	=	0x01B4,
	_01B5_	=	0x01B5,
	_01B6_	=	0x01B6,
	_01B7_	=	0x01B7,
	_01B8_	=	0x01B8,
	_01B9_	=	0x01B9,
	_01BA_	=	0x01BA,
	_01BB_	=	0x01BB,
	_01BC_	=	0x01BC,
	_01BD_	=	0x01BD,
	_01BE_	=	0x01BE,
	_01C4_	=	0x01C4,
	_01C5_	=	0x01C5,
	_01C6_	=	0x01C6,
	_01D0_	=	0x01D0,
	_01D1_	=	0x01D1,
	_01D2_	=	0x01D2,
	_01D3_	=	0x01D3,
	_01D4_	=	0x01D4,
	_01D5_	=	0x01D5,
	_01D6_	=	0x01D6,
	_01D7_	=	0x01D7,
	_01D8_	=	0x01D8,
	_01D9_	=	0x01D9,
	_01DA_	=	0x01DA,
	_01DB_	=	0x01DB,
	_01DC_	=	0x01DC,
	_01DD_	=	0x01DD,
	_01DE_	=	0x01DE,
	_01DF_	=	0x01DF,
	_01E0_	=	0x01E0,
	_01E1_	=	0x01E1,
	_01E2_	=	0x01E2,
	_01E3_	=	0x01E3,
	_01E4_	=	0x01E4,
	_01E5_	=	0x01E5,
	_01E6_	=	0x01E6,
	_01E7_	=	0x01E7,
	_01E8_	=	0x01E8,
	_01E9_	=	0x01E9,
	_01EA_	=	0x01EA,
	_01EB_	=	0x01EB,
	_01EC_	=	0x01EC,
	_01ED_	=	0x01ED,
	_01EE_	=	0x01EE,
	_01EF_	=	0x01EF,
	_01F0_	=	0x01F0,
	_01F1_	=	0x01F1,
	_01F2_	=	0x01F2,
	_01F3_	=	0x01F3,
	_01F4_	=	0x01F4,
	_01F5_	=	0x01F5,
	_01F6_	=	0x01F6,
	_0200_	=	0x0200,
	_0201_	=	0x0201,
	_0202_	=	0x0202,
	_0203_	=	0x0203,
	_0204_	=	0x0204,
	_0205_	=	0x0205,
	_0206_	=	0x0206,
	_0270_	=	0x0270,
	_0271_	=	0x0271,
	_0272_	=	0x0272,
	_0273_	=	0x0273,
	_0274_	=	0x0274,
	_0275_	=	0x0275,
	_0276_	=	0x0276,
	_0277_	=	0x0277,
	_0278_	=	0x0278,
	_0279_	=	0x0279,
	_027A_	=	0x027A,
	_027B_	=	0x027B,
	_027C_	=	0x027C,
	_0290_	=	0x0290,
	_0291_	=	0x0291,
	_0292_	=	0x0292,
	_0293_	=	0x0293,
	_0294_	=	0x0294,
	_0295_	=	0x0295,
	_0296_	=	0x0296,
	_0297_	=	0x0297,
	_0298_	=	0x0298,
	_0299_	=	0x0299,
	_029A_	=	0x029A,
	_029B_	=	0x029B,
	_029C_	=	0x029C,
	_029D_	=	0x029D,
	_029E_	=	0x029E,
	_02A0_	=	0x02A0,
	_02A1_	=	0x02A1,
	_02A2_	=	0x02A2,
	_02A3_	=	0x02A3,
	_02A4_	=	0x02A4,
	_02A5_	=	0x02A5,
	_02A6_	=	0x02A6,
	_02A7_	=	0x02A7,
	_02A8_	=	0x02A8,
	_02A9_	=	0x02A9,
	_02AA_	=	0x02AA,
	_02AB_	=	0x02AB,
	_02AC_	=	0x02AC,
	_02AD_	=	0x02AD,
	_02AE_	=	0x02AE,
	_02AF_	=	0x02AF,
	_02B0_	=	0x02B0,
	_02B1_	=	0x02B1,
	_02B2_	=	0x02B2,
	_02B3_	=	0x02B3,
	_02B4_	=	0x02B4,
	_02B5_	=	0x02B5,
	_02B6_	=	0x02B6,
	_02B7_	=	0x02B7,
	_02B8_	=	0x02B8,
	_02B9_	=	0x02B9,
	_02BA_	=	0x02BA,
	_02BB_	=	0x02BB,
	_02BC_	=	0x02BC,
	_02BD_	=	0x02BD,
	_02BE_	=	0x02BE,
	_02C4_	=	0x02C4,
	_02C5_	=	0x02C5,
	_02C6_	=	0x02C6,
} EMN34041PLRegs;
typedef struct MN34041PL_reg_addr_data
{
    EMN34041PLRegs eRegAddr;
    unsigned long  dwData;
} TMN34041PLRegAddrData;

const TMN34041PLRegAddrData atMN34041PLSerialRegTbl[MN34041PL_SERIAL_TBL_SIZE] =
{
	{_0001_,	0x0036},
	{_0002_,	0x0002},
	{_0004_,	0x01C2},
	{_0000_,	0x0003},
	{_0003_,	0x0209},
	{_0006_,	0x442A},
	{_0005_,	0x0000},
		
	{_0000_,	0x0013},
	{_0036_,	0x0021},
	{_0037_,	0x0300},
		
	{_0020_,	0x0080},
	{_0021_,	0x0080},
	{_0022_,	0x0000},
	{_0023_,	0x0000},
	{_0024_,	0x0010},
	{_0025_,	0x0011},
	{_0026_,	0x0012},
	{_0027_,	0x0013},
	{_0028_,	0x0000},
	{_0030_,	0x0424},
	{_0031_,	0x110A},
	{_0032_,	0x7450},
	{_0033_,	0x0000},
	{_0034_,	0x0000},
	{_0035_,	0x0281},
	
	{_0038_,	0x0001},
	{_0039_,	0x0210},
	{_003A_,	0x0333},
	{_003B_,	0x0111},
	{_003F_,	0x0000},
	{_0040_,	0x0004},
	{_0041_,	0x0200},
	{_0042_,	0x0100},
/* Version 1.0.0.1 modification, 2010.08.17 */
	{_0043_,	0x0071},
/* ========================== */
	{_0044_,	0x0000},
	{_0045_,	0x0000},
	{_0046_,	0x0000},
	{_0047_,	0x0010},
	{_0048_,	0x0000},
	{_0049_,	0x0002},
	{_004A_,	0x0FFE},
	{_004B_,	0x0004},
	{_0056_,	0x0000},
	{_0057_,	0x1FFF},
	{_0058_,	0x2000},
	{_0059_,	0x0000},
	{_005A_,	0x0011},
	{_0070_,	0x2084},
	{_0071_,	0xFFFF},
	{_0072_,	0x0000},
	{_0073_,	0x0000},
	{_0074_,	0x0000},
	{_0075_,	0x0004},
	{_0076_,	0x0230},
	{_0077_,	0x0541},
	{_0078_,	0x0001},
	{_0079_,	0x0011},
	{_0080_,	0x0002},
	{_0081_,	0x0003},
	{_0082_,	0x0000},
	{_0083_,	0x0001},
	{_0084_,	0x0012},
	{_0085_,	0x0013},
	{_0086_,	0x0010},
	{_0087_,	0x0011},
	{_0088_,	0x000A},
	{_0089_,	0x000B},
	{_008A_,	0x0008},
	{_008B_,	0x0009},
	{_008C_,	0x0006},
	{_008D_,	0x0007},
	{_008E_,	0x0004},
	{_008F_,	0x0005},
	{_0090_,	0x0016},
	{_0091_,	0x0017},
	{_0092_,	0x0014},
	{_0093_,	0x0015},
	{_0094_,	0x001A},
	{_0095_,	0x001B},
	{_0096_,	0x0018},
	{_0097_,	0x0019},
	{_00A0_,	0x3000},
	{_00A1_,	0x0000},
	{_00A2_,	0x0002},
	{_00A3_,	0x0000},
	{_00A4_,	0x0000},
	{_00A5_,	0x0000},
	{_00A6_,	0x0000},
	{_00A7_,	0x0000},
	{_00A8_,	0x000F},
	{_00A9_,	0x0022},
	{_00C0_,	0x5540},
	{_00C1_,	0x5FD5},
	{_00C2_,	0xF757},
	{_00C3_,	0xDF5F},
	{_00C4_,	0x208A},
	{_00C5_,	0x0071},
	{_00C6_,	0x0557},
	{_00C7_,	0x0000},
	{_00CA_,	0x0080},
	{_00CB_,	0x0000},
	{_00CC_,	0x0000},
	{_00CD_,	0x0000},
	{_00CE_,	0x0000},
	{_0100_,	0x03A8},
	{_0101_,	0x03A8},
	{_0102_,	0x02C0},
	{_0103_,	0x037A},
	{_0104_,	0x002B},
	{_0105_,	0x00DE},
	{_0106_,	0x00FA},
	{_0170_,	0x0002},
	{_0171_,	0x000D},
	{_0172_,	0x0007},
	{_0173_,	0x0004},
	{_0174_,	0x002A},
	{_0175_,	0x0062},
	{_0176_,	0x0079},
	{_0177_,	0x0326},
	{_0178_,	0x0003},
	{_0179_,	0x0033},
	{_017A_,	0x0360},
	{_017B_,	0x0002},
	{_017C_,	0x000D},
	{_0190_,	0x0000},
	{_0191_,	0x0000},
	{_0192_,	0x0000},
	{_0193_,	0x0000},
	{_0194_,	0x0000},
	{_0195_,	0x0000},
	{_0196_,	0x0000},
	{_0197_,	0x01BA},
	{_0198_,	0xB060},
	{_0199_,	0x7C0A},
	{_019A_,	0x0000},
	{_019B_,	0x0313},
	{_019C_,	0x0B08},
	{_019D_,	0x3906},
	{_019E_,	0x0000},
	{_01A0_,	0x0464},
	{_01A1_,	0x0000},
	{_01A2_,	0x0000},
	{_01A3_,	0x0464},
	{_01A4_,	0x0000},
	{_01A5_,	0x0453},
	{_01A6_,	0x0000},
	{_01A7_,	0x0464},
	{_01A8_,	0x0000},
	{_01A9_,	0x0454},
	{_01AA_,	0x0000},
	{_01AB_,	0x0000},
	{_01AC_,	0x0000},
	{_01AD_,	0x0454},
	{_01AE_,	0x0000},
	{_01AF_,	0x0000},
	{_01B0_,	0x0000},
	{_01B1_,	0x0454},
	{_01B2_,	0x0000},
	{_01B3_,	0x0000},
	{_01B4_,	0x0000},
	{_01B5_,	0x0454},
	{_01B6_,	0x0000},
	{_01B7_,	0x0000},
	{_01B8_,	0x0000},
	{_01B9_,	0x0453},
	{_01BA_,	0x0000},
	{_01BB_,	0x0000},
	{_01BC_,	0x0000},
	{_01BD_,	0x0453},
	{_01BE_,	0x0000},
	{_01C4_,	0x0000},
	{_01C5_,	0x0000},
	{_01C6_,	0x0011},
	{_0200_,	0x03A8},
	{_0201_,	0x03A8},
	{_0202_,	0x02C0},
	{_0203_,	0x037A},
	{_0204_,	0x002B},
	{_0205_,	0x00DE},
	{_0206_,	0x00FA},
	{_0270_,	0x0002},
	{_0271_,	0x000D},
	{_0272_,	0x0007},
	{_0273_,	0x0004},
	{_0274_,	0x002A},
	{_0275_,	0x0062},
	{_0276_,	0x0079},
	{_0277_,	0x0326},
	{_0278_,	0x0003},
	{_0279_,	0x0033},
	{_027A_,	0x0360},
	{_027B_,	0x0002},
	{_027C_,	0x000D},
	{_0290_,	0x0000},
	{_0291_,	0x0000},
	{_0292_,	0x0000},
	{_0293_,	0x0000},
	{_0294_,	0x0000},
	{_0295_,	0x0000},
	{_0296_,	0x0000},
	{_0297_,	0x01BA},
	{_0298_,	0xB060},
	{_0299_,	0x7C0A},
	{_029A_,	0x0000},
	{_029B_,	0x0313},
	{_029C_,	0x0B08},
	{_029D_,	0x3906},
	{_029E_,	0x0000},
	{_02A0_,	0x0464},
	{_02A1_,	0x0000},
	{_02A2_,	0x0000},
	{_02A3_,	0x0464},
	{_02A4_,	0x0000},
	{_02A5_,	0x0453},
	{_02A6_,	0x0000},
	{_02A7_,	0x0464},
	{_02A8_,	0x0000},
	{_02A9_,	0x0454},
	{_02AA_,	0x0000},
	{_02AB_,	0x0000},
	{_02AC_,	0x0000},
	{_02AD_,	0x0454},
	{_02AE_,	0x0000},
	{_02AF_,	0x0000},
	{_02B0_,	0x0000},
	{_02B1_,	0x0454},
	{_02B2_,	0x0000},
	{_02B3_,	0x0000},
	{_02B4_,	0x0000},
	{_02B5_,	0x0454},
	{_02B6_,	0x0000},
	{_02B7_,	0x0000},
	{_02B8_,	0x0000},
	{_02B9_,	0x0453},
	{_02BA_,	0x0000},
	{_02BB_,	0x0000},
	{_02BC_,	0x0000},
	{_02BD_,	0x0453},
	{_02BE_,	0x0000},
	{_02C4_,	0x0000},
	{_02C5_,	0x0000},
	{_02C6_,	0x0011},
	{_0108_,	0x0000},
	{_0109_,	0x000F},
	{_010A_,	0x0009},
	{_010B_,	0x0000},
	{_010C_,	0x0016},
	{_010D_,	0x0000},
	{_010E_,	0x000F},
	{_010F_,	0x0000},
	{_0110_,	0x0009},
	{_0111_,	0x0000},
	{_0112_,	0x0016},
	{_0113_,	0x0003},
	{_0114_,	0x000A},
	{_0115_,	0x0000},
	{_0116_,	0x0009},
	{_0117_,	0x0000},
	{_0118_,	0x0016},
	{_0119_,	0x0018},
	{_011A_,	0x0017},
	{_011B_,	0x0000},
	{_011C_,	0x0002},
	{_011D_,	0x0009},
	{_011E_,	0x0012},
	{_011F_,	0x0001},
	{_0120_,	0x003A},
	{_0121_,	0x0000},
	{_0122_,	0x0000},
	{_0123_,	0x0000},
	{_0124_,	0x0011},
	{_0125_,	0x0000},
	{_0126_,	0x0003},
	{_0127_,	0x0003},
	{_0128_,	0x0000},
	{_0129_,	0x0010},
	{_012A_,	0x0000},
	{_012B_,	0x0003},
	{_012C_,	0x0000},
	{_012D_,	0x0011},
	{_012E_,	0x0000},
	{_012F_,	0x0009},
	{_0130_,	0x0009},
	{_0131_,	0x0012},
	{_0132_,	0x0000},
	{_0133_,	0x0000},
	{_0134_,	0x0009},
	{_0135_,	0x0009},
	{_0136_,	0x0012},
	{_0137_,	0x0006},
	{_0138_,	0x0000},
	{_0139_,	0x0010},
	{_013A_,	0x0000},
	{_0140_,	0x0020},
	{_0141_,	0x0036},
	{_0142_,	0x0000},
	{_0143_,	0x0001},
	{_0144_,	0x0003},
	{_0145_,	0x0000},
	{_0146_,	0x0000},
	{_0150_,	0x0011},
	{_0151_,	0x0001},
	{_0152_,	0x0001},
	{_0153_,	0x0001},
	{_0154_,	0x0010},
	{_0155_,	0x0000},
	{_0156_,	0x0003},
	{_0157_,	0x0000},
	{_0158_,	0x0013},
	{_0159_,	0x0000},
	{_015A_,	0x0000},
	{_015B_,	0x0008},
	{_015C_,	0x0000},
	{_015D_,	0x0004},
	{_015E_,	0x0005},
	{_015F_,	0x000A},
	{_0160_,	0x0006},
	{_0161_,	0x000F},
	{_0162_,	0x000D},
	{_0163_,	0x0004},
	{_0164_,	0x0002},
	{_0165_,	0x0002},
	{_0166_,	0x0000},
	{_0167_,	0x0010},
	{_0168_,	0x0009},
	{_0169_,	0x0003},
	{_016A_,	0x0000},
	{_016B_,	0x0000},
	{_0180_,	0x0017},
	{_0181_,	0x00C5},
	{_0182_,	0x0000},
	{_0183_,	0x0000},
	{_0184_,	0x00FA},
	{_0185_,	0x00A5},
	{_0186_,	0x01EF},
	{_0187_,	0x03D9},
	{_0188_,	0x01FB},
	{_0189_,	0x035F},
	{_018A_,	0x0000},
	{_018B_,	0x0000},
	{_018C_,	0x0000},
	{_018D_,	0x0000},
	{_01D0_,	0x0000},
	{_01D1_,	0x0000},
	{_01D2_,	0x0000},
	{_01D3_,	0x0000},
	{_01D4_,	0x0000},
	{_01D5_,	0x0000},
	{_01D6_,	0x0000},
	{_01D7_,	0x0000},
	{_01D8_,	0x0006},
	{_01D9_,	0x0005},
	{_01DA_,	0x0001},
	{_01DB_,	0x0006},
	{_01DC_,	0x0001},
	{_01DD_,	0x0007},
	{_01DE_,	0x0001},
	{_01DF_,	0x0002},
	{_01E0_,	0x0001},
	{_01E1_,	0x0001},
	{_01E2_,	0x00C9},
	{_01E3_,	0x8000},
	{_01E4_,	0x003E},
	{_01E5_,	0x0015},
	{_01E6_,	0x003E},
	{_01E7_,	0x00C8},
	{_01E8_,	0x0043},
	{_01E9_,	0x00A9},
	{_01EA_,	0x00A9},
	{_01EB_,	0x00A9},
	{_01EC_,	0x00FB},
	{_01ED_,	0x00B0},
	{_01EE_,	0x00B9},
	{_01EF_,	0x01BB},
	{_01F0_,	0x02EC},
	{_01F1_,	0x02EC},
	{_01F2_,	0x02EC},
	{_01F3_,	0x01BD},
	{_01F4_,	0x034A},
	{_01F5_,	0x03D8},
	{_01F6_,	0x03FC},
	{_0000_,	0x0113},
	//{_0042_,	0x0100},sensor digital ouotput offset. [11-0] 0~4095 LSB
	//{_0043_,	0x0070},control OB accumulation. [0]0/1=On/Off. [1]setting timming 0/1=V/H. [6-4]7:0=Slow:early
	//{_004A_,	0x0FFE},set clipping value of Max outpot value. [11-0]0x001~0xFFE
	//{_004B_,	0x0004},set clipping value of Min outpot value. [11-0]0x001~0xFFE(Recommend 0x004)
	//{_00A0_,	0x3000},[15]Flip mode 1/0=Off/On
	//{_01A0_,	0x0464},[15-0]Vertical line number in mode1 = (PCLK/(2400*FPS))-1
	//{_01A1_,	0x0000},[0]Vertical line number in mode1 = (PCLK/(2400*FPS))-1
	//{_01A3_,	0x0464},[15-0]Vertical timming setting of pixel drive pulse rising pos. in mode1 = (PCLK/(2400*FPS))-1
	//{_01A4_,	0x0000},[0]Vertical timming setting of pixel drive pulse rising pos. in mode1 = (PCLK/(2400*FPS))-1
	//{_01A7_,	0x0464},[15-0]Vertical timming setting of ADC drive pulse rising pos. in mode1 = (PCLK/(2400*FPS))-1
	//{_01A8_,	0x0000},[0]Vertical timming setting of ADC drive pulse rising pos. in mode1 = (PCLK/(2400*FPS))-1
	//{_02A0_,	0x0464},[15-0]Vertical line number in mode2 = (PCLK/(2400*FPS))-1
	//{_02A1_,	0x0000},[0]Vertical line number in mode2 = (PCLK/(2400*FPS))-1
	//{_02A3_,	0x0464},[15-0]Vertical timming setting of pixel drive pulse rising pos. in mode2 = (PCLK/(2400*FPS))-1
	//{_02A4_,	0x0000},[0]Vertical timming setting of pixel drive pulse rising pos. in mode2 = (PCLK/(2400*FPS))-1
	//{_02A7_,	0x0464},[15-0]Vertical timming setting of ADC drive pulse rising pos. in mode2 = (PCLK/(2400*FPS))-1
	//{_02A8_,	0x0000},[0]Vertical timming setting of ADC drive pulse rising pos. in mode2 = (PCLK/(2400*FPS))-1
};


/*-------------------------------------------------------------------------------------*/
void mn34041pl_reset(void)
{
	unsigned long  dwIndex;
	int iEnSerialIF=1;
	int  chipsel=SPI_BUS_CS1;
	
	// release sensor reset, RSTN
	dw_spi_mn34031pl_write(spike_dev.spi_device,0x1001, 0x3100,chipsel);
	udelay(60);

	for (dwIndex=0; dwIndex<7; dwIndex++)
	{
		if (iEnSerialIF) 
		{
			dw_spi_mn34031pl_write(spike_dev.spi_device, atMN34041PLSerialRegTbl[dwIndex].eRegAddr, atMN34041PLSerialRegTbl[dwIndex].dwData,chipsel);
		}
/*
		else 
		{
			MN34041PL_WriteBuf(ptInfo, atMN34041PLregTbl[dwIndex].eRegAddr, atMN34041PLregTbl[dwIndex].dwData, 2, 0);
		}
*/
	}
	// sleep longer than 150us
	udelay(200);

	for (dwIndex=7; dwIndex<10; dwIndex++)
	{
		if (iEnSerialIF)
		{
			dw_spi_mn34031pl_write(spike_dev.spi_device, atMN34041PLSerialRegTbl[dwIndex].eRegAddr, atMN34041PLSerialRegTbl[dwIndex].dwData,chipsel);
		}
/*
		else 
		{
			MN34041PL_WriteBuf(ptInfo, atMN34041PLregTbl[dwIndex].eRegAddr, atMN34041PLregTbl[dwIndex].dwData, 2, 0);
		}
*/
	}
	// sleep longer than 30ms
	mdelay(40); 
	if (iEnSerialIF)
	{
		for (dwIndex=11; dwIndex<MN34041PL_SERIAL_TBL_SIZE; dwIndex++) 
		{
			dw_spi_mn34031pl_write(spike_dev.spi_device, atMN34041PLSerialRegTbl[dwIndex].eRegAddr, atMN34041PLSerialRegTbl[dwIndex].dwData,chipsel);
		}
		// Clear DCM reset
		dw_spi_mn34031pl_write(spike_dev.spi_device, 0x1001, 0x1100,chipsel);
		// Wait DCM stable
		mdelay(2);
		// Clear SW reset and inverse pixel clock output
/* Version 1.0.0.1 modification, 2010.08.17 */
		dw_spi_mn34031pl_write(spike_dev.spi_device,  0x1001, 0x0980,chipsel);
/* ========================== */
	}


}

#else
#define MN34030PL_TBL_SIZE 263
typedef enum MN34030PL_registers
{
	_0000_                  = 0x0000,
	_0001_                  = 0x0001,
	_0002_                  = 0x0002,
	_0003_                  = 0x0003,
	_0004_                  = 0x0004,
	_0005_                  = 0x0005,
	_0006_                  = 0x0006,
	_0007_                  = 0x0007,
	_0008_                  = 0x0008,
	_0020_                  = 0x0020,
	_0021_                  = 0x0021,
	_0022_                  = 0x0022,
	_0023_                  = 0x0023,
	_0024_                  = 0x0024,
	_0025_                  = 0x0025,
	_0026_                  = 0x0026,
	_0027_                  = 0x0027,
	_0028_                  = 0x0028,
	_0029_                  = 0x0029,
	_0030_                  = 0x0030,
	_0031_                  = 0x0031,
	_0032_                  = 0x0032,
	_0033_                  = 0x0033,
	_0034_                  = 0x0034,
	_0035_                  = 0x0035,
	_0036_                  = 0x0036,
	_0037_                  = 0x0037,
	_003E_                  = 0x003E,
	_003F_                  = 0x003F,
	_0040_                  = 0x0040,
	_0041_                  = 0x0041,
	_0042_                  = 0x0042,
	_0043_                  = 0x0043,
	_0046_                  = 0x0046,
	_004C_                  = 0x004C,
	_004E_                  = 0x004E,
	_004F_                  = 0x004F,
	_0050_                  = 0x0050,
	_0057_                  = 0x0057,
	_005C_                  = 0x005C,
	_0060_                  = 0x0060,
	_0061_                  = 0x0061,
	_0062_                  = 0x0062,
	_0063_                  = 0x0063,
	_0066_                  = 0x0066,
	_006C_                  = 0x006C,
	_006D_                  = 0x006D,	
	_006E_                  = 0x006E,
	_006F_                  = 0x006F,
	_0070_                  = 0x0070,
	_0077_                  = 0x0077,
	_007C_                  = 0x007C,
	_0080_                  = 0x0080,
	_0081_                  = 0x0081,
	_0082_                  = 0x0082,
	_0083_                  = 0x0083,
	_0084_                  = 0x0084,
	_0085_                  = 0x0085,
	_0086_                  = 0x0086,
	_0087_                  = 0x0087,
	_0088_                  = 0x0088,
	_0089_                  = 0x0089,
	_0090_                  = 0x0090,
	_0091_                  = 0x0091,
	_0092_                  = 0x0092,
	_0093_                  = 0x0093,
	_0094_                  = 0x0094,
	_0095_                  = 0x0095,
	_0096_                  = 0x0096,
	_0097_                  = 0x0097,
	_0098_                  = 0x0098,
	_0099_                  = 0x0099,
	_009A_                  = 0x009A,
	_009B_                  = 0x009B,
	_009C_                  = 0x009C,
	_009D_                  = 0x009D,
	_009E_                  = 0x009E,
	_009F_                  = 0x009F,
	_00A0_                  = 0x00A0,
	_00A1_                  = 0x00A1,
	_00A2_                  = 0x00A2,
	_00A3_                  = 0x00A3,
	_00A4_                  = 0x00A4,
	_00A9_                  = 0x00A9,
	_00C0_                  = 0x00C0,
	_00C1_                  = 0x00C1,
	_00C2_                  = 0x00C2,
	_00C3_                  = 0x00C3,
	_00C4_                  = 0x00C4,
	_00C5_                  = 0x00C5,
	_00C6_                  = 0x00C6,
	_00C7_                  = 0x00C7,
	_00C8_                  = 0x00C8,
	_00C9_                  = 0x00C9,
	_00CA_                  = 0x00CA,
	_00CB_                  = 0x00CB,
	_00CC_                  = 0x00CC,
	_00CD_                  = 0x00CD,
	_00CE_                  = 0x00CE,
	_00D0_                  = 0x00D0,
	_00D1_                  = 0x00D1,
	_00D2_                  = 0x00D2,
	_00D3_                  = 0x00D3,
	_0100_                  = 0x0100,
	_0101_                  = 0x0101,
	_0102_                  = 0x0102,
	_0103_                  = 0x0103,
	_0104_                  = 0x0104,
	_0105_                  = 0x0105,
	_0106_                  = 0x0106,
	_0107_                  = 0x0107,
	_0108_                  = 0x0108,
	_0109_                  = 0x0109,
	_010A_                  = 0x010A,
	_010B_                  = 0x010B,
	_010C_                  = 0x010C,
	_010D_                  = 0x010D,
	_010E_                  = 0x010E,
	_010F_                  = 0x010F,
	_0110_                  = 0x0110,
	_0111_                  = 0x0111,
	_0112_                  = 0x0112,
	_0113_                  = 0x0113,
	_0114_                  = 0x0114,
	_0115_                  = 0x0115,
	_0116_                  = 0x0116,
	_0117_                  = 0x0117,
	_0118_                  = 0x0118,
	_0119_                  = 0x0119,
	_011A_                  = 0x011A,
	_011B_                  = 0x011B,
	_011C_                  = 0x011C,
	_011D_                  = 0x011D,
	_011E_                  = 0x011E,
	_011F_                  = 0x011F,
	_0120_                  = 0x0120,
	_0121_                  = 0x0121,
	_0122_                  = 0x0122,
	_0123_                  = 0x0123,
	_0124_                  = 0x0124,
	_0125_                  = 0x0125,
	_0126_                  = 0x0126,
	_0127_                  = 0x0127,
	_0130_                  = 0x0130,
	_0131_                  = 0x0131,
	_0132_                  = 0x0132,
	_0133_                  = 0x0133,
	_0134_                  = 0x0134,
	_0135_                  = 0x0135,
	_0136_                  = 0x0136,
	_0137_                  = 0x0137,
	_0138_                  = 0x0138,
	_0139_                  = 0x0139,
	_013A_                  = 0x013A,
	_0140_                  = 0x0140,
	_0141_                  = 0x0141,
	_0142_                  = 0x0142,
	_0143_                  = 0x0143,
	_0144_                  = 0x0144,
	_0145_                  = 0x0145,
	_0146_                  = 0x0146,
	_0147_                  = 0x0147,
	_0148_                  = 0x0148,
	_0149_                  = 0x0149,
	_014A_                  = 0x014A,
	_014B_                  = 0x014B,
	_014C_                  = 0x014C,
	_014D_                  = 0x014D,
	_014E_                  = 0x014E,
	_014F_                  = 0x014F,
	_0150_                  = 0x0150,
	_0151_                  = 0x0151,
	_0152_                  = 0x0152,
	_0160_                  = 0x0160,
	_0161_                  = 0x0161,
	_0162_                  = 0x0162,
	_0163_                  = 0x0163,
	_0164_                  = 0x0164,
	_0165_                  = 0x0165,
	_0166_                  = 0x0166,
	_0167_                  = 0x0167,
	_0168_                  = 0x0168,
	_0169_                  = 0x0169,
	_016A_                  = 0x016A,
	_016B_                  = 0x016B,
	_0170_                  = 0x0170,
	_0171_                  = 0x0171,
	_0172_                  = 0x0172,
	_0173_                  = 0x0173,
	_0174_                  = 0x0174,
	_0175_                  = 0x0175,
	_0176_                  = 0x0176,
	_0177_                  = 0x0177,
	_0178_                  = 0x0178,
	_0179_                  = 0x0179,
	_017C_                  = 0x017C,
	_017D_                  = 0x017D,
	_017E_                  = 0x017E,
	_017F_                  = 0x017F,
	_0190_                  = 0x0190,
	_0191_                  = 0x0191,
	_0192_                  = 0x0192,
	_0193_                  = 0x0193,
	_0194_                  = 0x0194,
	_0195_                  = 0x0195,
	_0196_                  = 0x0196,
	_0197_                  = 0x0197,
	_0198_                  = 0x0198,
	_0199_                  = 0x0199,
	_019A_                  = 0x019A,
	_019B_                  = 0x019B,
	_01A0_                  = 0x01A0,
	_01A1_                  = 0x01A1,
	_01A2_                  = 0x01A2,
	_01A3_                  = 0x01A3,
	_01A4_                  = 0x01A4,
	_01A5_                  = 0x01A5,
	_01A6_                  = 0x01A6,
	_01A7_                  = 0x01A7,
	_01A8_                  = 0x01A8,
	_01A9_                  = 0x01A9,
	_01AA_                  = 0x01AA,
	_01AB_                  = 0x01AB,
	_01AC_                  = 0x01AC,
	_01AD_                  = 0x01AD,
	_01AE_                  = 0x01AE,
	_01AF_                  = 0x01AF,
	_01B0_                  = 0x01B0,
	_01B1_                  = 0x01B1,
	_01B2_                  = 0x01B2,
	_01B3_                  = 0x01B3,
	_01C0_                  = 0x01C0,
	_01C1_                  = 0x01C1,
	_01C2_                  = 0x01C2,
	_01C3_                  = 0x01C3,
	_01C4_                  = 0x01C4,
	_01C5_                  = 0x01C5,
	_01C6_                  = 0x01C6,
	_01C7_                  = 0x01C7,
	_01C8_                  = 0x01C8,
	_01D0_                  = 0x01D0,
	_01D1_                  = 0x01D1,
	_01D2_                  = 0x01D2,
	_01D3_                  = 0x01D3,
	_01E0_                  = 0x01E0,
	_01E1_                  = 0x01E1,
	_01E2_                  = 0x01E2,
	_01E3_                  = 0x01E3,
	_01E4_                  = 0x01E4,
	_01E5_                  = 0x01E5,
	_01E6_                  = 0x01E6,
	_01E7_                  = 0x01E7,
	_01E8_                  = 0x01E8,
	_01E9_                  = 0x01E9,
	_01EA_                  = 0x01EA,
	_01EB_                  = 0x01EB,
	_01EC_                  = 0x01EC,
	_01ED_                  = 0x01ED,
	_01EE_                  = 0x01EE,
	_01EF_                  = 0x01EF,
	_01F0_                  = 0x01F0,
	_01F1_                  = 0x01F1,
	_01F2_                  = 0x01F2,
	_01F3_                  = 0x01F3,
	_02A0_                  = 0x02A0,
	_02A2_                  = 0x02A2,
	_02A4_                  = 0x02A4,
} EMN34030PLRegs;

typedef struct MN34030PL_reg_addr_data
{
    EMN34030PLRegs eRegAddr;
 unsigned long  dwData;
} TMN34030PLRegAddrData;

/*==================================================================*/
const TMN34030PLRegAddrData atMN34030PLregTbl[MN34030PL_TBL_SIZE] = {
	{_0001_                 , 0x0030},
	{_0002_                 , 0x0002},
	{_0004_                 , 0x0546},
	{_0000_                 , 0x0003},
	{_0003_                 , 0x8220},
	{_0000_                 , 0x0013},
	{_0005_                 , 0x0000},
	{_0020_                 , 0x0080},
	{_0021_                 , 0x0000},
	{_0022_                 , 0x0000},
	{_0023_                 , 0x000F},
	{_0024_                 , 0x0000},
	{_0025_                 , 0x0000},
	{_0026_                 , 0x0000},
	{_0027_                 , 0x0000},
	{_0028_                 , 0x0008},
	{_0029_                 , 0x0000},
	{_0030_                 , 0x5420},
	{_0031_                 , 0x111A},
	{_0032_                 , 0x0460},
	{_0033_                 , 0x0000},
	{_0034_                 , 0x0000},
	{_0035_                 , 0x20E0},
	{_0036_                 , 0x0400},
	{_0037_                 , 0x0000},
	{_003E_                 , 0x014E},
	{_003F_                 , 0x0000},
	{_0040_                 , 0x0000},
	{_0041_                 , 0x0200},
	{_0042_                 , 0x0100},
	{_0043_                 , 0x7057},
	{_0046_                 , 0x0000},
	{_004C_                 , 0x0000},
	{_004E_                 , 0x0FFF},
	{_004F_                 , 0x0000},
	{_0050_                 , 0x0FFF},
	{_0057_                 , 0x0010},
	{_005C_                 , 0x0038},
	{_0060_                 , 0x0000},
	{_0061_                 , 0x0200},
	{_0062_                 , 0x0100},
	{_0063_                 , 0x7057},
	{_0066_                 , 0x0000},
	{_006C_                 , 0x0000},
	{_006D_                , 0x0000},
	{_006E_                 , 0x0FFF},
	{_006F_                 , 0x0000},
	{_0070_                 , 0x0FFF},
	{_0077_                 , 0x0010},
	{_007C_                 , 0x0038},
	{_0080_                 , 0x000C},
	{_0081_                 , 0xFFFF},
	{_0082_                 , 0x0000},
	{_0083_                 , 0x0000},
	{_0084_                 , 0x0000},
	{_0085_                 , 0x0004},
	{_0086_                 , 0x1032},
	{_0087_                 , 0x0000},
	{_0088_                 , 0x0001},
	{_0089_                 , 0x0000},
	{_0090_                 , 0x0012},
	{_0091_                 , 0x0013},
	{_0092_                 , 0x0010},
	{_0093_                 , 0x0011},
	{_0094_                 , 0x0016},
	{_0095_                 , 0x0017},
	{_0096_                 , 0x0014},
	{_0097_                 , 0x0015},
	{_0098_                 , 0x0002},
	{_0099_                 , 0x0003},
	{_009A_                 , 0x0000},
	{_009B_                 , 0x0001},
	{_009C_                 , 0x0006},
	{_009D_                 , 0x0007},
	{_009E_                 , 0x0004},
	{_009F_                 , 0x0005},
	{_00A0_                 , 0x0000},
	{_00A1_                 , 0x0000},
	{_00A2_                 , 0x0000},
	{_00A3_                 , 0x0000},
	{_00A4_                 , 0x0000},
	{_00A9_                 , 0x0000},
	{_00C0_                 , 0x1155},
	{_00C1_                 , 0xC5D5},
	{_00C2_                 , 0x93D5},
	{_00C3_                 , 0x01FC},
	{_00C4_                 , 0xF041},
	{_00C5_                 , 0x7FC4},
	{_00C6_                 , 0x0F54},
	{_00C7_                 , 0x0051},
	{_00C8_                 , 0x1541},
	{_00C9_                 , 0x0200},
	{_00CA_                 , 0x0100},
	{_00CB_                 , 0x0000},
	{_00CC_                 , 0x0000},
	{_00CD_                 , 0x0000},
	{_00CE_                 , 0x0000},
	{_00D1_                 , 0x041B},
	{_00D2_                 , 0x0000},
	{_00D3_                 , 0x041B},
	{_0100_                 , 0x0697},
	{_0101_                 , 0x0697},
	{_0102_                 , 0x0697},
	{_0103_                 , 0x01A5},
	{_0104_                 , 0x0000},
	{_0105_                 , 0x0000},
	{_0106_                 , 0x0000},
	{_0107_                 , 0x0000},
	{_0108_                 , 0x0008},
	{_0109_                 , 0x0009},
	{_010A_                 , 0x0696},
	{_010B_                 , 0x0008},
	{_010C_                 , 0x0000},
	{_010D_                 , 0x0008},
	{_010E_                 , 0x0009},
	{_010F_                 , 0x0696},
	{_0110_                 , 0x0000},
	{_0111_                 , 0x0003},
	{_0112_                 , 0x0009},
	{_0113_                 , 0x0696},
	{_0114_                 , 0x0330},
	{_0115_                 , 0x0333},
	{_0116_                 , 0x033F},
	{_0117_                 , 0x05EF},
	{_0118_                 , 0x0000},
	{_0119_                 , 0x0005},
	{_011A_                 , 0x001F},
	{_011B_                 , 0x0000},
	{_011C_                 , 0x0005},
	{_011D_                 , 0x0013},
	{_011E_                 , 0x001D},
	{_011F_                 , 0x001F},
	{_0120_                 , 0x0000},
	{_0121_                 , 0x0007},
	{_0122_                 , 0x0006},
	{_0123_                 , 0x0000},
	{_0124_                 , 0x033F},
	{_0125_                 , 0x05F9},
	{_0126_                 , 0x0002},
	{_0127_                 , 0x068F},
	{_0130_                 , 0x0000},
	{_0131_                 , 0x0000},
	{_0132_                 , 0x0000},
	{_0133_                 , 0x0000},
	{_0134_                 , 0x0000},
	{_0135_                 , 0x0605},
	{_0136_                 , 0x0341},
	{_0137_                 , 0x0000},
	{_0138_                 , 0x0000},
	{_0139_                 , 0x0000},
	{_013A_                 , 0x0000},
	{_0140_                 , 0x0009},
	{_0141_                 , 0x00D0},
	{_0142_                 , 0x0007},
	{_0143_                 , 0x030D},
	{_0144_                 , 0x0008},
	{_0145_                 , 0x00CF},
	{_0146_                 , 0x0006},
	{_0147_                 , 0x030C},
	{_0148_                 , 0x0000},
	{_0149_                 , 0x0000},
	{_014A_                 , 0x0008},
	{_014B_                 , 0x0006},
	{_014C_                 , 0x0000},
	{_014D_                 , 0x0002},
	{_014E_                 , 0x0002},
	{_014F_                 , 0x0007},
	{_0150_                 , 0x0008},
	{_0151_                 , 0x0006},
	{_0152_                 , 0x0009},
	{_0160_                 , 0x0001},
	{_0161_                 , 0x0004},
	{_0162_                 , 0x01A5},
	{_0163_                 , 0x0001},
	{_0164_                 , 0x003B},
	{_0165_                 , 0x004B},
	{_0166_                 , 0x0053},
	{_0167_                 , 0x0197},
	{_0168_                 , 0x01A4},
	{_0169_                 , 0x01A5},
	{_016A_                 , 0x0000},
	{_016B_                 , 0x0000},
	{_0170_                 , 0x0327},
	{_0171_                 , 0x0335},
	{_0172_                 , 0x0000},
	{_0173_                 , 0x0000},
	{_0174_                 , 0x0000},
	{_0175_                 , 0x0000},
	{_0176_                 , 0x0000},
	{_0177_                 , 0x0000},
	{_0178_                 , 0x0000},
	{_0179_                 , 0x0000},
	{_017C_                 , 0x0000},
	{_017D_                 , 0x0000},
	{_017E_                 , 0x0000},
	{_017F_                 , 0x0000},
	{_0190_                 , 0x0007},
	{_0191_                 , 0x0181},
	{_0192_                 , 0x0047},
	{_0193_                 , 0x0049},
	{_0194_                 , 0x004A},
	{_0195_                 , 0x004B},
	{_0196_                 , 0x004F},
	{_0197_                 , 0x0002},
/* Version 2.0.0.3 modification, 2011.08.11 */
	{_0198_                 , 0x0003},
/* ======================================== */
	{_0199_                 , 0x0002},
	{_019A_                 , 0x004F},
	{_019B_                 , 0x0002},
	{_01A0_                 , 0x0429},
	{_01A1_                 , 0x0000},
	{_01A2_                 , 0x0429},
	{_01A3_                 , 0x041A},
	{_01A4_                 , 0x0429},
	{_01A5_                 , 0x041A},
	{_01A6_                 , 0x0000},
	{_01A7_                 , 0x041B},
	{_01A8_                 , 0x0001},
	{_01A9_                 , 0x0009},
	{_01AA_                 , 0x0008},
	{_01AB_                 , 0x0009},
	{_01AC_                 , 0x0000},
	{_01AD_                 , 0x041C},
	{_01AE_                 , 0x0000},
	{_01AF_                 , 0x0000},
	{_01B0_                 , 0x0000},
	{_01B1_                 , 0x041C},
	{_01B2_                 , 0x0000},
	{_01B3_                 , 0x0000},
	{_01C0_                 , 0x0010},
	{_01C1_                 , 0x0417},
	{_01C2_                 , 0x0010},
	{_01C3_                 , 0x0417},
	{_01C4_                 , 0x0000},
	{_01C5_                 , 0x0000},
	{_01C6_                 , 0x0000},
	{_01C7_                 , 0x0000},
	{_01C8_                 , 0x000F},
	{_01D0_                 , 0x0000},
	{_01D1_                 , 0x0000},
	{_01D2_                 , 0x0000},
	{_01D3_                 , 0x0000},
	{_01E0_                 , 0x000A},
	{_01E1_                 , 0x0023},
	{_01E2_                 , 0x0023},
	{_01E3_                 , 0x0023},
	{_01E4_                 , 0x000E},
	{_01E5_                 , 0x000E},
	{_01E6_                 , 0x000E},
	{_01E7_                 , 0x0028},
	{_01E8_                 , 0x006B},
	{_01E9_                 , 0x006B},
	{_01EA_                 , 0x006B},
	{_01EB_                 , 0x0087},
	{_01EC_                 , 0x00CF},
	{_01ED_                 , 0x00D8},
	{_01EE_                 , 0x0106},
	{_01EF_                 , 0x0316},
	{_01F0_                 , 0x0003},
	{_01F1_                 , 0x001F},
	{_01F2_                 , 0x00B3},
	{_01F3_                 , 0x01BC},
	{_0000_                 , 0x0113},
	{_0003_                 , 0x8260}
};


int mn34031pl_reset()
{
  
  unsigned long  dwIndex;
  
   int  chipsel=SPI_BUS_CS1;


	/* Version 1.0.0.7 modification, 2010.08.30 */
	// release sensor reset, RSTN
	dw_spi_mn34031pl_write(spike_dev.spi_device, 0x1001, 0x3100,chipsel);
	udelay(60);
	/*==========================================*/

	for (dwIndex=0; dwIndex<4; dwIndex++)
	{
	  dw_spi_mn34031pl_write(spike_dev.spi_device,atMN34030PLregTbl[dwIndex].eRegAddr, atMN34030PLregTbl[dwIndex].dwData,chipsel);	
	}
	// sleep longer than 150us
	udelay(200);

	for (dwIndex=4; dwIndex<7; dwIndex++)
	{
	    dw_spi_mn34031pl_write(spike_dev.spi_device,atMN34030PLregTbl[dwIndex].eRegAddr, atMN34030PLregTbl[dwIndex].dwData,chipsel);
	}
	// sleep longer than 30ms
	mdelay(40);
  
	for (dwIndex=7; dwIndex<MN34030PL_TBL_SIZE; dwIndex++)
	{
	  dw_spi_mn34031pl_write(spike_dev.spi_device,atMN34030PLregTbl[dwIndex].eRegAddr, atMN34030PLregTbl[dwIndex].dwData,chipsel);	
	}
	
  
}
#endif

static int spike_prepare_spi_message(void)
{
  
    u8 chipid[3];
    int chipsel=SPI_BUS_CS0;
    int i=0;
 
#if defined(TEST_MN34041PL)
    dw_spi_readID(spike_dev.spi_device,chipid,chipsel);
    
    printk("================================\n");
	printk("Read FLASH ID (chip select:%d) :\n",chipsel);
	for(i=0;i<sizeof(chipid);i++)
	 {
	    
	    printk("[%d]=%02x ",i,chipid[i]);
	    if(i==(sizeof(chipid)-1))
	    {
	      
	      printk("\n");
	    }
	 }
    printk("================================\n");
    {
      u16 data=0x0;
      u16 address=0x0;
      chipsel=SPI_BUS_CS1;

    mn34041pl_reset();
    /*
    /drivers # insmod  ./MN34041PL.ko  iEnSerialIF=1
    MN34041PL_Init() iEnSerialIF=1
    0x00 0x36 0x30 0x21
    dwReadData=3021
    0x00 0x36 0x30 0x21
    [MN34041PL] : Detect device success 1
    /drivers #

      */
      address=0x0036;
    // dw_spi_mn34031pl_write(spike_dev.spi_device,address,0x0020,chipsel);
      dw_spi_mn34031pl_read(spike_dev.spi_device,address,&data,chipsel);
      printk("address: %04x, data: %04x (chip select:%d)\n",address,data,chipsel);
      
      dw_spi_mn34031pl_write(spike_dev.spi_device,address,0x0033,chipsel);
      dw_spi_mn34031pl_read(spike_dev.spi_device,address,&data,chipsel);
      printk("address: %04x, data: %04x (chip select:%d)\n",address,data,chipsel);
    
    }
#else
    dw_spi_readID(spike_dev.spi_device,chipid,chipsel);
    
    printk("================================\n");
	printk("Read FLASH ID (chip select:%d) :\n",chipsel);
	for(i=0;i<sizeof(chipid);i++)
	 {
	    
	    printk("[%d]=%02x ",i,chipid[i]);
	    if(i==(sizeof(chipid)-1))
	    {
	      
	      printk("\n");
	    }
	 }
    printk("================================\n");
 
    
    #if 0

	Test_Flash_RW();
      
    #endif


    {
      u16 data=0x0;
      u16 address=0x0;
      chipsel=SPI_BUS_CS1;

      mn34031pl_reset();
    
      address=0x0197;
      dw_spi_mn34031pl_read(spike_dev.spi_device,address,&data,chipsel);
      printk("address: %04x, data: %04x (chip select:%d)\n",address,data,chipsel);
      
      dw_spi_mn34031pl_write(spike_dev.spi_device,address,0x0760,chipsel);
      dw_spi_mn34031pl_read(spike_dev.spi_device,address,&data,chipsel);
      printk("address: %04x, data: %04x (chip select:%d)\n",address,data,chipsel);
      
      address=0x1C3;
      dw_spi_mn34031pl_read(spike_dev.spi_device,address,&data,chipsel);
      printk("address: %04x, data: %04x (chip select:%d)\n",address,data,chipsel);
    }
#endif
#if 0
    {
      //loopback testing
	struct spi_transfer t[2];
	struct spi_message m;  
  
	spi_message_init(&m);
	memset(t, 0, (sizeof t));
	memset(spike_ctl.tx_buff, 0, SPI_BUFF_SIZE);
	spike_ctl.tx_buff[0] = spike_dev.test_data++;
	spike_ctl.tx_buff[1] = spike_dev.test_data++;
	spike_ctl.tx_buff[2] = spike_dev.test_data++;
	spike_ctl.tx_buff[3] = spike_dev.test_data++;
	spike_ctl.tx_buff[4] = spike_dev.test_data++;
	spike_ctl.tx_buff[5] = spike_dev.test_data++;
	spike_ctl.tx_buff[6] = spike_dev.test_data++;
	spike_ctl.tx_buff[7] = spike_dev.test_data++;
	spike_ctl.tx_buff[8] = spike_dev.test_data++;
	spike_ctl.tx_buff[9] = spike_dev.test_data++;
	spike_ctl.tx_buff[10] = spike_dev.test_data++;
	spike_ctl.tx_buff[11] = spike_dev.test_data++;
	spike_ctl.tx_buff[12] = spike_dev.test_data++;
	spike_ctl.tx_buff[13] = spike_dev.test_data++;
	spike_ctl.tx_buff[14] = spike_dev.test_data++;
	spike_ctl.tx_buff[15] = spike_dev.test_data++;
	
	
	memset(spike_ctl.rx_buff, 0, SPI_BUFF_SIZE);
	t[0].tx_buf = spike_ctl.tx_buff;
	t[0].rx_buf = NULL;
	t[0].len = 16;
	t[0].bits_per_word=16;
	t[0].speed_hz=(25*1000000);
	t[0].cr0=0x78cf;
	t[0].cr1=0;
	t[0].ser=1 << SPI_BUS_CS0;
	t[0].ssienr=0x1;
	
	spi_message_add_tail(&t[0], &m);
	
	
	memset(spike_ctl.rx_buff, 0, SPI_BUFF_SIZE);
	t[1].tx_buf = NULL;
	t[1].rx_buf = spike_ctl.rx_buff;
	t[1].len = 16;
	t[1].bits_per_word=16;
	t[1].speed_hz=(25*1000000);
	t[1].cr0=0x78cf;
	t[1].cr1=0;
	t[1].ser=0x0;
	t[1].ssienr=0x0;
	spi_message_add_tail(&t[1], &m);
	
	spi_sync(spike_dev.spi_device, &m);
    }

#endif

    
    
	return 0;
}

static int spike_do_one_message(void)
{
	int status;

	if (down_interruptible(&spike_dev.spi_sem))
		return -ERESTARTSYS;

	if (!spike_dev.spi_device) {
		up(&spike_dev.spi_sem);
		return -ENODEV;
	}

	status=spike_prepare_spi_message();
	
	up(&spike_dev.spi_sem);

	return status;	
}

static ssize_t spike_read(struct file *filp, char __user *buff, size_t count,
			loff_t *offp)
{
	size_t len;
	ssize_t status = 0;

	if (!buff) 
		return -EFAULT;

	if (*offp > 0) 
		return 0;

	if (down_interruptible(&spike_dev.fop_sem)) 
		return -ERESTARTSYS;
	//printk("%d\n",__LINE__);
	status = spike_do_one_message();
	
	//printk("%d\n",__LINE__);
	if (status) {
		sprintf(spike_dev.user_buff, 
			"spike_do_one_message failed : %d\n",
			status);
	}
	else {
#if 0
		sprintf(spike_dev.user_buff, 
			"Status: %d\nTX: %d %d %d %d %d %d %d %d   %d %d %d %d %d %d %d %d\nRX: %d %d %d %d %d %d %d %d  %d %d %d %d %d %d %d %d\n",
			spike_ctl.msg.status,
			spike_ctl.tx_buff[0], spike_ctl.tx_buff[1], 
			spike_ctl.tx_buff[2], spike_ctl.tx_buff[3],spike_ctl.tx_buff[4],spike_ctl.tx_buff[5], spike_ctl.tx_buff[6],spike_ctl.tx_buff[7],spike_ctl.tx_buff[8],spike_ctl.tx_buff[9], spike_ctl.tx_buff[10],spike_ctl.tx_buff[11],spike_ctl.tx_buff[12],spike_ctl.tx_buff[13], spike_ctl.tx_buff[14],spike_ctl.tx_buff[15],
			spike_ctl.rx_buff[0], spike_ctl.rx_buff[1], 
			spike_ctl.rx_buff[2], spike_ctl.rx_buff[3],spike_ctl.rx_buff[4],spike_ctl.rx_buff[5] ,spike_ctl.rx_buff[6], spike_ctl.rx_buff[7],spike_ctl.rx_buff[8], spike_ctl.rx_buff[9],spike_ctl.rx_buff[10],spike_ctl.rx_buff[11] ,spike_ctl.rx_buff[12], spike_ctl.rx_buff[13],spike_ctl.rx_buff[14], spike_ctl.rx_buff[15]);
#endif 
	
	}
		
	len = strlen(spike_dev.user_buff);
 
	if (len < count) 
		count = len;

	if (copy_to_user(buff, spike_dev.user_buff, count))  {
		printk(KERN_ALERT "spike_read(): copy_to_user() failed\n");
		status = -EFAULT;
	} else {
		*offp += count;
		status = count;
	}

	up(&spike_dev.fop_sem);

	return status;	
}

static int spike_open(struct inode *inode, struct file *filp)
{	
	int status = 0;

	if (down_interruptible(&spike_dev.fop_sem)) 
		return -ERESTARTSYS;

	if (!spike_dev.user_buff) {
		spike_dev.user_buff = kmalloc(USER_BUFF_SIZE, GFP_KERNEL);
		if (!spike_dev.user_buff) 
			status = -ENOMEM;
	}	

	up(&spike_dev.fop_sem);

	return status;
}

static int spike_probe(struct spi_device *spi_device)
{
	if (down_interruptible(&spike_dev.spi_sem))
		return -EBUSY;

	spike_dev.spi_device = spi_device;

	up(&spike_dev.spi_sem);

	return 0;
}

static int spike_remove(struct spi_device *spi_device)
{
	if (down_interruptible(&spike_dev.spi_sem))
		return -EBUSY;
	
	spike_dev.spi_device = NULL;

	up(&spike_dev.spi_sem);

	return 0;
}
struct dw_spi_chip {
	u8 poll_mode;	/* 1 for contoller polling mode */
	u8 type;	/* SPI==00/SSP==01/Micrwire==10 */
	u8 enable_dma;
	u8 test_loopback_mode;  /*1 means use test loop back mode (CTRLR0[11:11]=1)*/
	u8 tmode; /* TR==00/TO==01/RO==01/EEPROM==11*/
	u8 wait_tx_fifo_empty;/*1:will wait, 0:will not wait*/
	u8 control_frame_size;/*0x0~0xf*/
	u8 after_two_transfer_disable_ssic;/*1:need 0:not need*/
	u8 need_set_ctlr1; /*1:need 0:not need*/
	u8 tx_buf_has_rx_len;/*1:has 0:not have*/
	void (*cs_control)(u32 command);
};
static int __init add_spike_device_to_bus(void)
{
	struct spi_master *spi_master;
	struct spi_device *spi_device;
	struct device *pdev;
	char buff[64];
	int status = 0;
	struct dw_spi_chip chip_info;
	memset(&chip_info,0x0,sizeof(struct dw_spi_chip));
	spi_master = spi_busnum_to_master(SPI_BUS);
	if (!spi_master) {
		printk(KERN_ALERT "spi_busnum_to_master(%d) returned NULL\n",
			SPI_BUS);
		printk(KERN_ALERT "Missing dw_spi_platform driver?\n");
		return -1;
	}

	spi_device = spi_alloc_device(spi_master);
	if (!spi_device) {
		put_device(&spi_master->dev);
		printk(KERN_ALERT "spi_alloc_device() failed\n");
		return -1;
	}

	//spi_device->chip_select = SPI_BUS_CS0;
	spi_device->chip_select = SPI_BUS_CS1;
	/* Check whether this SPI bus.cs is already claimed */
	snprintf(buff, sizeof(buff), "%s.%u", 
			dev_name(&spi_device->master->dev),
			spi_device->chip_select);

	pdev = bus_find_device_by_name(spi_device->dev.bus, NULL, buff);
 	if (pdev) {
		/* We are not going to use this spi_device, so free it */ 
		spi_dev_put(spi_device);
		
		/* 
		 * There is already a device configured for this bus.cs  
		 * It is okay if it us, otherwise complain and fail.
		 */
		if (pdev->driver && pdev->driver->name && 
				strcmp(this_driver_name, pdev->driver->name)) {
			printk(KERN_ALERT 
				"Driver [%s] already registered for %s\n",
				pdev->driver->name, buff);
			status = -1;
		} 
	} else {
 
		spi_device->max_speed_hz = SPI_BUS_SPEED;
		spi_device->mode = SPI_MODE_3;
		spi_device->bits_per_word = 8;
		spi_device->irq = -1;
		chip_info.poll_mode=1;
		chip_info.type=0;
		chip_info.enable_dma=0;
	
		spi_device->controller_state = NULL;
		spi_device->controller_data =&chip_info;
		strlcpy(spi_device->modalias, this_driver_name, strlen(this_driver_name));
		
		status = spi_add_device(spi_device);		
		if (status < 0) {	
			spi_dev_put(spi_device);
			printk(KERN_ALERT "spi_add_device() failed: %d\n", 
				status);		
		}				
	}
	spike_dev.spi_device = spi_device;
	put_device(&spi_master->dev);

	return status;
}

static struct spi_driver spike_driver = {
	.driver = {
		.name =	this_driver_name,
		.owner = THIS_MODULE,
	},
	.probe = spike_probe,
	.remove = __devexit_p(spike_remove),	
};

static int __init spike_init_spi(void)
{
	int error;
// FIFO depth of DW SPI  is 16 (entries). FIFO width is 16 bits (2 byte).
	spike_ctl.tx_buff = kmalloc(SPI_BUFF_SIZE, GFP_KERNEL | GFP_DMA);
	if (!spike_ctl.tx_buff) {
		error = -ENOMEM;
		goto spike_init_error;
	}

	spike_ctl.rx_buff = kmalloc(SPI_BUFF_SIZE, GFP_KERNEL | GFP_DMA);
	if (!spike_ctl.rx_buff) {
		error = -ENOMEM;
		goto spike_init_error;
	}

	error = spi_register_driver(&spike_driver);
	if (error < 0) {
		printk(KERN_ALERT "spi_register_driver() failed %d\n", error);
		goto spike_init_error;
	}

	error = add_spike_device_to_bus();
	if (error < 0) {
		printk(KERN_ALERT "add_spike_to_bus() failed\n");
		spi_unregister_driver(&spike_driver);
		goto spike_init_error;	
	}

	return 0;

spike_init_error:

	if (spike_ctl.tx_buff) {
		kfree(spike_ctl.tx_buff);
		spike_ctl.tx_buff = 0;
	}

	if (spike_ctl.rx_buff) {
		kfree(spike_ctl.rx_buff);
		spike_ctl.rx_buff = 0;
	}
	
	return error;
}

static const struct file_operations spike_fops = {
	.owner =	THIS_MODULE,
	.read = 	spike_read,
	.open =		spike_open,	
};

static int __init spike_init_cdev(void)
{
	int error;

	spike_dev.devt = MKDEV(0, 0);

	error = alloc_chrdev_region(&spike_dev.devt, 0, 1, this_driver_name);
	if (error < 0) {
		printk(KERN_ALERT "alloc_chrdev_region() failed: %d \n", 
			error);
		return -1;
	}

	cdev_init(&spike_dev.cdev, &spike_fops);
	spike_dev.cdev.owner = THIS_MODULE;
	
	error = cdev_add(&spike_dev.cdev, spike_dev.devt, 1);
	if (error) {
		printk(KERN_ALERT "cdev_add() failed: %d\n", error);
		unregister_chrdev_region(spike_dev.devt, 1);
		return -1;
	}	

	return 0;
}

static int __init spike_init_class(void)
{
	spike_dev.class = class_create(THIS_MODULE, this_driver_name);

	if (!spike_dev.class) {
		printk(KERN_ALERT "class_create() failed\n");
		return -1;
	}

	if (!device_create(spike_dev.class, NULL, spike_dev.devt, NULL, 	
			this_driver_name)) {
		printk(KERN_ALERT "device_create(..., %s) failed\n",
			this_driver_name);
		class_destroy(spike_dev.class);
		return -1;
	}

	return 0;
}

static int __init spike_init(void)
{
	memset(&spike_dev, 0, sizeof(spike_dev));
	memset(&spike_ctl, 0, sizeof(spike_ctl));

	sema_init(&spike_dev.spi_sem, 1);
	sema_init(&spike_dev.fop_sem, 1);
	
	if (spike_init_cdev() < 0) 
		goto fail_1;
	
	if (spike_init_class() < 0)  
		goto fail_2;

	if (spike_init_spi() < 0) 
		goto fail_3;

	return 0;

fail_3:
	device_destroy(spike_dev.class, spike_dev.devt);
	class_destroy(spike_dev.class);

fail_2:
	cdev_del(&spike_dev.cdev);
	unregister_chrdev_region(spike_dev.devt, 1);

fail_1:
	return -1;
}
module_init(spike_init);

static void __exit spike_exit(void)
{
	spi_unregister_device(spike_dev.spi_device);
	spike_dev.spi_device = NULL;
	spi_unregister_driver(&spike_driver);

	device_destroy(spike_dev.class, spike_dev.devt);
	class_destroy(spike_dev.class);

	cdev_del(&spike_dev.cdev);
	unregister_chrdev_region(spike_dev.devt, 1);

	if (spike_ctl.tx_buff)
		kfree(spike_ctl.tx_buff);

	if (spike_ctl.rx_buff)
		kfree(spike_ctl.rx_buff);

	if (spike_dev.user_buff)
		kfree(spike_dev.user_buff);
}
module_exit(spike_exit);

MODULE_AUTHOR("Jeff Liao");
MODULE_DESCRIPTION("an example SPI driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.2");

