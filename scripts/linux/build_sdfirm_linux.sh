#!/bin/bash
#
# This scripts build sdfirm bootable Linux image with early/late test
# specified.

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
	BUILD_ROOT=$HOME/workspace/Software
fi
if [ -z $BACKUP_ROOT ]; then
	BACKUP_ROOT=$HOME/workspace/Hardware
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

if [ -z $SDFIRM_ROOT ]; then
	SDFIRM_ROOT=${BUILD_ROOT}/sdfirm
fi
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
if [ "x${BUILD_NET}" = "xno" ]; then
	BUILD_IMAGE_OPS="$BUILD_IMAGE_OPS -d network"
fi
if [ "x${BUILD_STO}" = "xyes" ]; then
	BUILD_IMAGE_OPS="$BUILD_IMAGE_OPS -e storage"
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
			make clean
			make
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
		echo "echo -e \$PATH" >> ${EARLY_TEST}
		echo "ls /usr/local/bin" >> ${EARLY_TEST}
		echo "dhrystone 50" >> ${EARLY_TEST}
		echo "linpack" >> ${EARLY_TEST}
		echo "dhrystone 200000000" >> ${EARLY_TEST}
	fi
	if [ "x${TEST_EARLY}" = "xlitmus" ]; then
		build_litmus
		echo "#!/bin/sh" > ${EARLY_TEST}
		echo "litmus -st 1 -s 4k -r 20" >> ${EARLY_TEST}
		echo "litmus -st 1 -s 2M -r 20" >> ${EARLY_TEST}
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
	if [ "x${TEST_LATE}" = "xperf" ]; then
		build_perf
	fi
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
	echo
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
