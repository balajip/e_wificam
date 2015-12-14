# This is a example file of an application
#
# Basically, you only have to modify lib-y, deplib-y, ccflags-y, ldflags-y, and KBUILD_ROOT. 
#
# lib-y is a list about obj files that will be compiled and linked together. 
#
# deplib-y is a list about which libraries it depends on. 
# 	They will be found by "find" later and you do not have to specify it's
# 	path explicitly. 
#
# ccflags-y is C-compiler's option, e.g. -D__DEBUG__
#
# ldflags-y is linker's option, e.g. -lm 
#
# KBUILD_ROOT is the root directory of the whole project. In other words, 
#   KBUILD_ROOT is where you execute "make menuconfig" to configure this project. 
#   It is recommened that assign KBUILD_ROOT by cascading "parent_dir" which is 
#   designed to navigate to the parent directory. 
#
# Ex: if current working direcroty is /home/jsmith/KbuildSample/libs/xmlwrapper
# 	and KBUILD_ROOT = $(call parent_dir, $(call parent_dir, $(PWD)))
# 	then, 
# 	PWD = /home/jsmith/KbuildSample/libs/xmlwrapper
# 	KBUILD_ROOT = /home/jsmith/KbuildSample

obj-y +=  autoscene_tsk.o
dest-y = /usr/sbin/

init-y = init.d-autoscene

misc1-y = autoscene_conf.xml
misc1dest-y = /etc/conf.d/
misc1installopt-y = -m0755

userdef-y += .

deplib-y += msgreader  xmlwrapper expat videocap  resize  memmgr 
deplib-$(CONFIG_AUTOSCENE) += autoscene
ccflags-y += -D_LINUX -D_LINUX_ARM -D__ASM_ARCH_PLATFORM_MOZART_H__ -I./include
ccflags-$(CONFIG_AUTOSCENE) += -D_AUTOSCENE_ 
ccflags-$(CONFIG_PRE_NR) += -D_PRE_NR_ 
ccflags-$(CONFIG_DEBUG) += -D_DEBUG 
ccflags-$(CONFIG_1DRAM_VERSION) += -D_1DRAM_VERSION
ldflags-y += -lpthread -lm -lrt

PWD := $(shell /bin/pwd)
parent_dir = $(patsubst %/, % ,$(dir $(1)))
KBUILD_ROOT := $(call parent_dir, $(call parent_dir, $(PWD)))

ifdef V
  ifeq ("$(origin V)", "command line")
    KBUILD_VERBOSE = $(V)
  endif
endif
ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE = 0
endif
export KBUILD_VERBOSE

all:
	@ $(MAKE) -C $(KBUILD_ROOT) M=$(PWD) 

clean: 
	@ $(MAKE) -C $(KBUILD_ROOT) M=$(PWD) clean

