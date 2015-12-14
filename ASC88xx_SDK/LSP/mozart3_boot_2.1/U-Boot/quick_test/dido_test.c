#include <common.h>
#include <command.h>
#include <config.h>
#include <asm/arch/platform.h>


#define DI_PIN PLATFORM_DI_PIN
#define DO_PIN PLATFORM_DO_PIN

#define INPUT_DIR  0
#define OUTPUT_DIR 1

#define TESTPATTERN 0x5a6900ff

void gpio_set_dir(int pinnum, int dir)
{
	unsigned long dir_reg = v_inl(VPL_GPIOC_MMR_BASE + GPIOC_PIN_DIR);

	dir_reg &= ~(1 << pinnum);
	dir_reg |= dir << pinnum;

	v_outl(VPL_GPIOC_MMR_BASE + GPIOC_PIN_DIR, dir_reg);
}

int dido_test_func(void)
{
	unsigned long testpattern = TESTPATTERN ;
	unsigned long input_value, output_value ;
	int di_pin = DI_PIN, do_pin = DO_PIN;
	char *s;


	if ((s=getenv("di_pin")) != NULL)
		di_pin = simple_strtol(s, NULL, 10);
	if ((s=getenv("do_pin")) != NULL)
		do_pin = simple_strtol(s, NULL, 10);

	gpio_set_dir(di_pin, INPUT_DIR) ;
	gpio_set_dir(do_pin, OUTPUT_DIR);

	printf("dido_test_func!!!\n"
	       "Connect GPIO%d and GPIO%d\n", di_pin, do_pin);

	while (testpattern != 0)
	{
		//clear
		v_outl(VPL_GPIOC_MMR_BASE + GPIOC_DATA_CLEAR, 1 << do_pin) ;

		//write value
		output_value = (testpattern & 0x1) << do_pin ;
		v_outl(VPL_GPIOC_MMR_BASE + GPIOC_DATA_SET, output_value);

		udelay(500);

		//read value
		input_value = v_inl(VPL_GPIOC_MMR_BASE + GPIOC_DATA_IN) ;
		input_value &= (1 << di_pin) ;
		input_value >>= di_pin ;

		if(input_value != (testpattern & 0x1)) {
			return -1 ;
		}

		testpattern >>= 1 ;
	}

	return 0;
}
