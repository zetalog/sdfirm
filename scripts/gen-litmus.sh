#!/bin/bash

SCRIPT=`(cd \`dirname $0\`; pwd)`
SRCDIR=`(cd $SCRIPT/..; pwd)`

ARCH=riscv
MACH=spike64
CROSS_COMPILE=riscv64-linux-gnu-

LITMUS_SRCS=$SRCDIR/tests/riscv/litmus/
LITMUS_ELFS=$LITMUS_SRCS
LITMUS_CMD="-st 2 -s 5 -r 2"
LITMUS_JOBS=8
LITMUS_RFSH=no

usage()
{
	echo "Usage:"
	echo "`basename $0` [-a arch] [-m mach] [-c comp]"
	echo "		[-j jobs] [-o output] [-r] [test]"
	echo "Where:"
	echo " -a arch: specify CPU architecture"
	echo " -m mach: specify SoC architecure"
	echo " -c comp: specify cross compiler prefix"
	echo " -j jobs: specify number of build threads"
	echo " -o path: specify ELF output directory"
	echo " -r     : refresh ELF output results"
	echo "    test: specify a single test"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "a:m:c:j:o:r" opt
do
	case $opt in
	a) ARCH=$OPTARG;;
	m) MACH=$OPTARG;;
	c) CROSS_COMPILE=$OPTARG;;
	j) LITMUS_JOBS=$OPTARG;;
	o) LITMUS_ELFS=$OPTARG;;
	r) LITMUS_RFSH=yes;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

parse_litmus()
{
	cat $@ | awk '{							\
		if (match($0, /^obj-\$\(CONFIG_/)) {			\
			rem=substr($0, RLENGTH+1);			\
			if (match(rem, /[A-Za-z0-9_]+\) \+\=/)) {	\
				name=substr(rem, 0, RLENGTH-4);		\
				file=substr(rem, RLENGTH+1);		\
				print name;				\
			}						\
		}							\
	}'
}

LITMUS_DEF=${MACH}_litmus_defconfig
LITMUS_CFG=${SRCDIR}/arch/${ARCH}/configs/${LITMUS_DEF}
LITMUS_LOG=${LITMUS_ELFS}/litmus_build.log
LITMUS_INCL=${LITMUS_ELFS}/incl
LITMUS_EXCL=${LITMUS_ELFS}/excl
export CROSS_COMPILE
export ARCH

if [ -z $1 ]; then
	echo "Building all cases..."
else
	echo "Building $1 case..."
	LITMUS_CASE=$1
fi

build_litmus()
{
	(
		cd $SRCDIR
		CASE=$1
		CMD="-st 2 -s 5 -r 2"
		make $LITMUS_DEF > /dev/null
		cp $SRCDIR/.config $SRCDIR/defconfig
		cat $SRCDIR/defconfig | awk '{				\
		if (match($0, /^CONFIG_COMMAND_BATCH_COMMAND/)) {	\
			cfg=substr($0, 0, RLENGTH+2);			\
			print cfg "'$CASE' -st 2 -s 5 -r 2\"";		\
		} else if (match($0, /^CONFIG_LITMUS_RISCV_DUMMY/)) {	\
			print "# CONFIG_LITMUS_RISCV_DUMMY is not set"	\
		} else if (match($0, /CONFIG_'$CASE' is not set/)) {	\
			print "CONFIG_" "'$CASE'" "=y";			\
		} else {						\
			print
		}}' > $SRCDIR/.config
		rm -f $SRCDIR/defconfig
		make clean > /dev/null
		echo "===== $CASE =====" | tee -a $LITMUS_LOG
		make -j $LITMUS_JOBS > /dev/null 2>>$LITMUS_LOG
	)
	built=$?

	if [ $built -eq 0 ]; then
		echo "$1 success." | tee -a $LITMUS_LOG
		cp -f $SRCDIR/.config $LITMUS_ELFS/$1.cfg
		cp -f $SRCDIR/sdfirm $LITMUS_ELFS/$1.elf
		if [ -f ${SRCDIR}/arch/riscv/boot/sdfirm.rom ]; then
			cp -f ${SRCDIR}/arch/riscv/boot/sdfirm.rom \
			      ${LITMUS_ELFS}/$1.rom
		fi
		if [ -f ${SRCDIR}/arch/riscv/boot/sdfirm.ram ]; then
			cp -f ${SRCDIR}/arch/riscv/boot/sdfirm.ram \
			      ${LITMUS_ELFS}/$1.ram
		fi
		return 0
	else
		echo "$1 failure." | tee -a $LITMUS_LOG
		return 1
	fi
}

refresh_build()
{
	while :
	do
		sleep 2
		(
			cd $LITMUS_SRCS
			ls *.elf -tr
		)
	done
}

if [ "x$LITMUS_RFSH" = "xyes" ]; then
	refresh_build
else
	litmus_tsts=`parse_litmus $LITMUS_SRCS/Makefile.litmus`

	rm -f ${LITMUS_ELFS}/*.elf
	rm -f ${LITMUS_ELFS}/*.rom
	rm -f ${LITMUS_ELFS}/*.ram
	find ${LITMUS_ELFS} -not -name "make.cfg" -and -name "*.cfg" \
	       | xargs rm -f
	echo -n "" > $LITMUS_LOG
	echo -n "" > $LITMUS_INCL
	echo -n "" > $LITMUS_EXCL

	if [ -z $LITMUS_CASE ]; then
		for t in $litmus_tsts; do
			build_litmus $t
			if [ $? -eq 0 ]; then
				echo $t >> $LITMUS_INCL
			else
				echo $t >> $LITMUS_EXCL
			fi
		done
	else
		build_litmus $LITMUS_CASE
	fi
fi
