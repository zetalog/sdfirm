#!/bin/sh

DMATEST_MODULE=/sys/module/dmatest
DMATEST_test_buf_size=16384
DMATEST_iterations=0
DMATEST_timeout=3000
DMATEST_noverify=N
DMATEST_norandom=N
DMATEST_verbose=N
DMATEST_alignment=-1
DMATEST_transfer_size=0
DMATEST_polled=N
DMATEST_waited=N

usage()
{
	echo "Usage:"
	echo "`basename $0` [-b buf_size] [-i iterations] [-t timeout]"
	echo "              [-d feature] [-v] [-a alignment] [-x xfer_size]"
	echo "              [-p] [-w]"
	echo "              action [channel]"
	echo "Where:"
	echo " Options:"
	echo " -b buf_size   Size of the memcpy test buffer"
	echo " -i iterations Iterations before stopping test"
	echo "    iterations number / infinite(0)"
	echo "               default: infinite"
	echo " -t timeout    Transfer timeout in msec"
	echo "    timeout    number / infinite(-1)"
	echo "               default: 3000"
	echo " -d feature    Disable test features"
	echo "               feature can be verify or random"
	echo " -d verify     Disable data verification"
	echo "               default: verify"
	echo " -d random     Disable random offset setup"
	echo "               default: random"
	echo " -v            Enable success result message"
	echo "               default: off"
	echo " -a alignment  Custom data address alignment"
	echo "    alignment  number (means 2^alignment) / notused(-1)"
	echo "               default: notused"
	echo " -x xfer_size  Optional custom transfer size"
	echo "    xfer_size  number / notused(0)"
	echo "               default: notused"
	echo " -p            Use polling for completion instead of interrupts"
	echo " -w            Wait previous test to end before testing"
	echo " action:"
	echo "  list:       list DMA devices"
	echo "  kern:       list kernel configured DMA test parameters"
	echo "  user:       list user specified DMA test parameters"
	echo "  test:       perform DMA test on specified channel"
	echo " channel:     any / channel name"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

get_dmatest()
{
	f=${DMATEST_MODULE}/parameters/$1
	cat $f
}

set_dmatest()
{
	f=${DMATEST_MODULE}/parameters/$1
	echo $2 > $f
}

update_dmatest()
{
	v=`get_dmatest $1`
	if [ "x$v" != "x$2" ]; then
		echo "Info: Updating $1..."
		set_dmatest $1 $2
	fi
}

wait_dmatest()
{
	v=`get_dmatest iterations`
	if [ "x$v" != "x0" ]; then
		echo "Info: Wating for completion..."
		v=`get_dmatest run`
		while [ "x$v" = "xY" ]
		do
			sleep 1
			echo -n "."
			v=`get_dmatest run`
		done
		echo
	fi
}

while getopts "a:b:d:i:pt:vwx:" opt
do
	case $opt in
	a) if [ "x$OPTARG" = "xnotused" ]; then
		DMATEST_alignment=-1
	   else
		DMATEST_alignment=$OPTARG
	   fi;;
	b) DMATEST_test_buf_size=$OPTARG;;
	d) if [ "x$OPTARG" = "xverify" ]; then
		DMATEST_noverify=Y
	   elif [ "x$OPTARG" = "xrandom" ]; then
		DMATEST_norandom=Y
	   fi;;
	i) if [ "x$OPTARG" = "xinfinite" ]; then
		DMATEST_iterations=0
	   else
		DMATEST_iterations=$OPTARG
	   fi;;
	p) DMATEST_polled=Y;;
	t) if [ "x$OPTARG" = "xinfinite" ]; then
		DMATEST_timeout=-1
	   else
		DMATEST_timeout=$OPTARG
	   fi;;
	v) DMATEST_verbose=Y;;
	w) DMATEST_waited=Y;;
	x) if [ "x$OPTARG" = "xnotused" ]; then
		DMATEST_transfer_size=0
	   else
		DMATEST_transfer_size=$OPTARG
	   fi;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

if [ -z $1 ]; then
	echo "Error: dmatest action is not specified!"
	usage
fi
if [ "x$1" = "xlist" ]; then
	ls /sys/class/dma
fi
if [ "x$1" = "xtest" ]; then
	if [ ! -d ${DMATEST_MODULE} ]; then
		echo "Error: dmatest is not probed!"
	fi
	if [ -z $2 ]; then
		echo "Warning: no channel specified, use 'any'!"
	fi
	DMATEST_run=`get_dmatest run`
	if [ "x$DMATEST_run" = "xY" ]; then
		if [ "x$DMATEST_waited" = "xN" ]; then
			echo "Error: dmatest is running!"
			exit 1;
		fi
		wait_dmatest
	fi
	update_dmatest test_buf_size $DMATEST_test_buf_size
	update_dmatest iterations $DMATEST_iterations
	update_dmatest timeout $DMATEST_timeout
	update_dmatest noverify $DMATEST_noverify
	update_dmatest norandom $DMATEST_norandom
	update_dmatest verbose $DMATEST_verbose
	update_dmatest alignment $DMATEST_alignment
	update_dmatest transfer_size $DMATEST_transfer_size
	update_dmatest channel $2
	update_dmatest polled $DMATEST_polled
	set_dmatest run Y
fi
if [ "x$1" = "xkern" ]; then
	if [ ! -d ${DMATEST_MODULE} ]; then
		echo "Error: dmatest is not probed!"
	fi
	grep -H . ${DMATEST_MODULE}/parameters/*
fi
if [ "x$1" = "xuser" ]; then
	echo "test_buf_size=$DMATEST_test_buf_size"
	echo "iterations=$DMATEST_iterations"
	echo "timeout=$DMATEST_timeout"
	echo "noverify=$DMATEST_noverify"
	echo "norandom=$DMATEST_norandom"
	echo "verbose=$DMATEST_verbose"
	echo "alignment=$DMATEST_alignment"
	echo "transfer_size=$DMATEST_transfer_size"
	echo "polled=$DMATEST_polled"
fi
