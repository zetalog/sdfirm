#!/bin/sh
#
# Run spike with OpenOCD Enabled
#
# This script is used to run spike and sdfirm with openocd enabled.
# After running this script, type openocd -f arch/riscv/mach-spike/openocd-spike64.cfg.

SCRIPT=`(cd \`dirname $0\`; pwd)`

SPIKE_OPTS=
SPIKE_RBB_PORT=9824

usage()
{
	echo "Usage:"
	echo "`basename $0` [-p procs] [-j port] [-H]"
	echo "Where:"
	echo " -p num-cpus: specify number of CPUs"
	echo " -j rbb-port: specify jtag remote bitbang port (default 9824)"
	echo " -H:          enable halt on reset"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "j:p:H" opt
do
	case $opt in
	j) SPIKE_RBB_PORT=$OPTARG;;
	p) SPIKE_OPTS="-p$OPTARG ${SPIKE_OPTS}";;
	H) SPIKE_OPTS="${SPIKE_OPTS} -H";;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

spike ${SPIKE_OPTS} --rbb-port=${SPIKE_RBB_PORT} ${SCRIPT}/../sdfirm
