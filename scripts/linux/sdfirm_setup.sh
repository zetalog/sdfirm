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

# WORKING_DIR: where sdfirm scripts and build output exist
#              used by tests builds
# TOP: where other programs exist, used by progs builds
TOP=`pwd`
SCRIPT=`(cd ${SDFIRM_ROOT}/scripts/linux; pwd)`
WORKING_DIR=`(cd ${SCRIPT}/../../..; pwd)`
export TOP
export WORKING_DIR

PATH=${SDFIRM_ROOT}/scripts:${SDFIRM_ROOT}/scripts/linux:${PATH}
export PATH

if [ -z $ARCH ]; then
	ARCH=riscv
fi
if [ -z $MACH ]; then
	MACH=spike64
	BBLVAR=
fi
if [ -z ${BBL} ]; then
	BBL=sdfirm
fi
if [ -z ${HOSTNAME} ]; then
	HOSTNAME=sdfirm
fi
if [ -z $CROSS_COMPILE ]; then
	CROSS_COMPILE=riscv64-unknown-linux-gnu-
fi
export ARCH
export MACH
export BBLVAR
export BBL
export HOSTNAME
export CROSS_COMPILE

if [ -z $FORCE_REBUILD ]; then
	FORCE_REBUILD=no
fi
export FORCE_REBUILD

. ${SCRIPT}/setup_tests.sh
. ${SCRIPT}/setup_progs.sh
