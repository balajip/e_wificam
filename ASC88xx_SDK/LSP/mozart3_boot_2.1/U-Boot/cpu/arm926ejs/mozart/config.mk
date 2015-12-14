PLATFORM_CPPFLAGS += $(call cc-option,-mtune=arm926ejs,)
LDSCRIPT := $(SRCTREE)/cpu/arm926ejs/mozart/u-boot.lds
