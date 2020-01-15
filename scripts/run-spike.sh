#!/bin/sh

SCRIPT=`(cd \`dirname $0\`; pwd)`

spike --rbb-port=9824 -m0x10000000:0x20000 ${SCRIPT}/../sdfirm
