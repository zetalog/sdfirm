#!/bin/sh

export SYSTEM=
export TARGET=arm-eabi
export PREFIX=/usr/local/$TARGET
export GMP_VER=4.3.2
export MPFR_VER=2.4.2
export MPC_VER=0.8.2
export BIN_VER=2.21.1
export GCC_VER=4.5.1
export LIB_VER=1.18.0
export GDB_VER=7.2
export FLOAT=soft

export TOPDIR=`pwd`
export BLDDIR=$TOPDIR/.build/$TARGET
export PATH=$PATH:$BLDDIR/static/bin

export GMP_DIR=gmp-$GMP_VER
export MPFR_DIR=mpfr-$MPFR_VER
export MPC_DIR=mpc-$MPC_VER
export BIN_DIR=binutils-$BIN_VER
export GCC_DIR=gcc-$GCC_VER
export LIB_DIR=newlib-$LIB_VER
export GDB_DIR=gdb-$GDB_VER

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
		--disable-interwork \
		--with-float=$FLOAT \
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
	mkdir -p $BLDDIR/interm-$GCC_DIR
	mkdir -p $BLDDIR/static
	cd $BLDDIR/interm-$GCC_DIR
	../../../$GCC_DIR/configure \
		--target=$TARGET \
		--prefix=$BLDDIR/static \
		--with-local-prefix=$PREFIX/$TARGET/sys-root \
		--with-sysroot=$PREFIX/$TARGET/sys-root \
		--with-gmp=$PREFIX \
		--with-mpfr=$PREFIX \
		--with-mpc=$PREFIX \
		--disable-nls \
		--disable-threads \
		--disable-shared \
		--disable-multilib \
		--disable-interwork \
		--with-float=$FLOAT \
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
		--target=$TARGET \
		--prefix=$PREFIX \
		--disable-newlib-io-c99-formats \
		--disable-newlib-io-long-long \
		--disable-newlib-io-float \
		--disable-newlib-io-long-double
	make
	make install
)

(
	mkdir -p $BLDDIR/$GCC_DIR
	cd $BLDDIR/$GCC_DIR
	../../../$GCC_DIR/configure \
		--target=$TARGET \
		--prefix=$PREFIX \
		--with-local-prefix=$PREFIX/$TARGET/sys-root \
		--with-sysroot=$PREFIX/$TARGET/sys-root \
		--with-gmp=$PREFIX \
		--with-mpfr=$PREFIX \
		--with-mpc=$PREFIX \
		--disable-nls \
		--disable-threads \
		--disable-shared \
		--disable-multilib \
		--disable-interwork \
		--with-newlib \
		--with-float=$FLOAT \
		--disable-libada \
		--disable-libssp \
		--disable-libmudflap \
		--enable-target-optspace \
		--enable-languages=c
	make
	make install
)

(
	mkdir -p $BLDDIR/$GDB_DIR
	cd $BLDDIR/$GDB_DIR
	../../../$GDB_DIR/configure \
		--target=$TARGET \
		--prefix=$PREFIX \
		--disable-nls
	make
	make install
)

