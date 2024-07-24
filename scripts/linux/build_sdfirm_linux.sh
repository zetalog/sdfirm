#!/bin/bash
#
# This scripts build sdfirm bootable Linux image with early/late test
# specified.

if [ -z $SDFIRM_ROOT ]; then
	echo "SDFIRM_ROOT must be specified!"
	echo "As this script is sourced by the invoker, you must specify"
	echo "the abstract path of this script via SDFIRM_ROOT"
	echo "environment."
	exit 1
fi

TOP=`pwd`
SCRIPT=`(cd ${SDFIRM_ROOT}/scripts/linux; pwd)`
WORKING_DIR=`(cd ${SCRIPT}/../../..; pwd)`
if [ -z $ARCH ]; then
	ARCH=riscv
fi
if [ -z $MACH ]; then
	MACH=spike64
	BBLVAR=
fi
if [ -z $CROSS_COMPILE ]; then
	CROSS_COMPILE=riscv64-linux-gnu-
fi
if [ -z $BUILD_ROOT ]; then
	BUILD_ROOT=$WORKING_DIR
fi
if [ -z $BACKUP_ROOT ]; then
	BACKUP_ROOT=$WORKING_DIR/backup
fi
if [ -z $LITMUS_CORES ]; then
	LITMUS_CORES=4
fi
if [ -z $LITMUS_UPDATE ]; then
	LITMUS_UPDATE=yes
fi
if [ -z $LITMUS_DUMP ]; then
	LITMUS_DUMP=no
fi
if [ -z $CPU2006_UPDATE ]; then
	CPU2006_UPDATE=yes
fi
if [ -z $PERF_UPDATE ]; then
	PERF_UPDATE=yes
fi

# Early tests: litmus, benchmark
# Late tests: dmatest, perf, loopback
if [ -z $TEST_EARLY ]; then
	TEST_EARLY=
fi
if [ -z $TEST_LATE ]; then
	TEST_LATE=
fi
# Control if customer's rootfs is specified
#BUILD_CUSTOMER=kuhan
# Update SDFIRM_ROOT to abstract path
SDFIRM_ROOT=${BUILD_ROOT}/sdfirm
if [ -z $LINUX_ROOT ]; then
	LINUX_ROOT=${BUILD_ROOT}/linux
fi
if [ -z $LITMUS_ROOT ]; then
	LITMUS_ROOT=${BUILD_ROOT}/litmus
fi
SCRIPT=${SDFIRM_ROOT}/scripts/linux

# Environments that are required to be exported
export ARCH
export MACH
export CROSS_COMPILE
export SDFIRM_ROOT
export LITMUS_ROOT

# Customer support
if [ -z ${BUILD_CUSTOMER} ]; then
	# Enable busybox and applications
	BUILD_IMAGE_OPS="-f -u -n sdfirm"
else
	# Enable busybox only
	BUILD_IMAGE_OPS="-f -n sdfirm"
fi
if [ "x${BUILD_TINY}" = "xyes" ]; then
	BUILD_IMAGE_OPS="$BUILD_IMAGE_OPS -e tiny"
fi
if [ "x${BUILD_LIB}" = "xno" ]; then
	BUILD_IMAGE_OPS="$BUILD_IMAGE_OPS -d shared"
fi
if [ "x${BUILD_SMP}" = "xno" ]; then
	BUILD_IMAGE_OPS="$BUILD_IMAGE_OPS -d smp"
fi
if [ "x${BUILD_KVM}" = "xyes" ]; then
	BUILD_IMAGE_OPS="$BUILD_IMAGE_OPS -e kvm"
fi
if [ "x${BUILD_NET}" = "xno" ]; then
	BUILD_IMAGE_OPS="$BUILD_IMAGE_OPS -d network"
fi
if [ "x${BUILD_STO}" = "xno" ]; then
	BUILD_IMAGE_OPS="$BUILD_IMAGE_OPS -d storage"
	if [ "x${BUILD_STO_DEV}" != "x" ]; then
		export BUILD_STO_DEV
	fi
fi
if [ "x${FORCE_REBUILD}" = "xyes" ]; then
	BUILD_IMAGE_OPS="$BUILD_IMAGE_OPS -r"
fi
# Simple environments
if [ "x${SPACET_S2C_SPEEDUP}" = "xno" ]; then
	export SPACET_S2C_SPEEDUP
fi
if [ "x${SPACET_CPU}" != "x" ]; then
	export SPACET_CPU
fi
if [ "x${SPACET_DDR}" != "x" ]; then
	export SPACET_DDR
fi

# Control LITMUS partial builds
if [ "x${LITMUS_SUIT}" = "xHAND" ]; then
	LITMUS_CASE_START="LB_2B_amoadd_2D_data_2D_amoadd_2E_rl_2B_amoadd_2E_aq_2D_data_2D_amoadd"
	LITMUS_CASE_STOP="SB_2B_rfi_2D_addrs"
fi
if [ "x${LITMUS_SUIT}" = "xATOMICS/CO" ]; then
	LITMUS_CASE_START="RWC_2B_posxp_2B_pos"
	LITMUS_CASE_STOP="WRC_2B_fence_2E_rw_2E_rws_2B_fence_2E_rw_2E_rwsxx"
fi

export LITMUS_CASE_START
export LITMUS_CASE_STOP

BUILD_BIN=${BUILD_ROOT}/obj/bench/usr/local/bin
BUILD_INIT=${BUILD_ROOT}/obj/bench/etc/init.d

# Start the build
if [ ! -z ${BBLVAR} ]; then
	cp -f ${SDFIRM_ROOT}/arch/riscv/configs/${MACH}_bbl${BBLVAR}_defconfig \
	      ${SDFIRM_ROOT}/arch/riscv/configs/${MACH}_bbl_defconfig
fi
rm -rf ${BUILD_ROOT}/obj/bench/*
mkdir -p ${BUILD_BIN}
mkdir -p ${BUILD_INIT}
mkdir -p ${BACKUP_ROOT}

build_cpu2006()
{
	if [ "x${CPU2006_UPDATE}" = "xyes" ]; then
		if [ -z ${CPU2006_BENCHMARKS} ]; then
			BUILD_IMAGE_OPS="${BUILD_IMAGE_OPS} -b cint -b cfp"
		else
			benches=
			for b in ${CPU2006_BENCHMARKS}; do
				BUILD_IMAGE_OPS="${BUILD_IMAGE_OPS} -b ${b}"
			done
		fi
		if [ -z ${CPU2006_DATA} ]; then
			BUILD_IMAGE_OPS="${BUILD_IMAGE_OPS} -i ${CPU2006_DATA}"
		fi
	fi
}

build_litmus()
{
	if [ "x${LITMUS_UPDATE}" = "xyes" ]; then
		LITMUS_UPDATE_START=no
		export LITMUS=${LITMUS_CORES}
		LITMUS_DIR=${LITMUS_ROOT}/litmus-${LITMUS}cores
		LITMUS_RUN=${LITMUS_DIR}/run.c
		cp -f ${SDFIRM_ROOT}/tests/litmus_dump.c \
		      ${LITMUS_DIR}/litmus_dump.c
		rm -f ${LITMUS_DIR}/utils.o
		if [ "x${LITMUS_CASES}" = "x" ]; then
			LITMUS_CASES=`\
				cat ${LITMUS_DIR}/run_all.c | \
				grep extern | \
				cut -d' ' -f3 | \
				cut -d'(' -f1`
		fi

		# Generating run.c
		echo -n "" > ${LITMUS_RUN}
		echo "#include <stdio.h>" >> ${LITMUS_RUN}
		echo "#include <stdlib.h>" >> ${LITMUS_RUN}
		echo "#include <time.h>" >> ${LITMUS_RUN}
		echo "" >> ${LITMUS_RUN}
		if [ "x${LITMUS_DUMP}" = "xyes" ]; then
			echo "extern int dump_init(void);" >> ${LITMUS_RUN}
			echo "extern void dump_exit(void);" >> ${LITMUS_RUN}
		fi
		if [ "x${LITMUS_CASE_START}" != "x" ]; then
			LITMUS_UPDATE_START=no
		else
			LITMUS_UPDATE_START=yes
		fi
		for c in ${LITMUS_CASES}; do
			echo "Generating litmus ${c}..."
			if [ "x${LITMUS_UPDATE_START}" = "xno" ]; then
				if [ "x${c}" = "x${LITMUS_CASE_START}" ]; then
					LITMUS_UPDATE_START=yes
				fi
			fi
			if [ "x${LITMUS_UPDATE_START}" = "xyes" ]; then
				echo "extern int ${c}(int argc,char **argv,FILE *out);" >> ${LITMUS_RUN}
				if [ "x${LITMUS_UPDATE_STOP}" = "x${c}" ]; then
					LITMUS_UPDATE_START=no
					break
				fi
			fi
		done
		echo "static void my_date(FILE *out) {" >> ${LITMUS_RUN}
		echo "  time_t t = time(NULL);" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\",ctime(&t));" >> ${LITMUS_RUN}
		echo "}" >> ${LITMUS_RUN}
		echo "static void end_report(int argc,char **argv,FILE *out) {" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"Revision 6773cdcd07d2f1f9413a9cd2a38c74f552fe0faa, version 7.56+01(dev)\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"Command line: litmus7 -mach ./riscv.cfg -avail 4 -excl gcc.excl -excl instructions.excl -o hw-tests-src tests/non-mixed-size/@all\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"Parameters\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"#define SIZE_OF_TEST 100\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"#define NUMBER_OF_RUN 10\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"#define AVAIL 4\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"#define STRIDE 1\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"#define MAX_LOOP 0\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"/* gcc options: -D_GNU_SOURCE -DFORCE_AFFINITY -Wall -std=gnu99 -O2 -pthread */\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"/* gcc link options: -static */\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"/* barrier: userfence */\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"/* launch: changing */\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"/* affinity: incr1 */\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"/* alloc: dynamic */\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"/* memory: direct */\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"/* stride: 1 */\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"/* safer: write */\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"/* preload: random */\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"/* speedcheck: no */\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"%s\\n\",\"/* proc used: ${LITMUS} */\");" >> ${LITMUS_RUN}
		echo "  fprintf(out,\"Command:\");" >> ${LITMUS_RUN}
		echo "  for ( ; *argv ; argv++) {" >> ${LITMUS_RUN}
		echo "    fprintf(out,\" %s\",*argv);" >> ${LITMUS_RUN}
		echo "  }" >> ${LITMUS_RUN}
		echo "  putc('\\n',out);" >> ${LITMUS_RUN}
		echo "}" >> ${LITMUS_RUN}
		echo "static void run(int argc,char **argv,FILE *out) {" >> ${LITMUS_RUN}
		echo "  my_date(out);" >> ${LITMUS_RUN}
		if [ "x${LITMUS_CASE_START}" != "x" ]; then
			LITMUS_UPDATE_START=no
		else
			LITMUS_UPDATE_START=yes
		fi
		for c in ${LITMUS_CASES}; do
			if [ "x${LITMUS_UPDATE_START}" = "xno" ]; then
				if [ "x${c}" = "x${LITMUS_CASE_START}" ]; then
					LITMUS_UPDATE_START=yes
				fi
			fi
			if [ "x${LITMUS_UPDATE_START}" = "xyes" ]; then
				echo "  ${c}(argc,argv,out);" >> ${LITMUS_RUN}
				if [ "x${LITMUS_UPDATE_STOP}" = "x${c}" ]; then
					LITMUS_UPDATE_START=no
					break
				fi
			fi
		done
		echo "  end_report(argc,argv,out);" >> ${LITMUS_RUN}
		echo "  my_date(out);" >> ${LITMUS_RUN}
		echo "}" >> ${LITMUS_RUN}
		echo "int main(int argc,char **argv) {" >> ${LITMUS_RUN}
		if [ "x${LITMUS_DUMP}" = "xyes" ]; then
			echo "  dump_init();" >> ${LITMUS_RUN}
		fi
		echo "  run(argc,argv,stdout);" >> ${LITMUS_RUN}
		if [ "x${LITMUS_DUMP}" = "xyes" ]; then
			echo "  dump_exit();" >> ${LITMUS_RUN}
		fi
		echo "  return 0;" >> ${LITMUS_RUN}
		echo "}" >> ${LITMUS_RUN}

		BUILD_IMAGE_OPS="${BUILD_IMAGE_OPS} -c ${LITMUS_CORES}"
	else
		# Just copy pre-built litmus image
		cp -f ${BACKUP_ROOT}/litmus${LITMUS_CORES} ${BUILD_BIN}/litmus
		${CROSS_COMPILE}strip ${BUILD_BIN}/litmus
	fi
	# Will be updated by build_litmus.sh
	# cp -f ${BACKUP_ROOT}/litmus.sh ${BUILD_BIN}/litmus.sh
}

build_perf()
{
	if [ "x${PERF_UPDATE}" = "xyes" ]; then
		PERF_ROOT=${LINUX_ROOT}/tools/perf
		(
			cd ${PERF_ROOT}
			ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE make clean
			ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE make NO_LIBELF=1 NO_LIBTRACEEVENT=1
			${CROSS_COMPILE}strip -R .comment -R .note \
				${PERF_ROOT}/perf -o ${BUILD_BIN}/perf
		)
	fi
}

build_test()
{
	EARLY_TEST=${BUILD_INIT}/test_early
	LATE_TEST=${BUILD_INIT}/test_late

	if [ "x${TEST_EARLY}" = "xbenchmark" ]; then
		echo "#!/bin/sh" > ${EARLY_TEST}
		echo "dhrystone 50" >> ${EARLY_TEST}
		echo "linpack" >> ${EARLY_TEST}
		echo "dhrystone 200000000" >> ${EARLY_TEST}
		echo "coremark" >> ${EARLY_TEST}
		echo "stream" >> ${EARLY_TEST}
	fi
	if [ "x${TEST_EARLY}" = "xcpu2006" ]; then
		build_cpu2006
		echo "#!/bin/sh" > ${EARLY_TEST}
		echo "ARCH=${ARCH}" >> ${EARLY_TEST}
		echo "CPU2006_ROOT=/opt/cpu2006" >> ${EARLY_TEST}
		echo "CPU2006_BENCHMARKS=\`ls \${CPU2006_ROOT}/benchspec/CPU2006\`" >> ${EARLY_TEST}
		echo "CPU2006_DATA=\"ref test train\"" >> ${EARLY_TEST}
		echo "if [ ! -f /usr/bin/perl ]; then" >> ${EARLY_TEST}
		echo "	if [ ! -d /usr/bin ]; then" >> ${EARLY_TEST}
		echo "		mkdir /usr/bin" >> ${EARLY_TEST}
		echo "	fi" >> ${EARLY_TEST}
		echo "	ln -s $CPU2006_ROOT/bin/specperl /usr/bin/perl" >> ${EARLY_TEST}
		echo "fi" >> ${EARLY_TEST}
		if [ "x${CPU2006_REPORT}" = "x" ]; then
			# stress test only
			cat ${SCRIPT}/cpu2006/run.sh >> ${EARLY_TEST}
		elif [ "x${CPU2006_REPORT}" = "xspecinvoke" ]; then
			# use specinvoke
			cat ${SCRIPT}/cpu2006/specinvoke.sh >> ${EARLY_TEST}
		elif [ "x${CPU2006_REPORT}" = "xrunspec" ]; then
			# use runspec
			cat ${SCRIPT}/cpu2006/runspec.sh >> ${EARLY_TEST}
		else
			echo "exit 0" >> ${EARLY_TEST}
		fi
	fi
	if [ "x${TEST_EARLY}" = "xlitmus" ]; then
		build_litmus
		echo "#!/bin/sh" > ${EARLY_TEST}
		echo "echo \"Running test 0 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 1 -s 5k -r 20" >> ${EARLY_TEST}
		echo "echo \"Running test 1 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 1 -s 50k -r 200" >> ${EARLY_TEST}
		echo "echo \"Running test 2 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 1 -s 500k -r 20" >> ${EARLY_TEST}
		echo "echo \"Running test 3 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 1 -s 10k -r 1k" >> ${EARLY_TEST}
		echo "echo \"Running test 4 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 1 -s 100k -r 100" >> ${EARLY_TEST}
		echo "echo \"Running test 5 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 1 -s 5k -r 2k" >> ${EARLY_TEST}
		echo "echo \"Running test 6 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 1 -s 1M -r 10" >> ${EARLY_TEST}
		echo "echo \"Running test 7 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 2 -s 50k -r 200" >> ${EARLY_TEST}
		echo "echo \"Running test 8 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 2 -s 500k -r 20" >> ${EARLY_TEST}
		echo "echo \"Running test 9 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 2 -s 10k -r 1k" >> ${EARLY_TEST}
		echo "echo \"Running test 10 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 2 -s 100k -r 100" >> ${EARLY_TEST}
		echo "echo \"Running test 11 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 2 -s 5k -r 2k" >> ${EARLY_TEST}
		echo "echo \"Running test 12 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 2 -s 1M -r 10" >> ${EARLY_TEST}
		echo "echo \"Running test 13 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 3 -s 50k -r 200" >> ${EARLY_TEST}
		echo "echo \"Running test 14 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 3 -s 500k -r 20" >> ${EARLY_TEST}
		echo "echo \"Running test 15 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 3 -s 10k -r 1k" >> ${EARLY_TEST}
		echo "echo \"Running test 16 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 3 -s 100k -r 100" >> ${EARLY_TEST}
		echo "echo \"Running test 17 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 3 -s 5k -r 2k" >> ${EARLY_TEST}
		echo "echo \"Running test 18 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 3 -s 1M -r 10" >> ${EARLY_TEST}
		echo "echo \"Running test 19 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 4 -s 50k -r 200" >> ${EARLY_TEST}
		echo "echo \"Running test 20 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 4 -s 500k -r 20" >> ${EARLY_TEST}
		echo "echo \"Running test 21 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 4 -s 10k -r 1k" >> ${EARLY_TEST}
		echo "echo \"Running test 22 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 4 -s 100k -r 100" >> ${EARLY_TEST}
		echo "echo \"Running test 23 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 4 -s 5k -r 2k" >> ${EARLY_TEST}
		echo "echo \"Running test 24 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 4 -s 1M -r 10" >> ${EARLY_TEST}
		echo "echo \"Running test 25 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 5 -s 50k -r 200" >> ${EARLY_TEST}
		echo "echo \"Running test 26 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 5 -s 500k -r 20" >> ${EARLY_TEST}
		echo "echo \"Running test 27 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 5 -s 10k -r 1k" >> ${EARLY_TEST}
		echo "echo \"Running test 28 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 5 -s 100k -r 100" >> ${EARLY_TEST}
		echo "echo \"Running test 29 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 5 -s 5k -r 2k" >> ${EARLY_TEST}
		echo "echo \"Running test 30 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 5 -s 1M -r 10" >> ${EARLY_TEST}
		echo "echo \"Running test 31 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 6 -s 50k -r 200" >> ${EARLY_TEST}
		echo "echo \"Running test 32 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 6 -s 500k -r 20" >> ${EARLY_TEST}
		echo "echo \"Running test 33 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 6 -s 10k -r 1k" >> ${EARLY_TEST}
		echo "echo \"Running test 34 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 6 -s 100k -r 100" >> ${EARLY_TEST}
		echo "echo \"Running test 35 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 6 -s 5k -r 2k" >> ${EARLY_TEST}
		echo "echo \"Running test 36 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 6 -s 1M -r 10" >> ${EARLY_TEST}
		echo "echo \"Running test 37 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 7 -s 50k -r 200" >> ${EARLY_TEST}
		echo "echo \"Running test 38 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 7 -s 500k -r 20" >> ${EARLY_TEST}
		echo "echo \"Running test 39 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 7 -s 10k -r 1k" >> ${EARLY_TEST}
		echo "echo \"Running test 40 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 7 -s 100k -r 100" >> ${EARLY_TEST}
		echo "echo \"Running test 41 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 7 -s 5k -r 2k" >> ${EARLY_TEST}
		echo "echo \"Running test 42 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 7 -s 1M -r 10" >> ${EARLY_TEST}
		echo "echo \"Running test 43 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 8 -s 50k -r 200" >> ${EARLY_TEST}
		echo "echo \"Running test 44 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 8 -s 500k -r 20" >> ${EARLY_TEST}
		echo "echo \"Running test 45 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 8 -s 10k -r 1k" >> ${EARLY_TEST}
		echo "echo \"Running test 46 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 8 -s 100k -r 100" >> ${EARLY_TEST}
		echo "echo \"Running test 47 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 8 -s 5k -r 2k" >> ${EARLY_TEST}
		echo "echo \"Running test 48 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 8 -s 1M -r 10" >> ${EARLY_TEST}
		echo "echo \"Running test 49 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 31 -s 50k -r 200" >> ${EARLY_TEST}
		echo "echo \"Running test 50 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 31 -s 500k -r 20" >> ${EARLY_TEST}
		echo "echo \"Running test 51 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 31 -s 10k -r 1k" >> ${EARLY_TEST}
		echo "echo \"Running test 52 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 31 -s 100k -r 100" >> ${EARLY_TEST}
		echo "echo \"Running test 53 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 31 -s 5k -r 2k" >> ${EARLY_TEST}
		echo "echo \"Running test 54 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 31 -s 1M -r 10" >> ${EARLY_TEST}
		echo "echo \"Running test 55 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 133 -s 50k -r 200" >> ${EARLY_TEST}
		echo "echo \"Running test 56 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 133 -s 500k -r 20" >> ${EARLY_TEST}
		echo "echo \"Running test 57 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 133 -s 10k -r 1k" >> ${EARLY_TEST}
		echo "echo \"Running test 58 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 133 -s 100k -r 100" >> ${EARLY_TEST}
		echo "echo \"Running test 59 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 133 -s 5k -r 2k" >> ${EARLY_TEST}
		echo "echo \"Running test 60 out of 60\"" >> ${EARLY_TEST}
		echo "litmus -st 133 -s 1M -r 10" >> ${EARLY_TEST}
		echo "echo \"Done\"" >> ${EARLY_TEST}
	fi
	if [ "x${TEST_LATE}" = "xkvm" ]; then
		echo "#!/bin/sh" > ${LATE_TEST}
		echo "modprobe kvm" >> ${LATE_TEST}
		echo "cd /usr/local/bin"  >> ${LATE_TEST}
		if [ "x${BUILD_HTIF}" = "xyes" ]; then
			# TODO: dynamic lkvm support
			echo "lkvm run -m 128 -c1 --console virtio -p \"console=hvc0 earlycon=sbi rdinit=/sdfirm_init\" -k ./Image" >> ${LATE_TEST}
		else
			echo "lkvm run -m 128 -c1 --console serial -p \"console=ttyS0 earlycon=sbi rdinit=/sdfirm_init\" -k ./Image" >> ${LATE_TEST}
		fi
	fi
	if [ "x${TEST_LATE}" = "xdmatest" ]; then
		echo "#!/bin/sh" > ${LATE_TEST}
		echo "dmatest.sh -w -i 2 -b 4096 -v test dma0chan0" >> ${LATE_TEST}
		echo "dmatest.sh -w -i 2 -b 2097152 -v test dma0chan0" >> ${LATE_TEST}
		echo "dmatest.sh -w -i 20 -b 4096 -v test dma0chan0" >> ${LATE_TEST}
		echo "dmatest.sh -w -i 20 -b 2097152 -v test dma0chan0" >> ${LATE_TEST}
		echo "dmatest.sh -w -i 200 -b 4096 -v test dma0chan0" >> ${LATE_TEST}
		echo "dmatest.sh -w -i 200 -b 2097152 -v test dma0chan0" >> ${LATE_TEST}
		echo "dmatest.sh -w -i 2000 -b 4096 -v test dma0chan0" >> ${LATE_TEST}
		echo "dmatest.sh -w -i 2000 -b 2097152 -v test dma0chan0" >> ${LATE_TEST}
		echo "dmatest.sh -w -i 20000 -b 4096 -v test dma0chan0" >> ${LATE_TEST}
		echo "dmatest.sh -w -i 20000 -b 2097152 -v test dma0chan0" >> ${LATE_TEST}
	fi
	build_perf
	if [ -e ${EARLY_TEST} ]; then
		chmod +x ${EARLY_TEST}
	fi
	if [ -e ${LATE_TEST} ]; then
		chmod +x ${LATE_TEST}
	fi
}

build_customer()
{
	if [ "x${BUILD_CUSTOMER}" != "x" ]; then
		CUSTOMER_PATH=$HOME/upload/${BUILD_CUSTOMER}/rootfs
		if [ -d ${CUSTOMER_PATH} ]; then
			echo "Copying ${CUSTOMER_PATH}..."
			cp -arf ${CUSTOMER_PATH}/* ${BUILD_ROOT}/obj/bench/
		fi
	fi
}

build_image()
{
	(
		if [ "x${LITMUS_DUMP}" = "xyes" ]; then
			export USE_DUMP_MSG=1
		fi
		cd ${BUILD_ROOT}
		${SCRIPT}/build_image.sh ${BUILD_IMAGE_OPS} build
	)
}

backup_elfs()
{
	# Backup ELF files for debugging
	echo "Backing up ${BACKUP_ROOT}..."
	if [ -f ${BUILD_ROOT}/obj/linux-${ARCH}/vmlinux ]; then
		echo "Copying linux images..."
		cp -f ${BUILD_ROOT}/obj/linux-${ARCH}/vmlinux \
			${BACKUP_ROOT}/vmlinux
		cp -f ${BUILD_ROOT}/obj/linux-${ARCH}/arch/${ARCH}/boot/Image \
			${BACKUP_ROOT}/Image
		echo "Copying busybox images..."
		cp -f ${BUILD_ROOT}/obj/busybox-${ARCH}/busybox \
			${BACKUP_ROOT}/busybox
		echo "Copying sdfirm..."
		cp -f ${BUILD_ROOT}/obj/sdfirm-${ARCH}/sdfirm \
			${BACKUP_ROOT}/sdfirm
		echo "Copying sdfirm.rom..."
		cp -f ${BUILD_ROOT}/obj/sdfirm-${ARCH}/arch/${ARCH}/boot/sdfirm.rom \
			${BACKUP_ROOT}/sdfirm.rom
		if [ -z ${BUILD_CUSTOMER} ]; then
			if [ "x${TEST_EARLY}" = "xbenchmark" ]; then
				echo "Copying benchmarks..."
				cp -f ${SDFIRM_ROOT}/tests/bench/dhrystone.debug \
					${BACKUP_ROOT}/dhrystone
				cp -f ${SDFIRM_ROOT}/tests/bench/linpack.debug \
					${BACKUP_ROOT}/linpack
				cp -f ${SDFIRM_ROOT}/tests/bench/coremark.debug \
					${BACKUP_ROOT}/coremark
				cp -f ${SDFIRM_ROOT}/tests/bench/loopback.debug \
					${BACKUP_ROOT}/loopback
			fi
			if [ "x${TEST_EARLY}" = "xlitmus" ]; then
				echo "Copying litmus tests..."
				cp -f ${LITMUS_ROOT}/litmus-${LITMUS_CORES}cores/run.exe \
					${BACKUP_ROOT}/litmus
				cp -f ${LITMUS_ROOT}/litmus-${LITMUS_CORES}cores/run.exe \
					${BACKUP_ROOT}/litmus${LITMUS_CORES}
			fi
			if [ "x${TEST_LATE}" = "xperf" ]; then
				echo "Copying linux kernel programs..."
				cp -f ${PERF_ROOT}/perf \
					${BACKUP_ROOT}/perf
			fi
			if [ "x${TEST_EARLY}" = "xcpu2006" ]; then
				echo "Copying cpu2006 tests..."
				EXE_FILES=("/opt/cpu2006/benchspec/CPU2006/401.bzip2/exe/bzip2_base.riscv"
				            "..."
					   )
				for str in "${EXE_FILES[@]}"
				do
					if test -f "$str"; then
						cp -f ${str} ${BACKUP_ROOT}/
					fi
				done
			fi
		fi
	fi
}

if [ -z ${BUILD_CUSTOMER} ]; then
	echo "Building sdfirm..."
	build_test
else
	echo "Building ${BUILD_CUSTOMER}..."
	build_customer
fi
build_image
backup_elfs
