#!/bin/bash
#
# Run spike with debugging facility enabled
#
# This script is used to run spike and sdfirm with jtag/debug/trace
# enabled.
# 1. JTAG mode:
#    Run spike with JTAG enabled:
#    $ run-spike.sh -j 9824
#    Launch openocd:
#    $ openocd -f arch/riscv/mach-spike/openocd-spike64.cfg
#    Launch gdb:
#    $ riscv64-linux-gnu-gdb
#    And see arch/riscv/mach-spike/readme.txt for more information.
# 2. Embedded debugger/logger mode:
#    $ run-spike.sh -d -l
# 3. DTS dumper:
#    $ run-spike.sh -t spike.dts

SCRIPT=`(cd \`dirname $0\`; pwd)`

SPIKE_OPTS=
SPIKE_RBB_PORT=9824
SPIKE_RBB=no
SPIKE_PIPE=
SPIKE_ROM_SIZE=0x4000
SPIKE_MEM_BASE=0x80000000
SPIKE_MEM_SIZE=0x80000000
SPIKE_MEM_CUST=no
SPIKE_PROG=sdfirm
SPIKE_TRACE=stderr

usage()
{
	echo "Usage:"
	echo "`basename $0` [-p procs]"
	echo "              [-b base] [-s size] [-e entry]"
	echo "              [-j port]"
	echo "              [-t dts]"
	echo "              [-c] [-d] [-l trace]"
	echo "Where:"
	echo " -p num-cpus:  specify number of CPUs"
	echo " -b mem-base:  specify first memory region base"
	echo " -s mem-size:  specify first memory region size"
	echo " -e entry:     specify entry point"
	echo " -j rbb-port:  enable jtag remote bitbang (default 9824)"
	echo " -t dts-file:  dump device tree string file"
	echo " -d:           enable single step debug"
	echo " -c            enable split cpu trace logs"
	echo " -l trace-log: enable execution log to files (default stderr)"
	echo "               where trace-log can be some special file:"
	echo "               stderr: output to the console"
	echo "               cpulog: split output to cpu*.log files"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "b:cde:hj:l:p:s:t:" opt
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
	l) SPIKE_OPTS="${SPIKE_OPTS} -l"
	   SPIKE_TRACE=$OPTARG;;
	c) SPIKE_OPTS="${SPIKE_OPTS} -l"
	   SPIKE_TRACE=cpulog;;
	p) SPIKE_OPTS="-p$OPTARG ${SPIKE_OPTS}";;
	t) SPIKE_OPTS="--dump-dts ${SPIKE_OPTS}"
	   SPIKE_PIPE=">$OPTARG"
	   SPIKE_RBB=no
	   SPIKE_TRACE=stderr;;
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
	SPIKE_OPTS="-H --rbb-port=${SPIKE_RBB_PORT} ${SPIKE_OPTS}"
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

split_cpulog()
{
	awk '{								\
		pfx="cpu";						\
		sfx=".log";						\
		if (match($0, /^core +/)) {				\
			rem=substr($0, RLENGTH+1);			\
			if (match(rem, /^[0-9]+/)) {			\
				cpu=substr(rem, 0, RLENGTH);		\
				ins=substr(rem, RLENGTH+3);		\
				fnm=pfx""cpu""sfx;			\
				print ins >fnm;				\
			}						\
		}							\
	}'
}

echo "spike ${SPIKE_OPTS} ${SPIKE_PROG} ${SPIKE_PIPE}"
if [ "x${SPIKE_TRACE}" = "xstderr" ]; then
	eval spike ${SPIKE_OPTS} ${SPIKE_PROG} ${SPIKE_PIPE}
elif [ "x${SPIKE_TRACE}" != "xcpulog" ]; then
	eval spike ${SPIKE_OPTS} ${SPIKE_PROG} ${SPIKE_PIPE} 2>${SPIKE_TRACE}
else
	rm -rf cpu*.log
	eval spike ${SPIKE_OPTS} ${SPIKE_PROG} ${SPIKE_PIPE} 2> >(split_cpulog)
fi
