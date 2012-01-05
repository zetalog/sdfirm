#!/bin/sh

export SYSTEM=
export TARGET=avr
export PREFIX=/usr/local/$TARGET
export PATH=$PATH:$PREFIX/bin
export GMP_VER=4.3.2
export MPFR_VER=2.4.2
export MPC_VER=0.8.2
export BIN_VER=2.21
export GCC_VER=4.5.2
export LIB_VER=1.7.1
export DUDE_VER=5.11
export GDB_VER=7.3.1
export SIM_VER=0.1.2.7
export RICE_VER=2.12
export FLOAT=soft

export TOPDIR=`pwd`
export BLDDIR=$TOPDIR/.build/$TARGET
mkdir -p $BLDDIR
cd $BLDDIR

export GMP_DIR=gmp-$GMP_VER
export MPFR_DIR=mpfr-$MPFR_VER
export MPC_DIR=mpc-$MPC_VER
export BIN_DIR=binutils-$BIN_VER
export GCC_DIR=gcc-$GCC_VER
export LIB_DIR=avr-libc-$LIB_VER
export DUDE_DIR=avrdude-$DUDE_VER
export GDB_DIR=gdb-$GDB_VER
export SIM_DIR=simulavr-$SIM_VER
export RICE_DIR=avarice-$RICE_VER

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
	mkdir -p $BLDDIR/$GMP_DIR
	cd $BLDDIR/$GMP_DIR
	../../../$GMP_DIR/configure \
		--prefix=$PREFIX
	make
	make install
)

(
	mkdir -p $BLDDIR/$MPFR_DIR
	cd $BLDDIR/$MPFR_DIR
	../../../$MPFR_DIR/configure \
		--prefix=$PREFIX \
		--with-gmp=$PREFIX
	make
	make install
)

(
	mkdir -p $BLDDIR/$MPC_DIR
	cd $BLDDIR/$MPC_DIR
	../../../$MPC_DIR/configure \
		--prefix=$PREFIX \
		--with-gmp=$PREFIX \
		--with-mpfr=$PREFIX
	make
	make install
)

(
	mkdir -p $BLDDIR/$GCC_DIR
	cd $BLDDIR/$GCC_DIR
	../../../$GCC_DIR/configure \
		--target=$TARGET \
		--prefix=$PREFIX \
		--with-gmp=$PREFIX \
		--with-mpfr=$PREFIX \
		--with-mpc=$PREFIX \
		--with-dwarf2 \
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
		--host=$TARGET \
		--build=`../../../$LIB_DIR/config.guess`
	make
	make install
)

(
	mkdir -p $BLDDIR/$DUDE_DIR
	cd $BLDDIR/$DUDE_DIR
	../../../$DUDE_DIR/configure \
		--prefix=$PREFIX
	make
	make install
)

(
	mkdir -p $BLDDIR/$GDB_DIR
	cd $BLDDIR/$GDB_DIR
	../../../$GDB_DIR/configure \
		--prefix=$PREFIX \
		--target=$TARGET
	make
	make install
)

(
	mkdir -p $BLDDIR/$SIM_DIR
	cd $BLDDIR/$SIM_DIR
	rm -f ../../../$RICE_DIR/src/disp-vcd/config_scanner.c
	../../../$SIM_DIR/configure \
		--prefix=$PREFIX
	make
	make install
)

(
	CPPFLAGS=-I/usr/include/
	LDFLAGS=-L/usr/lib/
	mkdir -p $BLDDIR/$RICE_DIR
	cd $BLDDIR/$RICE_DIR
	../../../make distclean
	../../../$RICE_DIR/configure \
		--prefix=$PREFIX
	make
	make install
)
