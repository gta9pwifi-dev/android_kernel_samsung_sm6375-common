#
# Makefile for ADI battery_secrete On
#
# SPDX-License-Identifier: GPL-2.0-only
KDIR	:= /lib/modules/$(shell uname -r)/build
subdir-ccflags-y += -Wall -Werror
subdir-ccflags-y += -I$(srctree)/drivers/power/supply/battery_secrete/ds28e30lib

obj-$(CONFIG_ONEWIRE_GPIO) += battery_auth_class.o
# obj-$(CONFIG_BATT_VERIFY_BY_ST) += stick_verify.o
obj-$(CONFIG_BATT_VERIFY) += auth_battery.o
# obj-$(CONFIG_BATT_VERIFY_BY_SLG) += slg_verify.o

obj-$(CONFIG_BATT_VERIFY_BY_DS28E30) += ds28e30_verify.o
ds28e30_verify-y += \
	ds28e30.o	\
	ds28e30lib/bignum.o	\
	ds28e30lib/deep_cover_coproc.o	\
	ds28e30lib/ecc_generate_key.o	\
	ds28e30lib/ecdsa_generic_api.o	\
	ds28e30lib/ecdsa_high.o		\
	ds28e30lib/sha256_hmac.o	\
	ds28e30lib/sha256_software.o	\
	ds28e30lib/sha256_stone.o	\
	ds28e30lib/ucl_rng.o	\
	ds28e30lib/ucl_sha256.o	\
	ds28e30lib/ucl_sha3.o	\
	ds28e30lib/ucl_sia256.o	\
	ds28e30lib/ucl_sys.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
