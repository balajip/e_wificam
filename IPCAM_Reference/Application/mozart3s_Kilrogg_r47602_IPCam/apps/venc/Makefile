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

obj-$(CONFIG_VOC) += app/venc_queue.o app/venc_app.o app/venc_app_conf.o app/venc_app_init.o app/venc.o app/venc_display.o app/venc_display_conf.o app/venc_display_init.o app/venc_display_process.o app/venc_encoder.o app/venc_encoder_conf.o app/venc_encoder_dispatch.o app/venc_encoder_init.o app/venc_encoder_process.o app/venc_master.o app/venc_master_conf.o app/venc_master_init.o app/venc_master_process.o app/venc_message.o app/venc_motion.o app/venc_motion_conf.o app/venc_motion_init.o app/venc_motion_process.o app/vencslave_txtoverlay.o app/venc_videoin.o app/venc_videoin_conf.o app/venc_videoin_init.o app/venc_videoin_process.o app/videocomm.o app/venc_encoder_h264.o app/venc_encoder_jpeg.o app/venc_encoder_mp4v.o app/venc_encoder_svc.o

obj-y += app/venc_queue.o app/venc_app.o app/venc_app_conf.o app/venc_app_init.o app/venc.o  app/venc_encoder.o app/venc_encoder_conf.o app/venc_encoder_dispatch.o app/venc_encoder_init.o app/venc_encoder_process.o app/venc_master.o app/venc_master_conf.o app/venc_master_init.o app/venc_master_process.o app/venc_message.o app/venc_motion.o app/venc_motion_conf.o app/venc_motion_init.o app/venc_motion_process.o app/vencslave_txtoverlay.o app/venc_videoin.o app/venc_videoin_privacymask.o app/venc_videoin_conf.o app/venc_videoin_init.o app/venc_videoin_handler.o app/venc_videoin_process.o app/videocomm.o app/venc_encoder_h264.o app/venc_encoder_jpeg.o app/venc_encoder_mp4v.o app/venc_encoder_svc.o

dest-y = /usr/sbin/
userdef-y += .

subdir-y += app/fonts/ app/config_files/

deplib-y +=  xmlwrapper expat sharedbuffmgr memmgr msgreader videocap  massmemaccess  pbengine    freetype  frameratectrl eptz

deplib-$(CONFIG_IRE) += resize
deplib-$(CONFIG_AUTOSCENE) += autoscene
deplib-$(CONFIG_IBP) += imgbackendprocess

deplib-$(CONFIG_VOC) += videodisp
deplib-$(CONFIG_IBP) += png12
deplib-$(CONFIG_IBP) += z
deplib-$(CONFIG_IBP) += bitmaskresizer
deplib-$(CONFIG_MPV4)  += mp4venc 
deplib-$(CONFIG_JPEG)  += jpegenc 
deplib-$(CONFIG_H264)  += h264enc
deplib-$(CONFIG_SVC)  += svcenc
deplib-$(CONFIG_DPC) += defectpixelcorrect


ccflags-y += -D_LINUX -D_LINUX_ARM -D__ASM_ARCH_PLATFORM_MOZART_H__ -I./include
ccflags-$(CONFIG_DPC) += -D_DEFECT_PIXEL_ENABLE_
ccflags-$(CONFIG_VOC) += -D_VPL_VOC
ccflags-$(CONFIG_IBP) += -D_VMA_IBPE
ccflags-$(CONFIG_DIE) += -D_VMA_DIE
ccflags-$(CONFIG_1DRAM_VERSION) += -D_1DRAM_VERSION_
ccflags-$(CONFIG_IRE) += -D_VMA_IRE
ccflags-$(CONFIG_AF) += -D_AF
ccflags-$(CONFIG_JPEG) += -D_VMA_JEBE
ccflags-$(CONFIG_MPV4) += -D_VMA_MEBE
ccflags-$(CONFIG_H264) += -D_H4EE
ccflags-$(CONFIG_SVC) += -D_SVC
ccflags-$(CONFIG_YUV422) += -D_YUV422_


ccflags-$(CONFIG_ENC_ROI) += -D_ENC_ROI_
ccflags-$(CONFIG_MD_TRIGER_ROI) += -D_MD_TRIGER_ROI_
ccflags-$(CONFIG_PRE_NR) += -D_PRE_NR_
ccflags-$(CONFIG_NVP1114A) += -D_NVP1114A
ccflags-$(CONFIG_MT9P031) += -D_MT9P031
ccflags-$(CONFIG_IMX035) += -D_IMX035
ccflags-$(CONFIG_IMX104) += -D_IMX104
ccflags-$(CONFIG_IMX104) += -D_IMX138
ccflags-$(CONFIG_IMX222) += -D_IMX222
ccflags-$(CONFIG_IMX238) += -D_IMX238
ccflags-$(CONFIG_OV5653) += -D_OV5653
ccflags-$(CONFIG_OV5658) += -D_OV5658
ccflags-$(CONFIG_R2560x1920) += -D_R2560x1920
ccflags-$(CONFIG_5M2MSWITCH) += -D_5M2MSWITCH
ccflags-$(CONFIG_3M2MSWITCH) += -D_3M2MSWITCH
ccflags-$(CONFIG_TW2866) += -D_TW2866
ccflags-$(CONFIG_AR0331) += -D_AR0331
ccflags-$(CONFIG_R2048x1536) += -D_R2048x1536
ccflags-$(CONFIG_AR0130) += -D_AR0130
ccflags-$(CONFIG_AR0330) += -D_AR0330
ccflags-$(CONFIG_IT6604) += -D_IT6604
ccflags-$(CONFIG_WDR) += -D_WDR
ccflags-$(CONFIG_AUTOIRIS) += -D_AUTOIRIS
ccflags-$(CONFIG_AUTOSCENE) += -D_AUTOSCENE_
ccflags-$(CONFIG_DEBUG) += -D_DEBUG 
ccflags-y += -D_VMA_IBPE_BACK_DOOR
ccflags-$(CONFIG_JPEG_SNAPSHOT) += -D_JPEG_SNAPSHOT_
ccflags-$(CONFIG_JPEG_SNAPSHOT) += -D_DUMP_JPEG_FILE

ldflags-y += -lpthread -lm  -lROISelector -lrt

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

