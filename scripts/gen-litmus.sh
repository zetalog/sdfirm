#!/bin/bash
#
# Generate litmus test case ELF executables.

SCRIPT=`(cd \`dirname $0\`; pwd)`
SRCDIR=`(cd ${SCRIPT}/..; pwd)`

ARCH=riscv
MACH=spike64
CROSS_COMPILE=riscv64-linux-

LITMUS_SRCS=${SRCDIR}/tests/riscv/litmus/
LITMUS_ELFS=${LITMUS_SRCS}
LITMUS_JOBS=8
LITMUS_STRIDE=2
LITMUS_SIZE_OF_TEST=5
LITMUS_MAX_RUN=1
LITMUS_CONT=no

LITMUS_WAIT=no

usage()
{
	echo "Usage:"
	echo "`basename $0` [-a arch] [-m mach] [-g cross]"
	echo "              [-j jobs] [-o path]"
	echo "              [-r max_run] [-s stride] [-t size_of_test]"
	echo "              [-b test] [-w] [test]"
	echo "Where:"
	echo " Options:"
	echo " -a arch:         specify CPU architecture"
	echo " -m mach:         specify SoC architecure"
	echo " -g cross:        specify GCC cross compiler prefix"
	echo " -j jobs:         specify number of build threads"
	echo " -o path:         specify ELF output directory"
	echo " -b test:         continue ELF generation from test"
	echo " -w:              wait and refresh ELF output results"
	echo " -r max_run:      litmus max runs (-r)"
	echo " -s stride:       litmus stride (-st)"
	echo " -t size_of_test: litmus size_of_test (-s)"
	echo " Parameters:"
	echo " test:            specify a single test"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "a:b:m:g:j:o:r:s:t:w" opt
do
	case $opt in
	a) ARCH=$OPTARG;;
	b) LITMUS_CONT=yes
	   LITMUS_CONT_CASE=$OPTARG;;
	m) MACH=$OPTARG;;
	g) CROSS_COMPILE=$OPTARG;;
	j) LITMUS_JOBS=$OPTARG;;
	o) LITMUS_ELFS=$OPTARG;;
	r) LITMUS_MAX_RUN=$OPTARG;;
	s) LITMUS_STRIDE=$OPTARG;;
	t) LITMUS_SIZE_OF_TEST=$OPTARG;;
	w) LITMUS_WAIT=yes;;
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
	echo "Building all cases on ${MACH}..."
else
	echo "Building $1 case on ${MACH}..."
	LITMUS_CASE=$1
fi

build_litmus()
{
	(
		cd ${SRCDIR}
		CASE=$1
		make ${LITMUS_DEF} > /dev/null
		cp ${SRCDIR}/.config ${SRCDIR}/defconfig
		cat ${SRCDIR}/defconfig | awk '{			\
		if (match($0, /^CONFIG_COMMAND_BATCH_COMMAND/)) {	\
			cfg=substr($0, 0, RLENGTH+2);			\
			print cfg "'$CASE' -st '$LITMUS_STRIDE' -s '$LITMUS_SIZE_OF_TEST' -r '$LITMUS_MAX_RUN'\"";		\
		} else if (match($0, /^CONFIG_LITMUS_RISCV_DUMMY/)) {	\
			print "# CONFIG_LITMUS_RISCV_DUMMY is not set"	\
		} else if (match($0, /CONFIG_'$CASE' is not set/)) {	\
			print "CONFIG_" "'$CASE'" "=y";			\
		} else {						\
			print
		}}' > ${SRCDIR}/.config
		rm -f ${SRCDIR}/defconfig
		make clean > /dev/null
		echo "===== $CASE =====" | tee -a ${LITMUS_LOG}
		make -j ${LITMUS_JOBS} > /dev/null 2>>${LITMUS_LOG}
	)
	built=$?

	if [ $built -eq 0 ]; then
		echo "$1 success." | tee -a ${LITMUS_LOG}
		cp -f ${SRCDIR}/.config ${LITMUS_ELFS}/$1.cfg
		cp -f ${SRCDIR}/sdfirm ${LITMUS_ELFS}/$1.elf
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
		echo "$1 failure." | tee -a ${LITMUS_LOG}
		cp -f ${SRCDIR}/.config ${LITMUS_ELFS}/$1.cfg
		return 1
	fi
}

refresh_build()
{
	while :
	do
		sleep 2
		(
			cd ${LITMUS_SRCS}
			ls *.elf -tr
		)
	done
}

if [ "x${LITMUS_WAIT}" = "xyes" ]; then
	refresh_build
else
	litmus_tsts=`parse_litmus ${LITMUS_SRCS}/Makefile.litmus`

	if [ "x${LITMUS_CONT}" != "xyes" ]; then
		echo "Removing litmus binary from ${LITMUS_ELFS}..."
		rm -f ${LITMUS_ELFS}/*.elf
		rm -f ${LITMUS_ELFS}/*.rom
		rm -f ${LITMUS_ELFS}/*.ram
		if ls ${LITMUS_ELFS}/*.cfg >/dev/null 2>&1; then
			echo -n ""
		else
			ls ${LITMUS_ELFS}/*.cfg | grep -v make.cfg | xargs rm -f
		fi
		echo -n "" > ${LITMUS_LOG}
		echo -n "" > ${LITMUS_INCL}
		echo -n "" > ${LITMUS_EXCL}
	else
		LITMUS_CONT_START=no
	fi

	if [ -z ${LITMUS_CASE} ]; then
		echo "Generating litmus binary to ${LITMUS_ELFS}..."
		for t in ${litmus_tsts}; do
			if [ "x${LITMUS_CONT}" = "xyes" ]; then
				if [ "x${LITMUS_CONT_START}" = "xno" ]; then
					if [ "x${t}" != "x${LITMUS_CONT_CASE}" ]; then
						continue
					else
						LITMUS_CONT_START=yes
					fi
				fi
			fi
			build_litmus $t
			if [ $? -eq 0 ]; then
				echo $t >> ${LITMUS_INCL}
			else
				echo $t >> ${LITMUS_EXCL}
			fi
		done
	else
		echo "Generating ${LITMUS_CASE} binary to ${LITMUS_ELFS}..."
		build_litmus ${LITMUS_CASE}
		if [ $? -eq 0 ]; then
			echo ${LITMUS_CASE} >> ${LITMUS_INCL}
		else
			echo ${LITMUS_CASE} >> ${LITMUS_EXCL}
		fi
	fi
fi
