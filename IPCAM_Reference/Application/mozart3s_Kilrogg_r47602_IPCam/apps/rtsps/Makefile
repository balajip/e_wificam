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


obj-y += app/FileSrc.o app/rtsp_server.o app/RtspSrvrApp.o app/RtspSrvrApp_config.o app/ShrdBufSrc.o app/SockSrc.o app/UnixSockUtil.o

dest-y = /usr/sbin/
init-y = init.d-rtsps
userdef-y += .
#misc1-$(CONFIG_MT9P031) += app/IPCam/stream_server_config.xml
#misc1-$(CONFIG_R2560x1920) += app/IPCam/2560x1920/stream_server_config.xml
#misc1-$(CONFIG_R1920x1080) += app/IPCam/stream_server_config.xml
#misc1-$(CONFIG_NULLSensor) += app/IPCam/stream_server_config.xml
#misc1-$(CONFIG_IMX035) += app/IPCam/stream_server_config.xml
#misc1-$(CONFIG_OV2710) += app/IPCam/stream_server_config.xml
#misc1-$(CONFIG_MN34030PL) += app/IPCam/stream_server_config.xml
#misc1-$(CONFIG_MN34030PL_60FPS) += app/IPCam/stream_server_config.xml
#misc1-$(CONFIG_CAT6011) += app/IPCam/stream_server_config.xml
#misc1-$(CONFIG_OV9710) +=  app/IPCam/stream_server_config.xml
#misc1-$(CONFIG_NVP1114A) += app/VideoServer/stream_server_config.xml
#misc1-$(CONFIG_TW2866) += app/VideoServer/stream_server_config.xml

#misc1dest-y = /etc/conf.d/stream_server_config.xml
#misc1installopt-y = -m0755

ifeq ("$(CONFIG_APPS_LIVE555_RTSPS)", "y")
deplib-y += expat xmlmgr sharedbuffmgr memmgr rtspsrvr_live555
else
deplib-y += expat xmlmgr sharedbuffmgr memmgr rtspsrvr
endif
deplib-$(CONFIG_APPS_NEW_IPC) += membroker syncringbuffer
ccflags-$(CONFIG_ENABLE_DIFFERENT_DESTINATION_IP) += -D__ENABLE_DIFFERENT_DESTINATION_IP__
ccflags-y += -D_QOS_ -D_LINUX -D_LINUX_ARM -D_ENABLE_SHAREDBUFF_
ccflags-$(CONFIG_APPS_NEW_IPC) += -DUSE_NEW_IPC
ccflags-$(CONFIG_DEBUG) += -D_DEBUG
ldflags-y += -lpthread -lcrypt
ldflags-$(CONFIG_APPS_LIVE555_RTSPS) += -lstdc++
ldflags-$(CONFIG_APPS_NEW_IPC) += -lrt

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


