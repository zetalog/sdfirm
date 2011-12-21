#!/bin/sh

get_cwd()
{
	local bindir=`dirname $1`
	export SCRIPTS_ROOT=`(cd $bindir; pwd)`
}

get_cwd $0

(
	export LD_LIBRARY_PATH=$SCRIPTS_ROOT/cmn:$LD_LIBRARY_PATH
	export LD_LIBRARY_PATH=$SCRIPTS_ROOT/libusb:$LD_LIBRARY_PATH
	ldconfig
	cd $SCRIPTS_ROOT/usbtest
	./usbtest
)

