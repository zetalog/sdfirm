#!/bin/bash

TOP=`pwd`
SCRIPT=`(cd \`dirname $0\`; pwd)`
ARCH=riscv
HOSTNAME=sdfirm
BUILD_TINY=no
BUILD_LIB=yes
BUILD_SMP=yes
BUILD_UEFI=no
BUILD_NET=yes
BUILD_STO=yes
BUILD_KVM=no
INSTALL_INITRAMFS=yes
INSTALL_LINK=yes
FORCE_REBUILD=no

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
INITRAMFS_DIR=obj/initramfs/$ARCH
INITRAMFS_FILELIST=obj/initramfs/list-$ARCH
STORAGE_DIR=obj/storage/$ARCH
BBL_DIR=obj/bbl
ARCHIVES_DIR=$TOP/archive
DESTDIR=${TOP}/obj/bench
MODSDIR=${TOP}/obj/linux-modules
APPDIR=${DESTDIR}/usr/local/bin
mkdir -p ${APPDIR}

function apply_modcfg()
{
	if [ "x$1" = "xbusybox" ]; then
		dcfg=configs/$2
	else
		dcfg=arch/$ARCH/configs/$2
	fi
	mcfg=$SCRIPT/modcfg/$1/$3

	if [ -f $mcfg ]; then
		echo "Applying $1 modcfg $3..."
		$SDFIRM_PATH/scripts/modconfig.sh $2 $mcfg
		make oldconfig
		cp -f ./.config $dcfg
	fi
}

function clean_all()
{
	echo "== Clean all =="
	rm -rf $TOP/obj/busybox-$ARCH
	rm -rf $TOP/$INITRAMFS_DIR
	rm -rf $TOP/obj/linux-$ARCH
	rm -rf $TOP/obj/vlinux-$ARCH
	rm -rf $TOP/$BBL_DIR
}

function build_libfdt()
{
	echo "== Build libfdt =="
	(
	cd $LIBFDT_PATH
	export CC="${CROSS_COMPILE}gcc"
	TRIPLET=$($CC -dumpmachine)
	_SYSROOT=$($CC -print-sysroot)
	SYSROOT=`cd ${_SYSROOT}; pwd`
	echo "== TRIPLET: ${TRIPLET} =="
	echo "== SYSROOT: ${SYSROOT} =="
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE libfdt
	#make NO_PYTHON=1 NO_YAML=1 DESTDIR=${SYSROOT} PREFIX=/usr LIBDIR=/usr/lib/lp64d install-lib install-includes
	make NO_PYTHON=1 NO_YAML=1 DESTDIR=${DESTDIR} PREFIX=/usr install-lib install-includes
	mkdir -p ${SYSROOT}/usr/include
	mkdir -p ${SYSROOT}/usr/lib
	mv -f ${DESTDIR}/usr/include/* ${SYSROOT}/usr/include/
	mv -f ${DESTDIR}/usr/lib/* ${SYSROOT}/usr/lib/
	)
}

function build_kvmtool()
{
	echo "== Build kvmtool =="
	(
	cd $KVMTOOL_PATH
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE clean
	if [ "x${BUILD_LIB}" = "xyes" ]; then
		make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE
		make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE DESTDIR=$DESTDIR \
			prefix=/usr/local install
	else
		make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE lkvm-static
		cp ${KVMTOOL_PATH}/lkvm-static ${APPDIR}/lkvm
	fi
	)
}

function build_busybox()
{
	echo "== Build Busybox =="
	if [ "xyes" = "x${FORCE_REBUILD}" ]; then
		rm -rf $TOP/obj/busybox-$ARCH
	fi
	mkdir -pv $TOP/obj/busybox-$ARCH
	(
	cd $BUSYBOX_PATH
	if [ "xyes" = "x${FORCE_REBUILD}" ]; then
		echo "Rebuilding $BUSYBOX_CONFIG..."
		# Doing modcfgs in the original directory and save my_defconfig
		cp $SCRIPT/config/$BUSYBOX_CONFIG configs/my_defconfig
		if [ "xyes" = "x${BUILD_KVM}" ]; then
			BUILD_NET=yes
		fi
		if [ "xyes" = "x${BUILD_TINY}" ]; then
			apply_modcfg busybox my_defconfig e_tiny.cfg
		fi
		if [ "xno" = "x${BUILD_NET}" ]; then
			apply_modcfg busybox my_defconfig d_net.cfg
		fi
		# Starting the build process
		make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
			O=$TOP/obj/busybox-$ARCH/ my_defconfig
		make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE mrproper
	fi
	cd $TOP/obj/busybox-$ARCH
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE -j`nproc`
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE install

	if [ "xyes" = "x${BUILD_NET}" ]; then
		mkdir -p $SCRIPT/rootfs/usr/share/udhcpc/
		cp $BUSYBOX_PATH/examples/udhcp/simple.script \
		$SCRIPT/rootfs/usr/share/udhcpc/default.script
	fi
	)
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

install_initramfs_dir()
{
	if [ "xno" != "${INSTALL_INITRAMFS}" ]; then
		echo "dir ${ROOTFS_TARGET} 755 0 0" >> $TOP/$INITRAMFS_FILELIST
	fi
	if [ "xno" != "x${BUILD_STO}" ]; then
		echo "Creating directory $TOP/$STORAGE_DIR ${ROOTFS_TARGET}..."
		sudo mkdir -p $TOP/$STORAGE_DIR/${ROOTFS_TARGET}
	fi
}

install_initramfs_slink()
{
	if [ "xno" != "${INSTALL_INITRAMFS}" ]; then
                echo "slink ${ROOTFS_TARGET} $(readlink "$ROOTFS_HOST") 777 0 0" >> \
                        $TOP/$INITRAMFS_FILELIST
	fi
	if [ "xno" != "x${BUILD_STO}" ]; then
		ROOTFS_LINK=`readlink ${ROOTFS_HOST}`
		echo "Creating link $TOP/$STORAGE_DIR ${ROOTFS_TARGET} -> ${ROOTFS_LINK}..."
		sudo ln -s ${ROOTFS_LINK} $TOP/$STORAGE_DIR/${ROOTFS_TARGET}
	fi
}

install_initramfs_file()
{
	if [ "xno" != "${INSTALL_INITRAMFS}" ]; then
		echo "file ${ROOTFS_TARGET} ${ROOTFS_HOST} $1 0 0" >> \
			$TOP/$INITRAMFS_FILELIST
	fi
	if [ "xno" != "x${BUILD_STO}" ]; then
		echo "Creating file $TOP/$STORAGE_DIR ${ROOTFS_TARGET}..."
		sudo cp -f ${ROOTFS_HOST} $TOP/$STORAGE_DIR/${ROOTFS_TARGET}
	fi
}

install_initramfs_lib()
{
	# TODO: Reduce Image Size
	#
	# Use ldd to reduce installed dependent libraries.
	echo "lib $1 $2"
}

install_initramfs_one()
{
	ROOTFS_HOST=$1$2
	ROOTFS_TARGET=$2
	if [ -d ${ROOTFS_HOST} ]; then
		install_initramfs_dir
		install_initramfs $1 ${ROOTFS_TARGET}
	else
		if [ -L ${ROOTFS_HOST} ]; then
			install_initramfs_slink
		else
			if [ -x ${ROOTFS_HOST} ]; then
				#ROOTFS_LIBS=`ldd ${ROOTFS_HOST} | \
				#	sort | uniq | \
				#	awk -F\= '{print $2}' | \
				#	awk '{print $2}' | uniq`
				#for ROOTFS_LIB in ${ROOTFS_LIBS} ; do
				#	install_initramfs_lib \
				#		$1 ${ROOTFS_LIB}
				#done
				file ${ROOTFS_HOST} | grep shell >/dev/null
				install_initramfs_file 755
			else
				install_initramfs_file 644
			fi
		fi
	fi
}

install_initramfs()
{
	if [ -d $1$2 ]; then
		ROOTFS_FILES=`ls $1$2`
		for f in ${ROOTFS_FILES}; do
			if [ -h $1$2/${f} ]; then
				if [ "x${INSTALL_LINK}" = "xno" ]; then
					echo "Skipping soft link $2/${f}..."
				else
					echo "Installing $2/${f}..."
					install_initramfs_one $1 $2/${f}
				fi
			else
				echo "Installing $2/${f}..."
				install_initramfs_one $1 $2/${f}
			fi
		done
	else
		install_initramfs_one $1 $2
	fi
}

install_initramfs_sysroot()
{
	if [ -d $1$2 ]; then
		mkdir -p $TOP/obj/rootfs/lib/
		cp $1$2/lib/l*.so* $TOP/obj/rootfs/lib/ -rf
		cd $TOP/obj/rootfs/lib/
		ROOTFS_FILES=`ls l*.so*`
		cd -
		for f in ${ROOTFS_FILES}; do
			install_initramfs_one $TOP/obj/rootfs "/lib/${f}"
		done
		${CROSS_COMPILE}strip $TOP/obj/rootfs/lib/*
	fi
}

get_sysroot()
{
	GCC=`which ${CROSS_COMPILE}gcc`
	GCCDIR=`dirname $GCC`
	TOOLSDIR=`cd ${GCCDIR}/..; pwd`

	echo ${TOOLSDIR}/sysroot
}

function make_ramdisk()
{
	local image=$1
	local base=`basename $1`
	local ksize=$2

	echo "Creating image $base of ${ksize}KB"
	if dd if=/dev/zero of=$image bs=1024 count=$ksize 1>/dev/null 2>&1
	then
		if mkfs -t ext4 $image 1>/dev/null 2>&1; then
			echo "Creating image $base success"
			return 0
		fi
	fi
	echo "Creating image $base failure"
	return 1
}

function build_initramfs()
{
	echo "== Build initramfs =="

	echo "Installing initramfs..."
	rm -rf $TOP/$INITRAMFS_DIR
	mkdir -pv $TOP/$INITRAMFS_DIR

	# Prepare initramfs
	cp -rf $SCRIPT/config/$INITRAMFS_FILELIST_TEMPLATE \
		$TOP/$INITRAMFS_FILELIST

	# Prepare block rootfs
	if [ "xno" != "x${BUILD_STO}" ]; then
		if [ -z $1 ]; then
			img_size=4194304
			#img_size=16384
		else
			img_size=$1
		fi
		img_file=$TOP/storage.img
		rm -f ${img_file}
		echo "************************************************************"
		echo "Do not proceed as the root priviledge will be required."
		echo "Please confirm the directory used for rootfs is correct."
		echo "************************************************************"
		echo -n "Creating rootfs in $TOP/$STORAGE_DIR (y/n)? "
		read input
		if [ "x$input" = "xy" ]; then
			sudo rm -rf $TOP/$STORAGE_DIR
			make_ramdisk ${img_file} ${img_size}
			mkdir -p $TOP/$STORAGE_DIR
			sudo mount ${img_file} $TOP/$STORAGE_DIR -o loop
			sudo mkdir -p $TOP/$STORAGE_DIR/sys
			sudo mkdir -p $TOP/$STORAGE_DIR/proc
			sudo mkdir -p $TOP/$STORAGE_DIR/dev/pts
		else
			BUILD_STO=no
		fi
	fi

	echo "Installing busybox..."
	cp -av $TOP/obj/busybox-$ARCH/_install/* $TOP/$INITRAMFS_DIR
	if [ "xno" != "x${BUILD_STO}" ]; then
		sudo cp -av $TOP/obj/busybox-$ARCH/_install/* $TOP/$STORAGE_DIR
	fi
	build_initramfs_busybox
	# busybox need special handling
	#install_initramfs $TOP/obj/busybox-$ARCH/_install

	echo "Generating customizables..."
	rm -rf $TOP/obj/rootfs
	mkdir -p $TOP/obj/rootfs/etc
	echo $HOSTNAME > $TOP/obj/rootfs/etc/hostname
	if [ "xno" != "x${BUILD_STO}" ]; then
		sudo mkdir -p $TOP/$STORAGE_DIR/etc
		sudo echo $HOSTNAME > $TOP/$STORAGE_DIR/etc/hostname
		if [ "x${BUILD_STO_DEV}" != "x" ]; then
			sudo echo -n "/dev/${BUILD_STO_DEV}" > \
				$TOP/obj/rootfs/sdfirm_root
			sudo echo -n "/dev/${BUILD_STO_DEV}" > \
				$TOP/$STORAGE_DIR/sdfirm_root
		fi
	fi

	# TODO: Smarter way to build rootfs
	# Currently we only lists files in config-initramfs-${ARCH}

	# Install non-customizables
	echo "Installing rootfs fixed ${SCRIPT}/rootfs..."
	install_initramfs ${SCRIPT}/rootfs
	#if [ "xno" != "x${BUILD_NET}" ]; then
	#	install_initramfs ${SCRIPT}/features/net
	#fi
	#if [ "xno" != "x${BUILD_STO}" ]; then
	#	install_initramfs ${SCRIPT}/features/sto
	#fi

	# Install customizables
	echo "Installing rootfs no-fixed ${TOP}/obj/rootfs..."
	install_initramfs ${TOP}/obj/rootfs

	# Copy libraries
	if [ "x${BUILD_LIB}" != "xno" ]; then
		SYSROOT=`get_sysroot`
		echo "Installing rootfs toolchain ${SYSROOT}..."
		install_initramfs_sysroot ${SYSROOT}
		#install_initramfs ${SYSROOT} /sbin
		#install_initramfs ${SYSROOT} /lib
		#install_initramfs ${SYSROOT} /usr/bin
		#install_initramfs ${SYSROOT} /usr/sbin
		#install_initramfs ${SYSROOT} /usr/lib
		#install_initramfs ${SYSROOT} /usr/libexec
	fi

	if [ -x $TOP/obj/bench ]; then
		echo "Installing rootfs testbench $TOP/obj/bench..."
		install_initramfs $TOP/obj/bench
	fi
	if [ -x $TOP/obj/linux-modules ]; then
		echo "Installing rootfs modules $TOP/obj/linux-modules..."
		INSTALL_LINK=no
		install_initramfs $TOP/obj/linux-modules
		INSTALL_LINK=yes
	fi

	# Following stuffs are not installed in initramfs
	export INSTALL_INITRAMFS=no

	echo "Use INITRAMFS_SOURCE file list: $INITRAMFS_FILELIST"
	grep INITRAMFS_SOURCE $SCRIPT/config/$LINUX_CONFIG
	echo "So initramfs is built not here now but together with kernel later"
	#cat $TOP/$INITRAMFS_FILELIST

	if [ "xno" != "x${BUILD_STO}" ]; then
		sudo umount $TOP/$STORAGE_DIR
	fi
}

function build_linux()
{
	BUILD_MOD=no
	if [ "x$1" = "xv" ]; then
		echo "== Build Linux (G) =="
		LINUX_BUILD=$TOP/obj/vlinux-$ARCH
		BUILD_VLINUX=yes
	else
		echo "== Build Linux (S/H) =="
		LINUX_BUILD=$TOP/obj/linux-$ARCH
		BUILD_VLINUX=no
	fi
	if [ "xyes" = "x${FORCE_REBUILD}" ]; then
		rm -rf $LINUX_BUILD
	fi
	mkdir -p $LINUX_BUILD
	(
	cd $LINUX_PATH
	if [ "xyes" = "x${FORCE_REBUILD}" ]; then
		echo "Rebuilding $LINUX_CONFIG..."
		make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE distclean
		# Doing modcfgs in the original directory and save my_defconfig
		cp $SCRIPT/config/$LINUX_CONFIG arch/riscv/configs/my_defconfig
		if [ "xyes" = "x${BUILD_TINY}" ]; then
			apply_modcfg linux my_defconfig e_tiny.cfg
		fi
		if [ "xyes" = "x${BUILD_KVM}" ]; then
			if [ "xno" = "x${BUILD_VLINUX}" ]; then
				BUILD_MOD=yes
			fi
			BUILD_NET=yes
		fi
		if [ "xyes" = "x${BUILD_UEFI}" ]; then
			apply_modcfg linux my_defconfig e_uefi.cfg
		fi
		if [ "xno" = "x${BUILD_SMP}" ]; then
			apply_modcfg linux my_defconfig d_smp.cfg
		fi
		if [ "xno" = "x${BUILD_NET}" ]; then
			apply_modcfg linux my_defconfig d_net.cfg
		fi
		if [ "xno" = "x${BUILD_STO}" ]; then
			apply_modcfg linux my_defconfig d_sto.cfg
		fi
		if [ "xyes" = "x${BUILD_MOD}" ]; then
			apply_modcfg linux my_defconfig e_mod.cfg
		fi
		if [ "xyes" = "x${BUILD_KVM}" ]; then
			apply_modcfg linux my_defconfig e_kvm.cfg
		fi
		make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE mrproper
		# Starting the build process
		make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
			O=$LINUX_BUILD/ my_defconfig
		if [ "xyes" = "x${BUILD_VLINUX}" ]; then
			ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
				$LINUX_PATH/scripts/config \
				--file $LINUX_BUILD/.config \
				--set-str INITRAMFS_SOURCE $TOP/$INITRAMFS_FILELIST
		else
			ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
				$LINUX_PATH/scripts/config \
				--file $LINUX_BUILD/.config \
				--set-str INITRAMFS_SOURCE $TOP/$INITRAMFS_FILELIST
		fi
		make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
			O=$LINUX_BUILD/ clean
	fi
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
		O=$LINUX_BUILD/ -j`nproc`
	${CROSS_COMPILE}objcopy \
		--only-keep-debug $LINUX_BUILD/vmlinux \
		$LINUX_BUILD/kernel.sym
	if [ "xyes" = "x${BUILD_VLINUX}" ]; then
		if [ ! -f $LINUX_BUILD/vmlinux ]; then
			echo "Error: Failed to build Linux (G)"
			exit 1
		fi
		#make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
		#	O=$LINUX_BUILD/ \
		#	INSTALL_PATH=$DESTDIR install
		cp -f $LINUX_BUILD/arch/$ARCH/boot/Image ${APPDIR}/
	else
		if [ ! -f $LINUX_BUILD/arch/$ARCH/kvm/kvm.ko ]; then
			echo "Error: Failed to build KVM"
			exit 1
		fi
	fi
	if [ "xyes" = "x${BUILD_MOD}" ]; then
		if [ ! -f $LINUX_BUILD/vmlinux ]; then
			echo "Error: Failed to build Linux (S/H) modules"
			exit 1
		fi
		mkdir -p $MODSDIR
		make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
			O=$LINUX_BUILD/ \
			INSTALL_MOD_STRIP=1 \
			INSTALL_MOD_PATH=$MODSDIR modules_install
		sync
		build_initramfs ${BUILD_STO_SIZE}
		make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
			O=$LINUX_BUILD/ -j`nproc`
		if [ ! -f $LINUX_BUILD/vmlinux ]; then
			echo "Error: Failed to build Linux (S/H)"
			exit 1
		fi
	fi
	)
	cp -f ${TOP}/obj/linux-riscv/arch/${ARCH}/boot/Image ${SDFIRM_DIR}/Image
}

function build_sdfirm()
{
	echo "== Build sdfirm =="
	echo "Building ${MACH}_bbl_defconfig..."
	rm -rf $TOP/obj/sdfirm-$ARCH
	mkdir -p $TOP/obj/sdfirm-$ARCH
	(
	cd $SDFIRM_PATH
	if [ -x $TOP/obj/sdfirm-$ARCH ]; then
		make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
			O=$TOP/obj/sdfirm-$ARCH/ distclean
	fi
	# Doing modcfgs in the original directory and save my_defconfig
	cp arch/${ARCH}/configs/${MACH}_bbl_defconfig arch/riscv/configs/my_defconfig
	if [ "xno" = "x${BUILD_SMP}" ]; then
		apply_modcfg sdfirm my_defconfig d_smp.cfg
	fi
	if [ "xyes" = "x${BUILD_KVM}" ]; then
		apply_modcfg sdfirm my_defconfig e_kvm.cfg
	fi
	if [ "xno" = "x${SPACET_S2C_SPEEDUP}" ]; then
		apply_modcfg sdfirm my_defconfig d_k1m_s2c_speedup.cfg
	fi
	if [ "x" != "x${SPACET_CPU}" ]; then
		apply_modcfg sdfirm my_defconfig e_k1m_cpu${SPACET_CPU}.cfg
	fi
	if [ "x" != "x${SPACET_PG}" ]; then
		apply_modcfg sdfirm my_defconfig e_k1m_pg${SPACET_PG}.cfg
	fi
	if [ "x" != "x${SPACET_DDR}" ]; then
		apply_modcfg sdfirm my_defconfig e_k1m_ddr${SPACET_DDR}.cfg
	fi
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE mrproper
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
		O=$TOP/obj/sdfirm-$ARCH/ my_defconfig
	ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPLE $SDFIRM_PATH/scripts/config \
		--file $TOP/obj/sdfirm-$ARCH/.config \
		--set-str SBI_PAYLOAD_PATH \
		$TOP/obj/linux-$ARCH/arch/$ARCH/boot/Image
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
		O=$TOP/obj/sdfirm-$ARCH/ clean
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
		O=$TOP/obj/sdfirm-$ARCH/ -j`nproc`
	if [ ! -f $TOP/obj/sdfirm-$ARCH/sdfirm ]
	then
		echo "Error: Failed to build sdfirm"
		exit 1
	fi
	${CROSS_COMPILE}objcopy \
		--only-keep-debug $TOP/obj/sdfirm-$ARCH/sdfirm \
		$TOP/obj/sdfirm-$ARCH/sdfirm.sym
	)
}

function build_riscv-pk()
{
	echo "== Build riscv-pk =="
	rm -rf $TOP/$BBL_DIR
	mkdir -pv $TOP/$BBL_DIR
	(
	cd $BBL_DIR
	$SCRIPT/riscv-pk/configure \
		--enable-logo --host=riscv64-unknown-linux-gnu \
		--with-payload=$TOP/obj/linux-$ARCH/vmlinux
	make
	)
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

function build_uboot()
{
	echo "== Build u-boot =="
	echo "Building ${MACH}_spl_defconfig..."
	rm -rf $TOP/obj/u-boot-$ARCH
	mkdir -p $TOP/obj/u-boot-$ARCH
	(
	cd $UBOOT_PATH
	if [ -x $TOP/obj/u-boot-$ARCH ]; then
		make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
			O=$TOP/obj/u-boot-$ARCH/ distclean
	fi
	# Doing modcfgs in the original directory and save my_defconfig
	cp arch/${ARCH}/configs/${MACH}_spl_defconfig arch/riscv/configs/my_defconfig
	#if [ "xno" = "x${BUILD_SMP}" ]; then
	#	apply_modcfg u-boot my_defconfig d_smp.cfg
	#fi
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE mrproper
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
		O=$TOP/obj/u-boot-$ARCH/ my_defconfig
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
		O=$TOP/obj/u-boot-$ARCH/ clean
	make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE \
		O=$TOP/obj/u-boot-$ARCH/ -j`nproc`
	if [ ! -f $TOP/obj/u-boot-$ARCH/u-boot ]
	then
		echo "Error: Failed to build u-boot"
		exit 1
	fi
	${CROSS_COMPILE}objcopy \
		--only-keep-debug $TOP/obj/u-boot-$ARCH/u-boot \
		$TOP/obj/u-boot-$ARCH/u-boot.sym
	)
	#cp -v $TOP/obj/u-boot-$ARCH/spl/u-boot-spl.bin
	#cp -v $TOP/obj/u-boot-$ARCH/u-boot.itb
}

function build_boot()
{
	if [ "x$BOOT" = "xuboot" ]; then
		build_uboot
	fi
	if [ "x$BOOT" = "xedk2" ]; then
		build_edk2
	fi
}

cd $TOP

usage()
{
	echo "Usage:"
	echo "`basename $0` [-m bbl] [-s] [-u] [-a] [-r] [-n hostname] [target]"
	echo "              [-d feat] [-e feat]"
	echo "Where:"
	echo " -m bbl:      specify build of M-mode program (default sdfirm)"
	echo "              bbl includes:"
	echo "    sdfirm:   SDFIRM modified opensbi"
	echo "    riscv-pk: original BBL program"
	echo "    opensbi:  RISC-V opensbi"
	echo " -s:          specify build of S-mode program"
	echo " -u:          specify build of U-mode programs"
	echo " -a:          specify build of all modes programs"
	echo " -b:          specify build of boot loader (default uboot)"
	echo "    uboot:    Das U-Boot linux bootloader"
	echo "    edk2:     UEFI EDK-II BIOS"
	echo " -r:          force rebuild of all modes programs"
	echo " -n:          specify system hostname (default sdfirm)"
	echo " -d feat:     disable special features"
	echo "              feature includes:"
	echo "    shared:   shared library support"
	echo "    smp:      SMP support in OSen"
	echo "    network:  network and telnet login support"
	echo " -e feat:     enable special features"
	echo "              feature includes:"
	echo "    tiny:     ultra tiny kernel/busybox support"
	echo "    storage:  storage and NVME rootfs support"
	echo "    kvm:      KVM virtualization support"
	echo " target:      specify build type (default build)"
	echo "  build       build specified modules (default mode)"
	echo "  clean       build specified modules"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "ab:d:e:m:n:rsu" opt
do
	case $opt in
	a) M_MODE=yes
	   S_MODE=yes
	   U_MODE=yes;;
	d) if [ "x$OPTARG" = "xshared" ]; then
		BUILD_LIB=no
	   fi
	   if [ "x$OPTARG" = "xsmp" ]; then
		BUILD_SMP=no
	   fi
	   if [ "x$OPTARG" = "xnetwork" ]; then
		BUILD_NET=no
	   fi
	   if [ "x$OPTARG" = "xstorage" ]; then
		BUILD_STO=no
	   fi;;
	e) if [ "x$OPTARG" = "xtiny" ]; then
		BUILD_TINY=yes
	   fi
	   if [ "x$OPTARG" = "xkvm" ]; then
		BUILD_KVM=yes
	   fi;;
	m) M_MODE=yes
	   BBL=$OPTARG;;
	b) B_MODE=yes
	   BOOT=$OPTARG;;
	n) HOSTNAME=$OPTARG;;
	r) FORCE_REBUILD=yes;;
	s) S_MODE=yes;;
	u) U_MODE=yes;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

if [ -z $CROSS_COMPILE ]; then
	CROSS_COMPILE=riscv64-unknown-linux-gnu-
fi
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

echo "== Prepare =="
if [ "x${M_MODE}" = "xyes" ]; then
	if [ -z $BBL ]; then
		BBL=riscv-pk
	fi
	if [ "x$BBL" = "xsdfirm" ]; then
		if [ -z $SDFIRM_DIR ]; then
			SDFIRM_DIR=sdfirm
			SDFIRM_PATH=$TOP/sdfirm
		else
			if [ ! -d $SDFIRM_DIR ]; then
				echo "Sdfirm source $SDFIRM_DIR not found"
				exit 1
			fi
			SDFIRM_PATH=`(cd $SDFIRM_DIR; pwd)`
			SDFIRM_DIR=`dirname $SDFIRM_PATH`
		fi
		if [ -z $MACH ]; then
			MACH=spike64
		fi
	fi
fi
if [ "x${S_MODE}" = "xyes" ]; then
	if [ ! -f $SCRIPT/config/$LINUX_CONFIG ]; then
		echo "Linux config not found $LINUX_CONFIG"
		exit 1
	fi
	if [ ! -d $LINUX_DIR ]; then
		echo "Linux source $LINUX_DIR not found"
		exit 1
	fi
fi
if [ "x${U_MODE}" = "xyes" ]; then
	if [ ! -f $SCRIPT/config/$BUSYBOX_CONFIG ]; then
		echo "Busybox config not found $BUSYBOX_CONFIG"
		exit 1
	fi
	if [ ! -d $BUSYBOX_DIR ]; then
		echo "Busybox source $BUSYBOX_DIR not found"
		exit 1
	fi
fi

if [ "x$1" = "xclean" ]; then
	clean_all
else
	if [ "x${U_MODE}" = "xyes" ]; then
		if [ "x${BUILD_KVM}" = "xyes" ]; then
			build_libfdt
			build_kvmtool
			build_initramfs ${BUILD_STO_SIZE}
			build_linux v
		fi
		build_busybox
		build_initramfs ${BUILD_STO_SIZE}
	fi
	if [ "x${S_MODE}" = "xyes" ]; then
		build_linux
	fi
	if [ "x${M_MODE}" = "xyes" ]; then
		build_bbl
		if [ "x${B_MODE}" = "xyes" ]; then
			build_boot
		fi
	fi
fi
