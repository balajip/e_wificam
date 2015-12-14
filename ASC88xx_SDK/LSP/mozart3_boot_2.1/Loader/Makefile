#
# top Makefile
#
# Copyright 2011 VN Inc.  All rights reserved.
#
#

srctree := $(CURDIR)
export srctree
_c_flags += $(EXTRA_CFLAGS)

PHONY :=

ifdef BOOT
  _c_flags += -DWHERE_IS_BOOTLOADER=BOOT_$(BOOT)
endif


CONF=config.h
CONFMK=$(CONF:%.h=.%.mk)
-include $(CONFMK)

LOADERS = sf sd nf2k

ifeq ($(filter-out $(LOADERS:%=loader-%) nf2k_preloader/nf2k_preloader,$(MAKECMDGOALS)),)
TARGETS = $(MAKECMDGOALS)
endif
TARGETS-BIN = $(LOADERS:%=loader-%.bin)

obj-y = loader.o ddrsdmc.o pub.o #ddlt.o bist.o
obj-y += $(shell echo $(BOOT) | tr A-Z a-z).o
obj-$(CONFIG_PCIE_EP) += ep.o
obj-$(CONFIG_PUB_SW_TRAINING) += pub_sw_training.o
lds := loader.lds


$(CONF): FORCE
	@[ -n "$(config)" -o -f "$@" ] || (echo 'Usage: make $(MAKECMDGOALS) config=<config file>' && exit 1)
	@[ -z "$(config)" -o -f "$(config)" ] || (echo '$(config) not existed' && exit 2)
	@   (  [ -n "$(config)" ] \
	    && echo "#ifndef _CONFIG_H_" > $(@) \
	    && echo "#define _CONFIG_H_" >> $(@) \
	    && sed -n -e 's/#/\/\//' -e 's/\(\w\+\)\s*=\s*\([^\/ \t]*\)/#define \1\t(\2)/' -e 's/(y)/(1)/' -e 's/\(("\|")\)//g' -e 'p' $(config) >> $@) \
	    && echo '#endif /* _CONFIG_H_ */' >> $(@) \
	    && echo '-include $(config)' > $(CONFMK) \
	 || [ 1 ]
	@([ -z "$(MAKECMDGOALS)" ] && $(MAKE) all) || [ 1 ]

PHONY += all
all: $(LOADERS)

PHONY += $(LOADERS) 
$(LOADERS):: $(CONF) FORCE
	@$(MAKE) BOOT=`echo $(@) | tr a-z A-Z` loader-$(@)
nf2k:: nf2k_preloader/nf2k_preloader.lds FORCE
	@$(MAKE) nf2k_preloader/nf2k_preloader obj-y=nf2k_preloader/nf2k_preloader.o lds=nf2k_preloader/nf2k_preloader.lds

$(TARGETS): $(obj-y) $(lds) FORCE
	$(call if_changed,link_o_target)

clean: FORCE
	@rm -f $(LOADERS:%=loader-%); \
	rm -f $(TARGETS-BIN); \
	rm -f $(lds) nf2k_preloader/nf2k_preloader.lds; \
	rm -f *.o .*.cmd .*.d *.map *.ds; \
	cd nf2k_preloader && rm -f nf2k_preloader *.bin *.o .*.cmd .*.d *.map *.ds
#	rm -f $(LOADERS:%=loader-%.map); \
#	rm -f $(LOADERS:%=loader-%.ds); \
#	rm -f $(foreach dep,$(obj-y) $(lds),.$(dep).d); \
#	rm -f $(foreach cmd,$(obj-y) $(lds),.$(cmd).cmd); \

distclean: clean FORCE
	@rm -f $(CONF) $(CONFMK);

help: FORCE
	@echo "Usage: make [sd|sf|nf2k] [config=<config file>]"

include rule.mk
