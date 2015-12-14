#include <asm/mach-types.h>

		.section	".start", "ax"

		b	__beginning

__mach_data:	.long	MACH_TYPE_ROSSINI
		.align

__beginning:
		ldr	r7, __mach_data
