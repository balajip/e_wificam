#include <linux/list.h>
#include <linux/dma-mapping.h>

#include "mozart_ubi_vmalloc.h"

#ifdef JAMES_TEST_PRINTK
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif

static struct list_head Mozart_vma_list_head ;
static int max_size = 0 ;
static int current_size = 0 ;
static int max_length = 0 ;
static int current_length = 0 ;
static int counter = 0 ;

void *Mozart_vmalloc(const unsigned long size) 
{
	unsigned long vir_addr, handle ;
	struct Mozart_vma_list_node *node = kmalloc( sizeof(struct Mozart_vma_list_node), GFP_KERNEL) ;

	if ( counter == 0 ) {
		INIT_LIST_HEAD(&Mozart_vma_list_head) ;
		counter = 1 ;
	}

	vir_addr = dma_alloc_coherent(NULL, size, &handle, GFP_ATOMIC);            

	node->vir_addr = vir_addr ;
	node->handle = handle ;
	node->size = size ;
	list_add( &(node->list), &Mozart_vma_list_head) ;

	//DBG( "vir_addr = %x is added\n", vir_addr ) ;

	current_size += size ;
	if ( current_size > max_size ) {
		max_size = current_size ;
	}
	current_length++ ;
	if ( current_length > max_length ) {
		max_length = current_length ;
	}

	return vir_addr ;
}

void Mozart_vfree( const unsigned long vir_addr )
{
	struct Mozart_vma_list_node *target ;
	struct list_head *ptr ;
	
	if ( counter == 0 ) {
		INIT_LIST_HEAD(&Mozart_vma_list_head) ;
		counter = 1 ;
		DBG( "[ERR Mozart_vfree] list empty!\n" ) ;
		return ;
	}

	list_for_each( ptr, &Mozart_vma_list_head ) {
		target = list_entry( ptr, struct Mozart_vma_list_node, list ) ;

		if ( target->vir_addr == vir_addr ) {
			//DBG( "find!!!\n" ) ;
			goto correct_exit ;						
		}
	}

	DBG( "[ERR Mozart_vfree] This is the in-correct address!\n" ) ;
	return ;

correct_exit :	
	current_size -= target->size ;
	current_length-- ;

	list_del(ptr) ;
	dma_free_coherent( NULL, target->size, target->vir_addr, &target->handle);
	kfree(target) ;

	if ( list_empty(&Mozart_vma_list_head) ) {
		DBG( "[ERR Mozart_vfree] list empty!\n" ) ;
		printk( "[Mozart-UBI]max_length = %d\n", max_length ) ;
		printk( "            max_size = %d\n", max_size ) ;
		printk( "            current_size = %d\n", current_size ) ;
		printk( "            current_length = %d\n", current_length ) ;
	}
}

static int in_the_range( unsigned long start_addr, unsigned long end_addr, unsigned long target_addr )
{
	if ( (target_addr >= start_addr) && (target_addr <= end_addr)) {
		return 1 ;
	}

	return 0 ;
}

unsigned long Mozart_get_handle( const unsigned long target_vir_addr ) 
{
	struct Mozart_vma_list_node *entry ;
	struct list_head *ptr ;
	
	if ( counter == 0 ) {
		INIT_LIST_HEAD(&Mozart_vma_list_head) ;
		counter = 1 ;
		DBG( "[ERR Mozart_get_handle] list empty!\n" ) ;
		return 0 ;
	}


	if ( list_empty(&Mozart_vma_list_head) ) {
		DBG( "[Err Mozart_get_handle] list empty!\n" ) ;
		return 0 ;
	}

	list_for_each( ptr, &Mozart_vma_list_head ) {
		entry = list_entry( ptr, struct Mozart_vma_list_node, list ) ;

//		DBG("vir_addr = %x, handle = %x\n", entry->vir_addr, entry->handle ) ;

		if ( in_the_range( entry->vir_addr, entry->vir_addr + entry->size, target_vir_addr) ) {
//			DBG( "find!!!\n" ) ;
			goto correct_exit ;						
		}
	}

	DBG( "[ERR Mozart_get_handle] %x is the in-correct address!\n", target_vir_addr ) ;
	return 0 ;

correct_exit :		
	return entry->handle + (target_vir_addr - entry->vir_addr) ;
}

EXPORT_SYMBOL(Mozart_vmalloc) ;    
EXPORT_SYMBOL(Mozart_vfree) ;    
EXPORT_SYMBOL(Mozart_get_handle) ;    

