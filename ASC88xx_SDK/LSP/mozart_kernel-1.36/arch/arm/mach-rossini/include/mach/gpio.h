/*
 * Copyright (C) 2013  VN Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef    __ROSSINI_GPIO_H
#define    __ROSSINI_GPIO_H
#define ROSSINI_N_GPIO 20
#include <mach/gpio_export.h>
/*
 * basic gpio routines
 *
 * the gpio clock will be turned on when boot
 *
 * GPIOs are numbered 0..(ROSSINI_N_GPIO-1). For documentation, and maybe
 * for later updates, code should write GPIO(N)
 *
 */
#define    GPIO(X)        (X)        /* 0 <= X <= 70 */
struct gpio_controller {
	u32    version;
	u32    data_out;
	u32    data_in;
	u32    pin_dir;
	u32    pin_bypass;
	u32    data_set;
	u32    data_clr;
	u32    pin_pull_enable;
	u32    pin_pull_type;
	u32    intr_enable;
	u32    intr_raw_state;
	u32    intr_mask_state;
	u32    intr_mask;
	u32    intr_clear;
	u32    intr_trigger_type;
	u32    intr_both;
	u32    intr_dir;
	u32    debounce_enable;
	u32    debounce_period;
};

/* The __gpio_to_controller() and __gpio_mask() functions inline to constants
 * with constant parameters; or in outlined code they execute at runtime.
 *
 * You'd access the controller directly when reading or writing more than
 * one gpio value at a time, and to support wired logic where the value
 * being driven by the cpu need not match the value read back.
 *
 * These are NOT part of the cross-platform GPIO interface
 */
static inline struct gpio_controller *__iomem
__gpio_to_controller(unsigned gpio)
{
    void *__iomem ptr;

    if (gpio >= ROSSINI_N_GPIO)
        return NULL;

    ptr = (void *__iomem)IO_ADDRESS(VPL_GPIOC_MMR_BASE);

    return ptr;
}

static inline u32 __gpio_mask(unsigned gpio)
{
    return 1 << (gpio % 32);
}

/* The get/set/clear functions will inline when called with constant
 * parameters, for low-overhead bitbanging. Illegal constant parameters
 * cause link-time errors.
 *
 * Otherwise, calls with variable parameters use outlined functions.
 */
//extern int __error_inval_gpio(void);

extern void __gpio_set(unsigned gpio, int value);
extern int __gpio_get(unsigned gpio);

/* Returns zero or nonzero; works for gpios configured as inputs OR
 * as outputs.
 *
 * NOTE: changes in reported values are synchronized to the GPIO clock.
 * This is most easily seen after calling gpio_set_value() and then immediatly
 * gpio_get_value(), where the gpio_get_value() would return the old value
 * until the GPIO clock ticks and the new value gets latched.
 */


/* powerup default direction is IN */
#define BUILD_IN 1
#if BUILD_IN
extern int gpio_get_value(unsigned gpio);
extern void gpio_set_value(unsigned gpio, int value);
extern int gpio_direction_input(unsigned gpio);
extern int gpio_direction_output(unsigned gpio, int value);
extern int gpio_set_direction(unsigned gpio, int value);
extern int gpio_request(unsigned gpio, const char *tag);
extern void gpio_free(unsigned gpio);
#else
int (*gpio_get_value)(unsigned gpio);
void (*gpio_set_value)(unsigned gpio, int value);
int (*gpio_direction_input)(unsigned gpio);
int (*gpio_direction_output)(unsigned gpio, int value);
int (*gpio_set_direction)(unsigned gpio, int value);
int (*gpio_request)(unsigned gpio, const char *tag);
void (*gpio_free)(unsigned gpio);
#endif
#include <asm-generic/gpio.h>    /* cansleep wrappers */
static inline int gpio_to_irq(unsigned gpio)
{
    return gpio;
}

static inline int irq_to_gpio(unsigned irq)
{
    return irq;
}

#endif
/* __ROSSINI_GPIO_H */
