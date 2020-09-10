#!/bin/sh

SCRIPT=`(cd \`dirname $0\`; pwd)`
SRCDIR=`(cd $SCRIPT/..; pwd)`

LITMUS_SRCS=$SRCDIR/tests/riscv/litmus/
LITMUS_ARCH=riscv
LITMUS_MACH=spike64
LITMUS_ELFS=$LITMUS_SRCS
LITMUS_CMD="-st 2 -s 5 -r 2"
LITMUS_LOG=$LITMUS_SRCS/litmus_build.log
LITMUS_JOBS=8

usage()
{
	echo "Usage:"
	echo "`basename $0` [-a arch] [-m mach]"
	echo "		[-j jobs] [-o output]"
	echo "Where:"
	echo " -a arch: specify CPU architecture"
	echo " -j jobs: specify number of build threads"
	echo " -m mach: specify SoC architecure"
	echo " -o path: specify ELF output directory"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "a:m:o:" opt
do
	case $opt in
	a) LITMUS_ARCH=$OPTARG;;
	j) LITMUS_JOBS=$OPTARG;;
	m) LITMUS_MACH=$OPTARG;;
	o) LITMUS_ELFS=$OPTARG;;
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

LITMUS_DEF=${LITMUS_MACH}_litmus_defconfig
LITMUS_CFG=${SRCDIR}/arch/${LITMUS_ARCH}/configs/${LITMUS_DEF}

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
		} else if (match($0, /CONFIG_'$CASE'/)) {		\
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
		cp $SRCDIR/.config $LITMUS_ELFS/$1.cfg
		cp $SRCDIR/sdfirm $LITMUS_ELFS/$1.elf
		if [ -x $SRCDIR/arch/riscv/boot/sdfirm.rom ]; then
			cp ${SRCDIR}/arch/riscv/boot/sdfirm.rom	\
			   ${LITMUS_ELFS}/$1.rom
		fi
		if [ -x $SRCDIR/arch/riscv/boot/sdfirm.ram ]; then
			cp ${SRCDIR}/arch/riscv/boot/sdfirm.ram	\
			   ${LITMUS_ELFS}/$1.ram
		fi
	else
		echo "$1 failure." | tee -a $LITMUS_LOG
	fi
}

litmus_tsts=`parse_litmus $LITMUS_SRCS/Makefile.litmus`

rm -f ${LITMUS_ELFS}/*.elf
rm -f ${LITMUS_ELFS}/*.rom
rm -f ${LITMUS_ELFS}/*.ram

echo "" > $LITMUS_LOG
for t in $litmus_tsts; do
	build_litmus $t
done
