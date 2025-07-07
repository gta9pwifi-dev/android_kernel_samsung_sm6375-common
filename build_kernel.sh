#!/usr/bin/env bash
set -e                                     # stop on first error

# absolute path to the Clang r536225 prebuilt you already have
CLANG=~/android/prebuilts/clang/host/linux-x86/clang-r536225
export PATH=$CLANG/bin:$PATH               # makes clang, ld.lld, llvm-objcopy … visible

export ARCH=arm64
export SUBARCH=arm64

# tell Kbuild to use Clang everywhere and to add --target automatically
export CC=clang
export LLVM=1
export LLVM_IAS=1

# use the LLVM wrappers for binutils so no GNU toolchain (and thus no GCC) is needed
export CROSS_COMPILE=llvm-
export CLANG_TRIPLE=aarch64-linux-gnu-

OUT=$PWD/out
mkdir -p $OUT

# one-time defconfig for your Galaxy Tab A9+ Wi-Fi kernel
make O=$OUT vendor/gta9pwifi_defconfig

# fast rebuild: kernel image + device trees; add modules if you need them
make -j$(nproc) O=$OUT Image dtbs
# optional:
# make -j$(nproc) O=$OUT modules
# make O=$OUT modules_install INSTALL_MOD_PATH=$PWD/out/vendor_mods
