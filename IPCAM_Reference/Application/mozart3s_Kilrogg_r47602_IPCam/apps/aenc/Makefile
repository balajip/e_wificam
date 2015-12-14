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

obj-y += app/aenc.o  app/aenc_codec.o app/aenc_conf.o  app/aenc_msg.o app/aenc_task.o app/aenc_xmltree.o  app/audioin.o
obj-$(CONFIG_AAC4) +=  app/aenc_aac4.o
obj-$(CONFIG_G711) +=  app/aenc_g711.o
obj-$(CONFIG_GAMR) +=  app/aenc_gamr.o app/amr_rtp2storage.o
obj-$(CONFIG_G726) +=  app/aenc_g726.o


dest-y = /usr/sbin/
init-y = init.d-aenc
userdef-y += .
#misc1-y = app/aenc_conf.xml
#misc1dest-y = /etc/conf.d/
#misc1installopt-y = -m0755

deplib-y += xmlwrapper expat memmgr pbengine rtpamrpack msgreader asound massmemaccess
deplib-$(CONFIG_G711) += g711enc
deplib-$(CONFIG_G726) += g726enc
deplib-$(CONFIG_GAMR) += gamrenc gamrcomm gamrtool
deplib-$(CONFIG_AAC4) += aac4enc 
ccflags-y += -D_LINUX -D_LINUX_ARM 
ccflags-$(CONFIG_G711) += -D__G711_Enc__ 
ccflags-$(CONFIG_G726) += -D__G726_Enc__
ccflags-$(CONFIG_GAMR) += -D__GAMR_Enc__
ccflags-$(CONFIG_AAC4) += -D__AAC4_Enc__
ccflags-$(CONFIG_DEBUG) += -D__DEBUG__
ccflags-$(CONFIG_SSM2603) += -D__SSM2603__
ccflags-$(CONFIG_WAU8822) += -D__WAU8822__
ccflags-$(CONFIG_WM8978) += -D__WM8978__
ccflags-$(CONFIG_AIC3104) += -D__AIC3104__
ccflags-$(CONFIG_ALC5623) += -D__ALC5623__
ccflags-$(CONFIG_TW2866_AUDIO) += -D__TW2866_AUDIO__
ccflags-$(CONFIG_NVP1114A_AUDIO) += -D__NVP1114A_AUDIO__
ccflags-$(CONFIG_CAT6011_HDMI_AUDIO) += -D_CAT6011_HDMI_AUDIO_
ccflags-$(CONFIG_IT6604_HDMI_AUDIO) += -D_IT6604_HDMI_AUDIO_
#ccflags-y += -D__DUMP_PCM__
ccflags-y += -D_ASYNC_IO_
ccflags-y += -D_RESTART_AENC_
#ccflags-y += -D_DEBUG_TIMESTAMP_
ccflags-y += -D_CHECK_RTSPS_STATUS_
ldflags-y += -lpthread

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


