#!/bin/sh
#
# Run QEMU virtio platform
#
# This script is used to run qemu

SCRIPT=`(cd \`dirname $0\`; pwd)`

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

while getopts "hp:t:" opt
do
	case $opt in
	h) usage 0;;
	p) QEMU_OPTS="-smp $OPTARG ${QEMU_OPTS}";;
	t) QEMU_DTS=$OPTARG;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

SCRIPT=`(cd \`dirname $0\`; pwd)`

ARCH=riscv64
QEMU=qemu-system-${ARCH}

if [ "x${QEMU_DTS}" != "x" ]; then
	QEMU_DTB="`echo ${QEMU_DTS%.*}`.dtb"
	QEMU_MOPTS=",dumpdtb=${QEMU_DTB}"
	QEMU_FIRM=""
else
	QEMU_FIRM="-bios ${SCRIPT}/../sdfirm"
fi

${QEMU} ${QEMU_OPTS} -machine virt${QEMU_MOPTS} ${QEMU_FIRM}

if [ "x${QEMU_DTS}" != "x" ]; then
	dtc -I dtb -O dts -o ${QEMU_DTS} ${QEMU_DTB}
fi
