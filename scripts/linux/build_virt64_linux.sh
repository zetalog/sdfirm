#!/bin/bash

ARCH=riscv
MACH=virt64
BBLVAR=
CROSS_COMPILE=riscv64-linux-gnu-

BUILD_ROOT=$HOME/workspace
BUILD_CUSTOMER=
BUILD_TINY=no
BUILD_LIB=no
BUILD_SMP=yes
BUILD_NET=no
BUILD_STO=yes
BUILD_STO_DEV=vda

LITMUS_CORES=4
LITMUS_UPDATE=yes
LITMUS_DUMP=no
LITMUS_ROOT=$BUILD_ROOT/memory-model/litmus-tests-riscv

PERF_UPDATE=yes

# Early tests include benchmark, litmus
TEST_EARLY=
TEST_LATE=

. ${SDFIRM_ROOT}/scripts/linux/build_sdfirm_linux.sh
