#!/bin/bash

if [ -z $TOP ]; then
	echo "TOP environment must be specified!"
	echo "As this script is sourced by the invoker, you must specify"
	echo "the abstract path of TOP where we can find all program"
	echo "paths."
	exit 1
fi

if [ -z $BUILD_TINY ]; then
	BUILD_TINY=no
fi
if [ -z $BUILD_LIB ]; then
	BUILD_LIB=yes
fi
if [ -z $BUILD_SMP ]; then
	BUILD_SMP=yes
fi
if [ -z $BUILD_UEFI ]; then
	BUILD_UEFI=no
fi
if [ -z $BUILD_NET ]; then
	BUILD_NET=yes
fi
if [ -z $BUILD_STO ]; then
	BUILD_STO=yes
fi
if [ -z $BUILD_KVM ]; then
	BUILD_KVM=no
fi
if [ "x${BUILD_STO_DEV}" != "x" ]; then
	echo "Storage device is ${BUILD_STO_DEV}"
fi
export BUILD_TINY
export BUILD_LIB
export BUILD_SMP
export BUILD_UEFI
export BUILD_NET
export BUILD_STO
export BUILD_KVM
export BUILD_STO_DEV

if [ -z $LIBFDT_DIR ]; then
	LIBFDT_DIR=dtc
	LIBFDT_PATH=$TOP/dtc
else
	LIBFDT_PATH=`(cd $LIBFDT_DIR; pwd)`
	LIBFDT_DIR=`dirname $LIBFDT_PATH`
fi
if [ -z $KVMTOOL_DIR ]; then
	KVMTOOL_DIR=kvmtool
	KVMTOOL_PATH=$TOP/kvmtool
else
	KVMTOOL_PATH=`(cd $KVMTOOL_DIR; pwd)`
	KVMTOOL_DIR=`dirname $KVMTOOL_PATH`
fi
if [ -z $BUSYBOX_DIR ]; then
	BUSYBOX_DIR=busybox
	BUSYBOX_PATH=$TOP/busybox
else
	BUSYBOX_PATH=`(cd $BUSYBOX_DIR; pwd)`
	BUSYBOX_DIR=`dirname $BUSYBOX_PATH`
fi
if [ -z $LINUX_DIR ]; then
	LINUX_DIR=linux
	LINUX_PATH=$TOP/linux
else
	LINUX_PATH=`(cd $LINUX_DIR; pwd)`
	LINUX_DIR=`dirname $LINUX_PATH`
fi
if [ -z $UBOOT_DIR ]; then
	UBOOT_DIR=u-boot
	UBOOT_PATH=$TOP/u-boot
else
	UBOOT_PATH=`(cd $UBOOT_DIR; pwd)`
	UBOOT_DIR=`dirname $UBOOT_PATH`
fi
ARCHIVES_DIR=$TOP/archive
DESTDIR=${TOP}/obj/bench
APPDIR=${DESTDIR}/usr/local/bin
export LIBFDT_DIR
export LIBFDT_PATH
export KVMTOOL_DIR
export KVMTOOL_PATH
export BUSYBOX_DIR
export BUSYBOX_PATH
export LINUX_DIR
export LINUX_PATH
export UBOOT_DIR
export UBOOT_PATH
export ARCHIVES_DIR
export DESTDIR
export APPDIR

INITRAMFS_DIR=obj/initramfs/$ARCH
INITRAMFS_FILELIST=obj/initramfs/list-$ARCH
STORAGE_DIR=obj/storage/$ARCH
BBL_DIR=obj/bbl
export INITRAMFS_DIR
export INITRAMFS_FILELIST
export STORAGE_DIR
export BBL_DIR

# Build progs
if [ ! -z ${BBLVAR} ]; then
	SDFIRM_CONFIG=${MACH}_bbl${BBLVAR}_defconfig
else
	SDFIRM_CONFIG=${MACH}_bbl_defconfig
fi
echo "Using sdfirm configuration ${SDFIRM_CONFIG}..."
if [ -f $SCRIPT/config/config-busybox-$ARCH-$MACH ]; then
	BUSYBOX_CONFIG=config-busybox-$ARCH-$MACH
else
	BUSYBOX_CONFIG=config-busybox-$ARCH
fi
echo "Using busybox configuration ${BUSYBOX_CONFIG}..."
if [ -f $SCRIPT/config/config-linux-$ARCH-$MACH ]; then
	LINUX_CONFIG=config-linux-$ARCH-$MACH
else
	LINUX_CONFIG=config-linux-$ARCH
fi
echo "Using linux configuration ${LINUX_CONFIG}..."
INITRAMFS_FILELIST_TEMPLATE=config-initramfs-$ARCH
echo "Using rootfs configuration ${INITRAMFS_FILELIST_TEMPLATE}..."

export SDFIRM_CONFIG
export BUSYBOX_CONFIG
export LINUX_CONFIG
export INITRAMFS_FILELIST_TEMPLATE

. ${SCRIPT}/sdfirm_progs.sh
