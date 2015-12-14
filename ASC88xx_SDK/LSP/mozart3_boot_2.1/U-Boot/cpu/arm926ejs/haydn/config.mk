PLATFORM_CPPFLAGS += $(call cc-option,-mtune=arm926ejs,)
LDSCRIPT := $(SRCTREE)/cpu/arm926ejs/haydn/u-boot.lds
