#!/bin/bash
#
# Run spike with OpenOCD Enabled
#
# This script is used to run spike and sdfirm with openocd enabled.
# After running this script, type openocd -f arch/riscv/mach-spike/openocd-spike64.cfg.

SCRIPT=`(cd \`dirname $0\`; pwd)`

SPIKE_OPTS=
SPIKE_RBB_PORT=9824
SPIKE_RBB=yes
SPIKE_PIPE=
SPIKE_ROM_SIZE=0x4000
SPIKE_MEM_BASE=0x80000000
SPIKE_MEM_SIZE=0x80000000
SPIKE_MEM_CUST=no
SPIKE_PROG=sdfirm

usage()
{
	echo "Usage:"
	echo "`basename $0` [-p procs]"
	echo "              [-b base] [-s size] [-e entry]"
	echo "              [-j port] [-H]"
        echo "              [-t file]"
	echo "              [-d] [-l]"
	echo "Where:"
	echo " -p num-cpus: specify number of CPUs"
	echo " -b mem-base: specify first memory region base"
	echo " -s mem-size: specify first memory region size"
	echo " -e entry:    specify entry point"
	echo " -j rbb-port: specify jtag remote bitbang port (default 9824)"
	echo " -H:          enable halt on reset"
	echo " -t dts-file: dump device tree string file"
	echo " -d:          enable single step debug"
	echo " -l:          enable execution log"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "b:de:hj:lp:s:t:H" opt
do
	case $opt in
	b) SPIKE_MEM_BASE=$OPTARG
	   SPIKE_MEM_CUST=yes;;
	s) SPIKE_MEM_SIZE=$OPTARG
	   SPIKE_MEM_CUST=yes;;
	e) SPIKE_OPTS="${SPIKE_OPTS} --pc=$OPTARG";;
	d) SPIKE_OPTS="${SPIKE_OPTS} -d";;
	h) usage 0;;
	j) SPIKE_RBB_PORT=$OPTARG
	   SPIKE_RBB=yes;;
	l) SPIKE_OPTS="${SPIKE_OPTS} -l";;
	p) SPIKE_OPTS="-p$OPTARG ${SPIKE_OPTS}";;
	t) SPIKE_OPTS="--dump-dts ${SPIKE_OPTS}"
	   SPIKE_PIPE=">$OPTARG"
	   SPIKE_RBB=no;;
	H) SPIKE_OPTS="${SPIKE_OPTS} -H"
	   SPIKE_RBB=yes;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

if [ -z $1 ]; then
	SPIKE_PROG=${SCRIPT}/../sdfirm
else
	SPIKE_PROG=$1
fi

if [ "x${SPIKE_RBB}" = "xyes" ]; then
	SPIKE_OPTS="--rbb-port=${SPIKE_RBB_PORT} ${SPIKE_OPTS}"
fi
if [ "x${SPIKE_MEM_CUST}" = "xyes" ]; then
	((ram_size=$SPIKE_MEM_BASE))
	((rom_size=$SPIKE_ROM_SIZE))
	if [ $ram_size -lt $rom_size ]; then
		echo "RAM base (${SPIKE_MEM_BASE}) less than ROM size (${SPIKE_ROM_SIZE})!"
		exit 1
	fi
	SPIKE_OPTS="-m${SPIKE_MEM_BASE}:${SPIKE_MEM_SIZE} ${SPIKE_OPTS}"
fi

echo "spike ${SPIKE_OPTS} ${SPIKE_PROG} ${SPIKE_PIPE}"
eval spike ${SPIKE_OPTS} ${SPIKE_PROG} ${SPIKE_PIPE}
