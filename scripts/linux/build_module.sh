#!/bin/bash

TOP=`pwd`
SCRIPT=`(cd \`dirname $0\`; pwd)`
ARCH=riscv
if [ -z $CROSS_COMPILE ]; then
	CROSS_COMPILE=riscv64-unknown-linux-gnu-
fi
if [ -f $SCRIPT/config/config-busybox-$ARCH-$MACH ]; then
	BUSYBOX_CONFIG=config-busybox-$ARCH-$MACH
else
	BUSYBOX_CONFIG=config-busybox-$ARCH
fi
if [ -f $SCRIPT/config/config-linux-$ARCH-$MACH ]; then
	LINUX_CONFIG=config-linux-$ARCH-$MACH
else
	LINUX_CONFIG=config-linux-$ARCH
fi
INITRAMFS_FILELIST_TEMPLATE=config-initramfs-$ARCH

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
if [ -z $BBL ]; then
	BBL=riscv-pk
fi
if [ "x$BBL" = "xsdfirm" ]; then
	if [ -z $SDFIRM_DIR ]; then
		SDFIRM_DIR=sdfirm
		SDFIRM_PATH=$TOP/sdfirm
	else
		SDFIRM_PATH=`(cd $SDFIRM_DIR; pwd)`
		SDFIRM_DIR=`dirname $SDFIRM_PATH`
	fi
	if [ -z $MACH ]; then
		MACH=spike64
	fi
fi
INITRAMFS_DIR=obj/initramfs/$ARCH
INITRAMFS_FILELIST=obj/initramfs/list-$ARCH
BBL_DIR=obj/bbl

ARCHIVES_DIR=$TOP/archive

function clean_all()
{
	echo "== Clean all =="
	rm -rf $TOP/obj/busybox-$ARCH
	rm -rf $TOP/$INITRAMFS_DIR
	rm -rf $TOP/obj/linux-$ARCH
	rm -rf $TOP/$BBL_DIR
}

function build_busybox()
{
	echo "== Build Busybox =="
	rm -rf $TOP/obj/busybox-$ARCH
	cd $BUSYBOX_PATH
	mkdir -pv $TOP/obj/busybox-$ARCH
	cp $SCRIPT/config/$BUSYBOX_CONFIG ./.config
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE O=$TOP/obj/busybox-$ARCH/ oldconfig
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE mrproper
	cd $TOP/obj/busybox-$ARCH
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE -j6
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE install
	cd -
}

build_initramfs_busybox()
{
	BUSYBOX_INSTALL=${TOP}/obj/busybox-${ARCH}/_install
	BUSYBOX_FILES=`ls ${BUSYBOX_INSTALL}$1`
	for f in ${BUSYBOX_FILES}; do
		if [ "$f" == "busybox" ]; then
			echo "file $1/busybox ${BUSYBOX_INSTALL}$1/busybox 755 0 0" >> $TOP/$INITRAMFS_FILELIST
			continue
		fi
		if [ -d ${BUSYBOX_INSTALL}$1/${f} ]; then
			echo "dir $1/${f} 755 0 0" >> $TOP/$INITRAMFS_FILELIST
			build_initramfs_busybox $1/$f
		else
			echo "slink $1/$f /bin/busybox 777 0 0" >> $TOP/$INITRAMFS_FILELIST
		fi
	done
}

build_initramfs_rootfs()
{
	ROOTFS_INSTALL=${SCRIPT}/rootfs
	ROOTFS_FILES=`ls ${ROOTFS_INSTALL}$1`
	for f in ${ROOTFS_FILES}; do
		if [ -d ${ROOTFS_INSTALL}$1/${f} ]; then
			echo "dir $1/${f} 755 0 0" >> $TOP/$INITRAMFS_FILELIST
			build_initramfs_rootfs $1/$f
		else
			if [ -x ${ROOTFS_INSTALL}$1/${f} ]; then
				echo "file $1/$f ${ROOTFS_INSTALL}$1/$f 755 0 0" >> $TOP/$INITRAMFS_FILELIST
			else
				echo "file $1/$f ${ROOTFS_INSTALL}$1/$f 644 0 0" >> $TOP/$INITRAMFS_FILELIST
			fi
		fi
	done
}

function build_initramfs()
{
	echo "== Build initramfs =="

	echo "Installing initramfs..."
	rm -rf $TOP/$INITRAMFS_DIR
	mkdir -pv $TOP/$INITRAMFS_DIR
	cp -rf $SCRIPT/config/$INITRAMFS_FILELIST_TEMPLATE \
		$TOP/$INITRAMFS_FILELIST

	echo "Installing busybox..."
	cp -av $TOP/obj/busybox-$ARCH/_install/* $TOP/$INITRAMFS_DIR
	build_initramfs_busybox

	# TODO: Smarter way to build rootfs
	# Currently we only lists files in config-initramfs-${ARCH}
	echo "Installing rootfs..."
	rm -rf $TOP/obj/rootfs
	build_initramfs_rootfs

	echo "Installing benchmarks..."
	if [ -x $TOP/obj/bench ]; then
		echo "dir /usr/local 755 0 0" >> $TOP/$INITRAMFS_FILELIST
		echo "dir /usr/local/bin 755 0 0" >> $TOP/$INITRAMFS_FILELIST
		for f in `ls $TOP/obj/bench`; do
			echo "file /usr/local/bin/$f $TOP/obj/bench/$f 755 0 0" >> $TOP/$INITRAMFS_FILELIST
		done
	fi

	echo "Use INITRAMFS_SOURCE file list: $INITRAMFS_FILELIST"
	grep INITRAMFS_SOURCE $SCRIPT/config/$LINUX_CONFIG
	echo "So initramfs is built not here now but together with kernel later"
	cat $TOP/$INITRAMFS_FILELIST
}

function build_linux()
{
	echo "== Build Linux =="
	rm -rf $TOP/obj/linux-$ARCH
	#mkdir -p $TOP/obj/linux-$ARCH
	cd $LINUX_PATH
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE distclean
	cp $SCRIPT/config/$LINUX_CONFIG arch/$ARCH/configs/my_defconfig
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE O=$TOP/obj/linux-$ARCH/ my_defconfig
	ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE $LINUX_PATH/scripts/config \
		--file $TOP/obj/linux-$ARCH/.config \
		--set-str INITRAMFS_SOURCE $TOP/$INITRAMFS_FILELIST
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE O=$TOP/obj/linux-$ARCH/ -j6
	if [ ! -f $TOP/obj/linux-$ARCH/vmlinux ]
	then
		echo "Error: Failed to build Linux"
		exit 1
	fi
	${CROSS_COMPILE}objcopy --only-keep-debug $TOP/obj/linux-$ARCH/vmlinux $TOP/obj/linux-$ARCH/kernel.sym
	cd -
}

function build_sdfirm()
{
	echo "== Build sdfirm =="
	rm -rf $TOP/obj/sdfirm-$ARCH
	mkdir -p $TOP/obj/sdfirm-$ARCH
	cd $SDFIRM_PATH
	if [ -x $TOP/obj/sdfirm-$ARCH ]; then
		make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE O=$TOP/obj/sdfirm-$ARCH/ distclean
	fi
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE distclean
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE distclean
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE O=$TOP/obj/sdfirm-$ARCH/ ${MACH}_bbl_defconfig
	ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPLE $SDFIRM_PATH/scripts/config \
		--file $TOP/obj/sdfirm-$ARCH/.config \
		--set-str SBI_PAYLOAD_PATH $TOP/obj/linux-$ARCH/arch/$ARCH/boot/Image
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE O=$TOP/obj/sdfirm-$ARCH/ -j6
	if [ ! -f $TOP/obj/sdfirm-$ARCH/sdfirm ]
	then
		echo "Error: Failed to build sdfirm"
		exit 1
	fi
	${CROSS_COMPILE}objcopy --only-keep-debug $TOP/obj/sdfirm-$ARCH/sdfirm $TOP/obj/sdfirm-$ARCH/sdfirm.sym
	cd -
}

function build_riscv-pk()
{
	echo "== Build riscv-pk =="
	rm -rf $TOP/$BBL_DIR
	mkdir -pv $TOP/$BBL_DIR
	cd $BBL_DIR
	$SCRIPT/riscv-pk/configure  --enable-logo --host=riscv64-unknown-linux-gnu --with-payload=$TOP/obj/linux-$ARCH/vmlinux
	make
	cd -
}

function build_bbl()
{
	if [ "x$BBL" = "xriscv-pk" ]; then
		build_riscv-pk
	fi
	if [ "x$BBL" = "xsdfirm" ]; then
		build_sdfirm
	fi
}

cd $TOP

echo "== Prepare =="
if [ ! -f $SCRIPT/config/$LINUX_CONFIG ]
then
	echo "Linux config not found $LINUX_CONFIG"
	exit 1
fi

if [ ! -f $SCRIPT/config/$BUSYBOX_CONFIG ]
then
	echo "Busybox config not found $BUSYBOX_CONFIG"
	exit 1
fi

if [ ! -d $LINUX_DIR ]
then
	echo "Linux source $LINUX_DIR not found"
	exit 1
fi

if [ ! -d $BUSYBOX_DIR ]
then
	echo "Busybox source $BUSYBOX_DIR not found"
	exit 1
fi

if [ $# -eq 1 ]
then
	if [ "$1" == "clean" ]
	then
		clean_all
	elif [ "$1" == "busybox" ]
	then
		build_busybox
	elif [ "$1" == "initramfs" ]
	then
		build_initramfs
	elif [ "$1" == "linux" ]
	then
		build_linux
	elif [ "$1" == "bbl" ]
	then
		build_bbl
	fi
else
		clean_all
		build_busybox
		build_initramfs
		build_linux
		build_bbl
fi
