# SPDX-License-Identifier: GPL-2.0
ifeq ($(CONFIG_SEC_GTA9P_PROJECT),y)
dts-dirs += gta9/gta9p
else ifeq ($(CONFIG_SEC_GTA9PWIFI_PROJECT),y)
dts-dirs += gta9/gta9pwifi
endif

ifeq ($(CONFIG_SEC_A23XQ_PROJECT),y)
dts-dirs += a23/a23xq
else ifeq ($(CONFIG_SEC_A23XQ_PROJECT),y)
dts-dirs += a23/a23xq
endif

always          := $(dtb-y)
subdir-y        := $(dts-dirs)

clean-files := *.dtb *.reverse.dts *.dtbo
