#!/bin/bash
#
# This scripts build sdfirm bootable Linux image with early/late test
# specified.

if [ -z $WORKING_DIR ]; then
	echo "WORKING_DIR environment must be specified!"
	echo "As this script is sourced by the invoker, you must specify"
	echo "the abstract path of this script via SDFIRM_ROOT"
	echo "environment."
	exit 1
fi
if [ -z $BUILD_ROOT ]; then
	BUILD_ROOT=$WORKING_DIR
fi
if [ -z $BACKUP_ROOT ]; then
	BACKUP_ROOT=$WORKING_DIR/backup
fi
export BUILD_ROOT
export BACKUP_ROOT

if [ -z ${SDFIRM_DIR} ]; then
	SDFIRM_DIR=${WORKING_DIR}/sdfirm
fi
if [ -z $LINUX_ROOT ]; then
	LINUX_ROOT=${BUILD_ROOT}/linux
fi
export SDFIRM_DIR
export LINUX_ROOT

if [ -z ${LITMUS_ROOT} ]; then
	LITMUS_ROOT=${WORKING_DIR}/litmus-tests-riscv
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
# Control LITMUS partial builds
if [ "x${LITMUS_SUIT}" = "xHAND" ]; then
	LITMUS_CASE_START="LB_2B_amoadd_2D_data_2D_amoadd_2E_rl_2B_amoadd_2E_aq_2D_data_2D_amoadd"
	LITMUS_CASE_STOP="SB_2B_rfi_2D_addrs"
fi
if [ "x${LITMUS_SUIT}" = "xATOMICS/CO" ]; then
	LITMUS_CASE_START="RWC_2B_posxp_2B_pos"
	LITMUS_CASE_STOP="WRC_2B_fence_2E_rw_2E_rws_2B_fence_2E_rw_2E_rwsxx"
fi
if [ "x${LITMUS_DUMP}" = "xyes" ]; then
	USE_DUMP_MSG=1
fi
export LITMUS_ROOT
export LITMUS_CORES
export LITMUS_UPDATE
export LITMUS_DUMP
export LITMUS_CASE_START
export LITMUS_CASE_STOP
export USE_DUMP_MSG

if [ -z ${CPU2006_ROOT} ]; then
	CPU2006_ROOT=${WORKING_DIR}/cpu2006
fi
if [ -z $CPU2006_UPDATE ]; then
	CPU2006_UPDATE=yes
fi
if [ -z $PERF_UPDATE ]; then
	PERF_UPDATE=yes
fi
export CPU2006_ROOT
export CPU2006_UPDATE
export PERF_UPDATE

# Early tests: litmus, benchmark
# Late tests: dmatest, perf, loopback
if [ -z $TEST_EARLY ]; then
	TEST_EARLY=
fi
if [ -z $TEST_LATE ]; then
	TEST_LATE=
fi
export TEST_EARLY
export TEST_LATE

# Control if customer's rootfs is specified
#BUILD_CUSTOMER=kuhan
export BUILD_CUSTOMER

. ${SCRIPT}/sdfirm_tests.sh
