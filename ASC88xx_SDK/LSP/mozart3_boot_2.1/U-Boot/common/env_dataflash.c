/* LowLevel function for DataFlash environment support
 * Author : Gilles Gastaldi (Atmel)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */
#include <common.h>
#include <command.h>
#include <environment.h>
#include <linux/stddef.h>
#include <dataflash.h>

DECLARE_GLOBAL_DATA_PTR;

extern env_t *env_ptr ;

extern char *env_name_spec;

extern int read_dataflash (unsigned long addr, unsigned long size, char
*result);
extern int write_dataflash (unsigned long addr_dest, unsigned long addr_src,
		     unsigned long size);
extern int vpl_SerialFlashInit (void);
extern uchar default_environment[];
/* extern int default_environment_size; */

extern flash_info_t flash_info[];

uchar dataflash_env_get_char_spec (int index)
{
	uchar c;
	//read_dataflash(CONFIG_ENV_ADDR + index + offsetof(env_t,data), 1, (char *)&c);
    printf( "ERROR, we should not call this func\n" ) ;
    
	return (c);
}

void dataflash_env_relocate_spec (void)
{
	//read_dataflash(CONFIG_ENV_ADDR, CONFIG_ENV_SIZE, (char *)env_ptr);
    return ;    
}

extern int flash_sect_erase (ulong addr_first, ulong addr_last) ;
int dataflash_saveenv(void)
{
	/* env must be copied to do not alter env structure in memory*/
	unsigned char temp[CONFIG_ENV_SIZE];
	memcpy(temp, env_ptr, CONFIG_ENV_SIZE);

    //[jam patch] In the original version, it can write data without erasing first. But we cannot!!!
    flash_sect_erase( CONFIG_ENV_ADDR, CONFIG_ENV_ADDR + CONFIG_ENV_SIZE - 1 ) ; 
    
	return write_dataflash(CONFIG_ENV_ADDR, (unsigned long)temp, CONFIG_ENV_SIZE);
}

/************************************************************************
 * Initialize Environment use
 *
 * We are still running from ROM, so data use is limited
 * Use a (moderately small) buffer on the stack
 */
int dataflash_env_init(void)
{
    ulong read_crc, count_crc ;

    env_name_spec = "dataflash..." ;
    
	vpl_SerialFlashInit();	/* prepare for DATAFLASH read/write */

	/* read old CRC */
    //[jam patch] We always read the data at CONFIG_ENV_ADDR to envptr
	read_dataflash(CONFIG_ENV_ADDR, CONFIG_ENV_SIZE, (char *)env_ptr);
        
    read_crc = env_ptr->crc ;
    count_crc = crc32(0, env_ptr->data, sizeof(env_ptr->data)) ;
     
	if (read_crc == count_crc) {
		gd->env_addr = (ulong)(&(env_ptr->data)) ;
		gd->env_valid = 1;
	} else {
		gd->env_addr  = (ulong)&default_environment[0];
		gd->env_valid = 0;
	}

    gd->flags |= GD_FLG_RELOC;
    
	return (0);
}
