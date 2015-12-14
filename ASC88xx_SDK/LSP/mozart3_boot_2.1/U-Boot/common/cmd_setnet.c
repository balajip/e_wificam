#include <common.h>
#include <command.h>
#include <asm/config.h>
#include <asm/arch/platform.h>

DECLARE_GLOBAL_DATA_PTR;

char set_ethaddr_array[50] ;
char set_ipaddr_array[50] ;
int ethaddr_template[] = { 0x02, 0x00, 0x02, 0x01, 0x02, 0x00} ;
int ipaddr_template[] ={ 172, 17, 207, 0} ;
int do_setnet (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{    
	int evm_number = -1 ;
			
    if (argc != 2) {
        goto setnet_usage ;
    }   

    evm_number = simple_strtoul(argv[1],NULL,10) ;
	
	printf("\n - Evm_number = %d\n", evm_number) ;

	sprintf (set_ethaddr_array, "%02x:%02x:%02x:%02x:%02x:%02x",
		 ethaddr_template[0], ethaddr_template[1], ethaddr_template[2],
		 ethaddr_template[3], ethaddr_template[4], evm_number
	);
	setenv("ethaddr", set_ethaddr_array) ;
	printf(" - Your Ethaddr will be %s\n", set_ethaddr_array) ;

	sprintf (set_ipaddr_array, "%d.%d.%d.%d",
		 ipaddr_template[0], ipaddr_template[1], 
		 ipaddr_template[2], evm_number
	);
	setenv("ipaddr", set_ipaddr_array) ;
	printf(" - Your Ipaddr will be %s\n", set_ipaddr_array) ;

	printf(" - Saveenv......\n") ;
	(*saveenv)() ;

    return 0;
    
setnet_usage :
    printf ("Usage:\n%s\n", cmdtp->usage);
    return 1;
}

U_BOOT_CMD(
	setnet, 2,	0,	do_setnet,
	"Set net environment variables for EVM.",
	"setnet [evm#] - Set net environment variables for EVM No.X .\n"
);

