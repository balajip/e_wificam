#ifndef __MOZART_UBI_VMALLOC_H__
#define __MOZART_UBI_VMALLOC_H__

#include <linux/list.h>

struct Mozart_vma_list_node {
    unsigned long vir_addr ;
    unsigned long handle ;
    unsigned long size ;
    struct list_head list ;
} ;

extern void *Mozart_vmalloc(unsigned long size) ;
extern void Mozart_vfree( unsigned long vir_addr ) ;
extern unsigned long Mozart_get_handle( unsigned long vir_addr ) ;

#endif
