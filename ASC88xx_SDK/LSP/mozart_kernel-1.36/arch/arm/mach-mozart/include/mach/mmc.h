#ifndef ASMARM_ARCH_MMC_H
#define ASMARM_ARCH_MMC_H

#include <linux/mmc/host.h>

struct device;

struct mozart_platform_data {
	int (*card_present)(struct device *);
	int (*get_ro)(struct device *);
	int (* init)(struct device *dev);
};

extern void mozart_set_mmc_info(struct mozart_platform_data *info);

#endif
