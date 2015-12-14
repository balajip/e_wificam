/*
 * Copyright (C) 2012  VN Inc.
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

#ifndef    __MOZART_AGPO_H
#define    __MOZART_AGPO_H
#define MOZART_N_AGPO 8
#include <mach/agpo_export.h>
/*
 * basic agpo routines
 *
 * the agpo clock will be turned on when boot
 *
 * AGPOs are numbered 0..(MOZART_N_AGPO-1). For documentation, and maybe
 * for later updates, code should write AGPO(N):
 *
 */
#define    AGPO(X)        (X)        /* 0 <= X <= 70 */

#define AGPO_STATUS_IDLE	0x00000001
#define AGPO_STATUS_SETUP	0x00000002
#define AGPO_STATUS_ENABLE	0x00000004

typedef struct
{
    unsigned long agpo_status[MOZART_N_AGPO];
	struct semaphore mutex;
} AGPO_Dev;

struct agpo_controller {
	u32    version;			// version
    u32    ctrl;			// agpo send pattern when 1
    u32    status;
	u32    intr_mask;
    u32    data_oe_n;		// (0) output mode, (1) disable output mode
    u32    def_data_val;
    u32    ctrl_port_sel;	// (0) normal mode, (1) I2C mode, def high
	u32    pattern_0;
	u32    pattern_1;
	u32    pattern_2;
	u32    pattern_3;
	u32    period;
	u32    data_length;
	u32    interval_repeat;
};

/* The __agpo_to_controller() and __agpo_mask() functions inline to constants
 * with constant parameters; or in outlined code they execute at runtime.
 *
 * You'd access the controller directly when reading or writing more than
 * one gpio value at a time, and to support wired logic where the value
 * being driven by the cpu need not match the value read back.
 *
 * These are NOT part of the cross-platform AGPO interface
 */
static inline struct agpo_controller *__iomem
__agpo_to_controller(unsigned agpo)
{
    void *__iomem ptr;

    if (agpo >= MOZART_N_AGPO)
        return NULL;

    ptr = (void *__iomem)IO_ADDRESS(VPL_AGPOC_MMR_BASE);

    return ptr;
}

static inline u32 __agpo_mask(unsigned agpo)
{
    return 1 << (agpo % 32);
}

/* The get/set/clear functions will inline when called with constant
 * parameters, for low-overhead bitbanging. Illegal constant parameters
 * cause link-time errors.
 *
 * Otherwise, calls with variable parameters use outlined functions.
 */

extern int agpo_request(unsigned agpo, const char *tag);
extern void agpo_free(unsigned agpo);
extern int agpo_set_direction(unsigned agpo, int value);
extern int agpo_setup(unsigned agpo, AGPO_Info * AGPO_info);
extern int agpo_read_status(unsigned agpo);
extern int agpo_intr_mask(unsigned agpo, int value);
extern int agpo_set_default_val(unsigned agpo, int value);
extern void agpo_set_mode(unsigned agpo, AGPO_Info * AGPO_info);
extern int agpo_multi_enable(unsigned agpo, unsigned pins, int enable);
extern int agpo_enable(unsigned agpo, int value);
#endif
/* __MOZART_AGPO_H */
