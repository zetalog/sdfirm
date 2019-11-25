#!/bin/sh

SCRIPT=`(cd \`dirname $0\`; pwd)`
SDFIRM=`(cd $SCRIPT/../../..; pwd)`

if [ "x${IMAGE}" = "x" ]; then
	IMAGE=${SDFIRM}/arch/arm64/boot/sdfirm.bin
fi

sudo dd if=$IMAGE of=$1 bs=1024 seek=8
