#!/bin/sh
insmod led-class.ko
insmod compat.ko
insmod compat_firmware_class.ko
insmod rfkill_backport.ko
insmod cfg80211.ko
insmod mac80211.ko
insmod ath.ko
insmod ath9k_hw.ko
insmod ath9k_common.ko
insmod ath9k.ko

