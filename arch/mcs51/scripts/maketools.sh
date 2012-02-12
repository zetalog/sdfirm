#!/bin/sh

export SYSTEM=
export TARGET=mcs51
export PREFIX=/usr/local/sdcc

export SDCC_VER=3.1.0
export SDCC_DIR=sdcc

export TOPDIR=`pwd`
export BLDDIR=$TOPDIR/.build/sdcc
export PATH=$PATH:$BLDDIR/static/bin

(
	mkdir -p $BLDDIR
	cd $BLDDIR
	../../$SDCC_DIR/configure	\
		--prefix=$PREFIX	\
		--disable-gbz80-port	\
		--disable-z80-port	\
		--disable-avr-port	\
		--disable-ds390-port	\
		--disable-ds400-port	\
		--disable-hc08-port	\
		--disable-pic14-port	\
		--disable-pic16-port	\
		--disable-xa51-port	\
		--disable-ucsim		\
		2>&1 | tee configure.log
	make 2>&1 | tee make.log
	make install
)
