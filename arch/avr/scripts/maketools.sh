#!/bin/sh

export SYSTEM=
export TARGET=avr
export PREFIX=/usr/local/$TARGET
export BIN_VER=2.21
export GCC_VER=4.5.1
export LIB_VER=1.7.0
export FLOAT=soft

export TOPDIR=`pwd`
export BLDDIR=$TOPDIR/.build/$TARGET
export PATH=$PATH:$BLDDIR/static/bin
mkdir -p $BLDDIR
cd $BLDDIR

export BIN_DIR=binutils-$BIN_VER
export GCC_DIR=gcc-$GCC_VER
export LIB_DIR=avr-libc-$LIB_VER

(
	mkdir -p $BLDDIR/$BIN_DIR
	cd $BLDDIR/$BIN_DIR
	../../../$BIN_DIR/configure \
		--target=$TARGET \
		--prefix=$PREFIX \
		--disable-nls \
		--disable-threads \
		--disable-shared \
		--disable-multilib \
		--enable-target-optspace \
		--disable-libada \
		--disable-libssp
	make
	make install
)

(
	mkdir -p $BLDDIR/$GCC_DIR
	cd $BLDDIR/$GCC_DIR
	../../../$GCC_DIR/configure \
		--target=$TARGET \
		--prefix=$PREFIX \
		--disable-nls \
		--disable-threads \
		--disable-shared \
		--disable-multilib \
		--disable-libada \
		--disable-libssp \
		--disable-libmudflap \
		--enable-target-optspace \
		--enable-languages=c
	make
	make install
)

(
	mkdir -p $BLDDIR/$LIB_DIR
	cd $BLDDIR/$LIB_DIR
	../../../$LIB_DIR/configure \
		--prefix=$PREFIX \
		--host=$TARGET
	make
	make install
)
