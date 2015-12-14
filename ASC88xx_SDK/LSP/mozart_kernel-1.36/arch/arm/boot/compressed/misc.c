/*
 * misc.c
 *
 * This is a collection of several routines from gzip-1.0.3
 * adapted for Linux.
 *
 * malloc by Hannu Savolainen 1993 and Matthias Urlichs 1994
 *
 * Modified for ARM Linux by Russell King
 *
 * Nicolas Pitre <nico@visuaide.com>  1999/04/14 :
 *  For this code to run directly from Flash, all constant variables must
 *  be marked with 'const' and all other variables initialized at run-time
 *  only.  This way all non constant variables will end up in the bss segment,
 *  which should point to addresses in RAM and cleared to 0 on start.
 *  This allows for a much quicker boot time.
 */

unsigned int __machine_arch_type;

#include <linux/string.h>

#ifdef STANDALONE_DEBUG
#define putstr printf
#else

static void putstr(const char *ptr);

#include <linux/compiler.h>
#include <mach/uncompress.h>

#ifdef CONFIG_DEBUG_ICEDCC

#ifdef CONFIG_CPU_V6

static void icedcc_putc(int ch)
{
	int status, i = 0x4000000;

	do {
		if (--i < 0)
			return;

		asm volatile ("mrc p14, 0, %0, c0, c1, 0" : "=r" (status));
	} while (status & (1 << 29));

	asm("mcr p14, 0, %0, c0, c5, 0" : : "r" (ch));
}

#else

static void icedcc_putc(int ch)
{
	int status, i = 0x4000000;

	do {
		if (--i < 0)
			return;

		asm volatile ("mrc p14, 0, %0, c0, c0, 0" : "=r" (status));
	} while (status & 2);

	asm("mcr p14, 0, %0, c1, c0, 0" : : "r" (ch));
}

#endif

#define putc(ch)	icedcc_putc(ch)
#define flush()	do { } while (0)
#endif
/*
static void putstr(const char *ptr)
{
	char c;

	while ((c = *ptr++) != '\0') {
		if (c == '\n')
			putc('\r');
		putc(c);
	}

	flush();
}
*/
#endif

#define __ptr_t void *

/*
 * Optimised C version of memzero for the ARM.
 */

void __memzero (__ptr_t s, size_t n)
{
	union { void *vp; unsigned long *ulp; unsigned char *ucp; } u;
	int i;

	u.vp = s;

	for (i = n >> 5; i > 0; i--) {
		*u.ulp++ = 0;
		*u.ulp++ = 0;
		*u.ulp++ = 0;
		*u.ulp++ = 0;
		*u.ulp++ = 0;
		*u.ulp++ = 0;
		*u.ulp++ = 0;
		*u.ulp++ = 0;
	}

	if (n & 1 << 4) {
		*u.ulp++ = 0;
		*u.ulp++ = 0;
		*u.ulp++ = 0;
		*u.ulp++ = 0;
	}

	if (n & 1 << 3) {
		*u.ulp++ = 0;
		*u.ulp++ = 0;
	}

	if (n & 1 << 2)
		*u.ulp++ = 0;

	if (n & 1 << 1) {
		*u.ucp++ = 0;
		*u.ucp++ = 0;
	}

	if (n & 1)
		*u.ucp++ = 0;
}

static inline __ptr_t memcpy(__ptr_t __dest, __const __ptr_t __src,
			    size_t __n)
{
	int i = 0;
	unsigned char *d = (unsigned char *)__dest, *s = (unsigned char *)__src;

	for (i = __n >> 3; i > 0; i--) {
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
	}

	if (__n & 1 << 2) {
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
	}

	if (__n & 1 << 1) {
		*d++ = *s++;
		*d++ = *s++;
	}

	if (__n & 1)
		*d++ = *s++;

	return __dest;
}

/*
 * gzip delarations
 */
#define OF(args)  args
#define STATIC static

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;

#define WSIZE 0x8000		/* Window size must be at least 32k, */
				/* and a power of two */

static uch *inbuf;		/* input buffer */
static uch window[WSIZE] __attribute__((aligned(64)));	/* Sliding window buffer */

static unsigned insize;		/* valid bytes in inbuf */
static unsigned inptr;		/* index of next byte to be processed in inbuf */
static unsigned outcnt;		/* bytes in output buffer */

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define CONTINUATION 0x02 /* bit 1 set: continuation of multi-part gzip file */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define ENCRYPTED    0x20 /* bit 5 set: file is encrypted */
#define RESERVED     0xC0 /* bit 6,7:   reserved */

#define get_byte()  (inptr < insize ? inbuf[inptr++] : fill_inbuf())

/* Diagnostic functions */
#ifdef DEBUG
#  define Assert(cond,msg) {if(!(cond)) error(msg);}
#  define Trace(x) fprintf x
#  define Tracev(x) {if (verbose) fprintf x ;}
#  define Tracevv(x) {if (verbose>1) fprintf x ;}
#  define Tracec(c,x) {if (verbose && (c)) fprintf x ;}
#  define Tracecv(c,x) {if (verbose>1 && (c)) fprintf x ;}
#else
#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#endif

static int  fill_inbuf(void);
static void flush_window(void);
static void error(char *m);
static void gzip_mark(void **);
static void gzip_release(void **);

extern char input_data[];
extern char input_data_end[];

static uch *output_data;
static ulg output_ptr;
static ulg bytes_out;

static void error(char *m);
static void gzip_mark(void **);
static void gzip_release(void **);

static void putstr(const char *);

extern int end;
static ulg free_mem_ptr;
static ulg free_mem_end_ptr;

#define HEAP_SIZE 0x2000

#include "../../../../lib/inflate.c"

#ifndef STANDALONE_DEBUG
#if 0
static void *malloc(int size)
{
	void *p;

	if (size <0) error("Malloc error");
	if (free_mem_ptr <= 0) error("Memory error");

	free_mem_ptr = (free_mem_ptr + 3) & ~3;	/* Align */

	p = (void *)free_mem_ptr;
	free_mem_ptr += size;

	if (free_mem_ptr >= free_mem_end_ptr)
		error("Out of memory");
	return p;
}

static void free(void *where)
{ /* gzip_mark & gzip_release do the free */
}
#endif
static void gzip_mark(void **ptr)
{
	arch_decomp_wdog();
	*ptr = (void *) free_mem_ptr;
}

static void gzip_release(void **ptr)
{
	arch_decomp_wdog();
	free_mem_ptr = (long) *ptr;
}
#else
static void gzip_mark(void **ptr)
{
}

static void gzip_release(void **ptr)
{
}
#endif

/* ===========================================================================
 * Fill the input buffer. This is called only when the buffer is empty
 * and at least one byte is really needed.
 */
int fill_inbuf(void)
{
	if (insize != 0)
		error("ran out of input data");

	inbuf = input_data;
	insize = &input_data_end[0] - &input_data[0];

	inptr = 1;
	return inbuf[0];
}

/* ===========================================================================
 * Write the output window window[0..outcnt-1] and update crc and bytes_out.
 * (Used for the decompressed data only.)
 */
//jon
#define v_inl(addr) (*(volatile unsigned long *)(addr))
#define v_outl(addr, value) (*(volatile unsigned long *)(addr) = value)
void flush_window(void)
{
	ulg c = crc;
	unsigned n;
	uch *in, *out;//, ch, ch1;
#ifdef CONFIG_ARCH_HAYDN
#elif defined(CONFIG_ARCH_BACH)
#else
	uch ch;
#endif

	in = window;
	out = &output_data[output_ptr];
#ifdef CONFIG_ARCH_HAYDN
//jon windows size 32k
	//dma 32k from in to out
/*	APB DMA
	v_outl(0x99500090, in); //source
	v_outl(0x99500094, out); //destination
	v_outl(0x99500098, 0x0); //disable LLP
	v_outl(0x9950009c, 0x400e0501);
	while(v_inl(0x9950009c) & 0x02 == 0x0);
*/

//jon need to flush dcache and write buffer first
	__asm__(
		"fLoop: mrc	p15, 0, r15, c7, c10, 3\n"	\
		"bne	fLoop\n"	\
		"mcr	p15, 0, ip, c7, c10, 4"
	);
//use AHB DMA
	v_outl(0x99900010, in); //source
	v_outl(0x99900014, out); //destination
	v_outl(0x99900018, outcnt); //size
	v_outl(0x99900008, 0x4); //start!
#elif defined(CONFIG_ARCH_BACH)
//jon need to flush dcache and write buffer first
	__asm__(
		"mcr 	p15, 0, ip, c7, c10, 0\n  "	\
		"mcr	p15, 0, ip, c7, c10, 4"
	);
//use AHB DMA
	v_outl(0x90c00010, in); //source
	v_outl(0x90c00014, out); //destination
	v_outl(0x90c00018, outcnt); //size
	v_outl(0x90c00008, 0x4); //start!
#endif
	//while(v_inl(0x9990000c) & 0x01 == 0x0);

	for (n = 0; n < outcnt; n++) {
#ifdef CONFIG_ARCH_HAYDN
		c = crc_32_tab[((int)c ^ (*in++)) & 0xff] ^ (c >> 8);
#elif defined(CONFIG_ARCH_BACH)
		c = crc_32_tab[((int)c ^ (*in++)) & 0xff] ^ (c >> 8);
#else
		ch = *out++ = *in++;
		c = crc_32_tab[((int)c ^ ch) & 0xff] ^ (c >> 8);
#endif
		/*check
		ch = *in++;
		ch1 = *out++;
		if(ch != ch1)
			putstr("x");
		*/
	}
	crc = c;
	bytes_out += (ulg)outcnt;
	output_ptr += (ulg)outcnt;
	outcnt = 0;
#ifdef CONFIG_ARCH_HAYDN
	//jon polling dma
//	while(v_inl(0x9950009c) & 0x02 == 0x0);
	while((v_inl(0x9990000c) & 0x01) == 0x0);
#elif defined(CONFIG_ARCH_BACH)
	while((v_inl(0x90c0000c) & 0x01) == 0x0);
#endif

	putstr(".");
}

#ifndef arch_error
#define arch_error(x)
#endif

static void error(char *x)
{
	arch_error(x);

	putstr("\n\n");
	putstr(x);
	putstr("\n\n -- System halted");

	while(1);	/* Halt */
}

#if defined CONFIG_ARCH_BACH
#include <asm/arch/platform.h>
void uart_init(int i)
{
    unsigned volatile char *uart_reg;
    if(i==0)
        uart_reg=(unsigned char *)BACH_UARTC0_MMR_BASE;
    else if(i==1)
        uart_reg=(unsigned char *)BACH_UARTC1_MMR_BASE;
    else if(i==2)
        uart_reg=(unsigned char *)BACH_UARTC2_MMR_BASE;
    else if(i==3)
        uart_reg=(unsigned char *)BACH_UARTC3_MMR_BASE;
    else
        return;

    *(uart_reg+SERIAL_LCR) = 0x83;
    *(uart_reg+SERIAL_DLL) = DEFAULT_HOST_BAUD;
    *(uart_reg+SERIAL_LCR) = 0x3;
    *(uart_reg+SERIAL_FCR) = 0xc1;
    return;
}
#endif

#if defined CONFIG_ARCH_VVTK1000
#include <asm/arch/platform.h>
void uart_init(int i)
{
    unsigned volatile char *uart_reg;
    if(i==0)
        uart_reg=(unsigned char *)VVTK1000_UART1_BASE;
    else if(i==1)
        uart_reg=(unsigned char *)VVTK1000_UART2_BASE;
    else
        return;

    *(uart_reg+SERIAL_LCR) = 0x83;
    *(uart_reg+SERIAL_DLL) = DEFAULT_HOST_BAUD;
    *(uart_reg+SERIAL_LCR) = 0x3;
    *(uart_reg+SERIAL_FCR) = 0xc1;
    return;
}
#endif

#if defined CONFIG_ARCH_HAYDN
#include <asm/arch/platform.h>
void uart_init(int i)
{
    unsigned volatile char *uart_reg;
    if(i==0)
        uart_reg=(unsigned char *)HAYDN_UARTC0_MMR_BASE;
    else if(i==1)
        uart_reg=(unsigned char *)HAYDN_UARTC1_MMR_BASE;
    else
        return;

    *(uart_reg+SERIAL_LCR) = 0x83;
    *(uart_reg+SERIAL_DLL) = DEFAULT_HOST_BAUD;
    *(uart_reg+SERIAL_LCR) = 0x3;
    *(uart_reg+SERIAL_FCR) = 0xc1;
    return;
}
#endif

#if defined CONFIG_ARCH_MOZART || defined CONFIG_ARCH_ROSSINI
#include <mach/platform.h>
void uart_init(int i)
{
    unsigned volatile char *uart_reg;
    if(i==0)
        uart_reg=(unsigned char *)VPL_UARTC_0_MMR_BASE;
    else if(i==1)
        uart_reg=(unsigned char *)VPL_UARTC_1_MMR_BASE;
    else if(i==2)
        uart_reg=(unsigned char *)VPL_UARTC_2_MMR_BASE;
    else if(i==3)
        uart_reg=(unsigned char *)VPL_UARTC_3_MMR_BASE;
    else
        return;

    *(uart_reg+SERIAL_LCR) = 0x83;
    *(uart_reg+SERIAL_DLL) = DEFAULT_HOST_BAUD;
    *(uart_reg+SERIAL_LCR) = 0x3;
    *(uart_reg+SERIAL_FCR) = 0xc1;

	/* UART clock source */
	uart_reg = (unsigned char *)VPL_SYSC_MMR_BASE;
	if (*(uart_reg + 0x14) == 0x03000000) // Mozart v3
	{
#define I2SSC_PLL_REF_SEL_SHIFT   (25)
#define I2SSC_PLL_REF_SEL(x)      ((x) << I2SSC_PLL_REF_SEL_SHIFT)
		int val = *(uart_reg + 0x48);
		if (CONFIG_UART_CLK == 18432000)
			*(uart_reg + 0x48) = val & ~I2SSC_PLL_REF_SEL(1);
		else if (CONFIG_UART_CLK == 24000000)
			*(uart_reg + 0x48) = val | I2SSC_PLL_REF_SEL(1);
	}

    return;
}
#endif

#ifndef STANDALONE_DEBUG

ulg
decompress_kernel(ulg output_start, ulg free_mem_ptr_p, ulg free_mem_end_ptr_p,
		  int arch_id)
{
	output_data		= (uch *)output_start;	/* Points to kernel start */
	free_mem_ptr		= free_mem_ptr_p;
	free_mem_end_ptr	= free_mem_end_ptr_p;
	__machine_arch_type	= arch_id;

	arch_decomp_setup();
	/*
	int i = 0;
	char * backup = 0x2000000;
    for(i = input_data; i <= input_data_end; i++);
		*backup++ = *((char*)i);
	*/
	//jon
	//uart_init(1);//james_test
	//uart_init(2);
#if defined(CONFIG_UART_PORT_NUM_0) || defined(CONFIG_ROSSINI_UART_PORT_NUM_0)
    uart_init(0);
#elif defined(CONFIG_UART_PORT_NUM_1) || defined(CONFIG_ROSSINI_UART_PORT_NUM_1)
    uart_init(1);
#elif defined(CONFIG_UART_PORT_NUM_2) || defined(CONFIG_ROSSINI_UART_PORT_NUM_2)
    uart_init(2);
#elif defined(CONFIG_UART_PORT_NUM_3) || defined(CONFIG_ROSSINI_UART_PORT_NUM_3)
    uart_init(3);
#else
#error "You have to select correct UART-PORT-NUMBER(0~3)!!"
#endif

//	while(1){
	/*
	backup = 0x2000000;
	for(i = input_data; i <= input_data_end; i++);
		*((char*)i) = *backup++;
	*/
#if 0
	output_data		= (uch *)output_start;	/* Points to kernel start */
	free_mem_ptr		= free_mem_ptr_p;
	free_mem_end_ptr	= free_mem_end_ptr_p;
	__machine_arch_type	= arch_id;

	insize = 0;
	inptr = 0;
	outcnt = 0;		/* bytes in output buffer */
	output_ptr = 0;
	bytes_out = 0;
#endif
	makecrc();
	putstr("Uncompressing Linux...");
	gunzip();
	putstr(" done, booting the kernel.\n");
//	}
	return output_ptr;
}
#else

char output_buffer[1500*1024];

int main()
{
	output_data = output_buffer;

	makecrc();
	putstr("Uncompressing Linux...");
	gunzip();
	putstr("done.\n");
	return 0;
}
#endif
