#!/bin/bash
#
# Run spike with debugging facility enabled
#
# This script is used to run spike and sdfirm with jtag/debug/trace
# enabled.
# 1. Trace log mode (testbench with cpu trace and uart log):
#    $ run-spike.sh -l cpulog -u uartlog
# 2. JTAG debugger mode:
#    Run spike with JTAG enabled:
#    $ run-spike.sh -j 9824
#    Launch openocd:
#    $ openocd -f arch/riscv/mach-spike/openocd-spike64.cfg
#    Launch gdb:
#    $ riscv64-linux-gnu-gdb
#    And see arch/riscv/mach-spike/readme.txt for more information.
# 3. Embedded single step debugger mode:
#    $ run-spike.sh -d
# 4. DTS dumper mode:
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
	echo "`basename $0` [-p procs] [-b base] [-s size] [-e entry]"
	echo "              [-l trace] [-u uart]"
	echo "              [-j port] [-d]"
	echo "              [-t dts]"
	echo "Where:"
	echo "Simulator options:"
	echo " -p num-cpus:  specify number of CPUs"
	echo " -b mem-base:  specify first memory region base"
	echo " -s mem-size:  specify first memory region size"
	echo " -e entry:     specify entry point"
	echo "Tracer options:"
	echo " -l trace-log: enable execution log to files (default stderr)"
	echo "               where trace-log can be some special file:"
	echo "               stderr: output to the console"
	echo "               cpulog: split output to cpu*.log files"
	echo " -u uart-log:  enable serial console output to file"
	echo "Debugger options:"
	echo " -j rbb-port:  enable jtag remote bitbang (default 9824)"
	echo " -d:           enable single step debug"
	echo "Other options:"
	echo " -t dts-file:  dump device tree string file"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "b:de:hj:l:p:u:s:t:" opt
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
	l) SPIKE_OPTS="${SPIKE_OPTS} -l --log-commits"
	   SPIKE_TRACE=$OPTARG;;
	u) SPIKE_UART=$OPTARG;;
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
				ctx=substr(rem, RLENGTH+3);		\
				fnm=pfx""cpu""sfx;			\
				if (match(ctx, /^0x[0-9a-f]+ /)) {	\
					print ctx >fnm;			\
				} else if (match(ctx, /^>>>>  /)) {	\
					fun=substr(ctx, RLENGTH+1);	\
					print "***** "fun" *****" >fnm;	\
				} else {				\
					print " - " ctx >fnm;		\
				}					\
			}						\
		}							\
	}'
}

echo "spike ${SPIKE_OPTS} ${SPIKE_PROG} ${SPIKE_PIPE}"
if [ "x${SPIKE_TRACE}" = "xstderr" ]; then
	if [ "x${SPIKE_UART}" != "x" ]; then
		eval spike ${SPIKE_OPTS} ${SPIKE_PROG} ${SPIKE_PIPE} | tee ${SPIKE_UART}
	else
		eval spike ${SPIKE_OPTS} ${SPIKE_PROG} ${SPIKE_PIPE}
	fi
elif [ "x${SPIKE_TRACE}" != "xcpulog" ]; then
	if [ "x${SPIKE_UART}" != "x" ]; then
		eval spike ${SPIKE_OPTS} ${SPIKE_PROG} ${SPIKE_PIPE} 2>${SPIKE_TRACE} | tee ${SPIKE_UART}
	else
		eval spike ${SPIKE_OPTS} ${SPIKE_PROG} ${SPIKE_PIPE} 2>${SPIKE_TRACE}
	fi
else
	rm -rf cpu*.log*
	riscv64-linux-objdump -D -M numeric ${SPIKE_PROG} > ${SPIKE_PROG}.dis
	eval spike ${SPIKE_OPTS} ${SPIKE_PROG} ${SPIKE_PIPE} 2> >(split_cpulog)
fi
