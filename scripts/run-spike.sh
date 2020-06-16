#!/bin/sh
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

usage()
{
	echo "Usage:"
	echo "`basename $0` [-d] [-j port] [-l] [-p procs] [-t file] [-H]"
	echo "Where:"
	echo " -d:          enable single step debug"
	echo " -l:          enable execution log"
	echo " -j rbb-port: specify jtag remote bitbang port (default 9824)"
	echo " -p num-cpus: specify number of CPUs"
	echo " -t dts-file: dump device tree string file"
	echo " -H:          enable halt on reset"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "dhj:lp:t:H" opt
do
	case $opt in
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

if [ "x${SPIKE_RBB}" = "xyes" ]; then
	SPIKE_OPTS="--rbb-port=${SPIKE_RBB_PORT} ${SPIKE_OPTS}"
fi

eval spike ${SPIKE_OPTS} ${SCRIPT}/../sdfirm ${SPIKE_PIPE}
