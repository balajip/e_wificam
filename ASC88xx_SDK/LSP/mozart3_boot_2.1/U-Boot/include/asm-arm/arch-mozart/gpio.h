#ifndef __MOZART_GPIO_H__
#define __MOZART_GPIO_H__

#include <asm/io.h>
#include <asm/errno.h>
#include <asm/arch/hardware.h>

#define GPIOC_DATA_OUT            (0x04)
#define GPIOC_DATA_IN             (0x08)
#define GPIOC_PIN_DIR             (0x0C)
#define GPIOC_DATA_SET            (0x14)
#define GPIOC_DATA_CLEAR          (0x18)
#define GPIOC_PIN_PULL_ENABLE     (0x1C)
#define GPIOC_PIN_PULL_TYPE       (0x20)
#define GPIOC_INTR_ENABLE         (0x24)
#define GPIOC_INTR_MASK           (0x30)
#define GPIOC_INTR_CLEAR          (0x34)
#define GPIOC_INTR_TRIGGER_TYPE   (0x38)
#define GPIOC_INTR_BOTH           (0x3C)
#define GPIOC_INTR_DIR            (0x40)

#endif
