#!/bin/bash
#
# Build rz sz images.

TOP=`pwd`
SCRIPT=`(cd \`dirname $0\`; pwd)`
SRCDIR=`(cd ${SCRIPT}/..; pwd)`
WORKING_DIR=`(cd ${SCRIPT}/../../..; pwd)`

APPDIR=${TOP}/obj/bench/usr/local/bin
mkdir -p ${APPDIR}

GCC=`which ${CROSS_COMPILE}gcc`
BASENAME=`basename ${GCC}`
TARGET=${BASENAME%%-gcc}

mkdir -p ${SDFIRM_DIR}/tests/lrzsz/build/
cd ${SDFIRM_DIR}/tests/lrzsz/build/
../configure --build=x86_64-linux --host=$TARGET --target=$TARGET --prefix=/usr --exec_prefix=/usr --bindir=/usr/bin --sbindir=/usr/sbin --libexecdir=/usr/libexec --datadir=/usr/share --sysconfdir=/etc --sharedstatedir=/com --localstatedir=/var --libdir=/usr/lib --includedir=/usr/include --oldincludedir=/usr/include --infodir=/usr/share/info --mandir=/usr/share/man --disable-silent-rules --disable-dependency-tracking --enable-nls LDFLAGS="-static"
make -j6
cp -f src/lsz ${APPDIR}/sz
cp -f src/lrz ${APPDIR}/rz
cd -
