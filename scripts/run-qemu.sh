#!/bin/sh

SCRIPT=`(cd \`dirname $0\`; pwd)`

ARCH=riscv64
QEMU=qemu-system-${ARCH}

${QEMU} -nographic -machine virt -bios ${SCRIPT}/../sdfirm
