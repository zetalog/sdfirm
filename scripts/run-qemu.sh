#!/bin/sh
#
# Run QEMU virtio platform
#
# This script is used to run qemu

SCRIPT=`(cd \`dirname $0\`; pwd)`

ARCH=riscv64
QEMU=qemu-system-${ARCH}
QEMU_OPTS=-nographic
QEMU_MOPTS=

usage()
{
	echo "Usage:"
	echo "`basename $0` [-b base] [-m size] [-p procs]"
	echo "              [-n] [-s drive]"
	echo "              [-l trace] [-u uart]"
	echo "              [-g port]"
	echo "              [-t dts]"
	echo " -p num-cpus: specify number of CPUs"
	echo " -b mem-base: specify first memory region base"
	echo " -m mem-size: specify first memory region size"
	echo " -t dts-file: dump device tree string file"
	echo " -l log-file: dump CPU trace to log file"
	echo " -g gdb-port: enable gdb server (default 1234)"
	echo " -s    drive: specify usage of storage device"
	echo " Where drivers are:"
	echo "        virt: use VirtIO emulation"
	echo "        nvme: use NVME emulation"
	echo " -n         : specify usage of network device (eth)"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "g:hl:m:np:s:t:" opt
do
	case $opt in
	m) QEMU_OPTS="-m $OPTARG";;
	h) usage 0;;
	g) QEMU_OPTS="-gdb tcp::$OPTARG -S ${QEMU_OPTS}";;
	l) QEMU_OPTS="-D $OPTARG -d cpu,exec,in_asm ${QEMU_OPTS}";;
	n) QEMU_OPTS="-netdev type=tap,script=qemu-ifup,downscript=qemu-ifdown,id=net0 -device virtio-net-device,netdev=net0 ${QEMU_OPTS}";;
	p) QEMU_OPTS="-smp $OPTARG ${QEMU_OPTS}";;
	s) if [ "x$OPTARG" = "xnvme" ]; then
		QEMU_OPTS="-device nvme,serial=deadbeef,drive=nvm -drive file=storage.img,if=none,id=nvm ${QEMU_OPTS}"
	   fi
	   if [ "x$OPTARG" = "xvirt" ]; then
		QEMU_OPTS="-hda storage.img -drive file=storage.img,if=virtio,id=drive-virtio-disk0 ${QEMU_OPTS}"
	   fi;;
	t) QEMU_DTS=$OPTARG;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

if [ -z $1 ]; then
	QEMU_PROG=${SCRIPT}/../sdfirm
else
	QEMU_PROG=$1
fi
if [ ! -f ${QEMU_PROG} ]; then
	echo "warning: No -bios option specified."
	echo "warning: This default will change in a future release."
	echo "warning: See deprecation documentation for details."
	QEMU_FIRM=""
else
	QEMU_FIRM="-bios ${QEMU_PROG}"
fi
if [ "x${QEMU_DTS}" != "x" ]; then
	QEMU_DTB="`echo ${QEMU_DTS%.*}`.dtb"
	QEMU_MOPTS=",dumpdtb=${QEMU_DTB}"
else
	QEMU_FIRM="-bios ${QEMU_PROG}"
fi

echo "${QEMU} ${QEMU_OPTS} -machine virt${QEMU_MOPTS} ${QEMU_FIRM}"
${QEMU} ${QEMU_OPTS} -machine virt${QEMU_MOPTS} ${QEMU_FIRM} 2>/dev/null

if [ "x${QEMU_DTS}" != "x" ]; then
	if [ -f ${QEMU_DTB} ]; then
		dtc -I dtb -O dts -o ${QEMU_DTS} ${QEMU_DTB}
		echo "DTS file (${QEMU_DTS}) is generated."
	else
		echo "error: DTS file is not generate."
		if [ "x${QEMU_FIRM}" = "x" ]; then
			echo "error: Try to use -bios to avoid breakage."
		fi
	fi
fi
