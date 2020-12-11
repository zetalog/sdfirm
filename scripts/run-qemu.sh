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
	echo "`basename $0` [-p procs] [-t file]"
	echo "Where:"
	echo " -p num-cpus: specify number of CPUs"
	echo " -t dts-file: dump device tree string file"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "hnp:t:" opt
do
	case $opt in
	h) usage 0;;
	n) QEMU_OPTS="-netdev type=tap,script=qemu-ifup,downscript=qemu-ifdown,id=net0 -device virtio-net-device,netdev=net0 ${QEMU_OPTS}";;
	p) QEMU_OPTS="-smp $OPTARG ${QEMU_OPTS}";;
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
