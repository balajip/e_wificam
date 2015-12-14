# Copyright (C) 2007-2013  VN Inc.
# morgan.chang@vn.com
#

#########################################################################
TARGETS = watchdog
#########################################################################

srctree := $(CURDIR)
export srctree

CROSS_COMPILE ?= arm-linux-

override EXTRA_CFLAGS +=

obj-y = watchdog.o

PHONY = all
all: $(TARGETS)
$(TARGETS): $(obj-y) FORCE
	$(call if_changed,link_o_target)

#########################################################################
clean: FORCE
	@rm -f $(TARGETS); \
	rm -f *.o .*.cmd .*.d *.map *.ds *.bin;

include rule.mk
