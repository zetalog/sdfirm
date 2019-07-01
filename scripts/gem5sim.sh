#!/bin/sh
#
# COPYRIGHT: 
#   Copyright (c) 2019 - 2019 by ZETALOG, All rights reserved.
#   Author: Lv ZHENG <zhenglv@hotmail.com>.
#
# NAME:
#     gem5sim.sh
#
# SYNOPSIS:
#     gem5sim.sh [-w gem5] [-f] [-x flag]
#                [-s step] [-c cpt] [-i interval]
#                [-a architecture] [-p program]
#                [-b binary] [-d disk]
#
# DESCRIPTION:
#     This scripts makes simpoint based gem5 simulation easier. The
#     simulation steps can be specified by [-s] and are ordered as:
#     gem5bbv:
#       Generate basic block vectors using gem5.
#     simpoint:
#       Generate simulation checkpoints using SimPoint, it accepts gem5
#       generated basic block vectors.
#     gem5cpt:
#       Generate gem5 checkpoints using gem5, it accepts SimPoint generated
#       simulation checkpoints.
#     gem5sim:
#       Simulate checkpoint using gem5, it accepts gem5 generated
#       checkpoints.
#
# EXAMPLES:
#
#   To build test program:
#   $ mkdir ${GEM5_SRC}/tests/test-progs/hello/bin/arm64/linux
#   $ aarch64-linux-gnu-gcc -static -o \
#     ${GEM5_SRC}/tests/test-progs/hello/bin/arm64/linux/hello \
#     ${GEM5_SRC}/tests/test-progs/hello/src/hello.c
#
#   To generate simpoint:
#   $ ./gem5sim.sh -s gem5bbv -i 1000 -p hello -a arm64
#   $ ./gem5sim.sh -s simpoint
#   $ ./gem5sim.sh -s gem5cpt -i 1000
#   $ ./gem5sim.sh -s gem5sim -c Help
#   Weight Checkpoint
#   0.0810811 0
#   0.027027 1
#   0.027027 2
#   0.810811 3
#   0.027027 5
#   0.027027 6
#
#   To simulate the best checkpoint (3 in the above example):
#   $ ./gem5sim.sh -s gem5sim -x Exec -c 3

SCRIPT=`(cd \`dirname $0\`; pwd)`
GEM5_SRC=~/workspace/gem5
GEM5_DBG=
GEM5_DEBUG_HELP=
GEM5_DEBUG_FLAGS=
GEM5_FULL_SYSTEM=
SIM_STEP=gem5sim
SIM_INTERVAL=1000000
SIM_CHEPOINT=0
SIM_K_BEST=30
SIM_LIST_CHECKPOINTS=
SE_ARCH=arm
SE_PROG=hello
SE_LIST_PROGS=
SE_LIST_ARCHS=
FS_DISK=linaro-minimal-aarch64.img
FS_KERN=vmlinux.aarch64.20140821
FS_LIST_DISKS=
FS_LIST_KERNS=
SE_ARCH_ARM="arm arm64"
SE_ARCH_X86="i386 x86 x86_64"
SE_ARCH_RISCV="riscv rv32 rv64"

usage()
{
	echo "Usage:"
	echo "`basename $0` [-w gem5] [-x flag] [-f]"
	echo "  [-s step] [-c checkpoint] [-i interval] [-k best]"
	echo "  [-a arch] [-p prog]"
	echo "  [-b binary] [-d disk]"
	echo "Where:"
	echo "GEM5 global options:"
	echo "        -w: specify gem5 home source, also working directory"
	echo "            default ${GEM5_SRC}"
	echo "        -x: add GEM5 debug flags, special flags include:"
       	echo "            Help: to list all debug flags"
	echo "        -f: use full speed mode"
	echo "GEM5 simpoint options:"
	echo "        -s: specify GEM5 simulation step, the steps are ordered as:"
       	echo "            gem5bbv: to generate basic block vectors w/ gem5"
       	echo "            simpoint: to generate simpoints w/ gem5 generated bbv"
       	echo "            gem5cpt: to generate simpoint checkpoints w/ gem5"
       	echo "            gem5sim: to simulate checkpoint w/ gem5"
	echo "        -c: specify GEM5 checkpoint for gem5sim mode"
       	echo "            Help: to list all simulatable checkpoints"
	echo "        -i: specify simpoint interval, for gem5bbv/gem5cpt modes"
	echo "            default ${SIM_INTERVAL}"
	echo "        -k: Specify K for K's best"
	echo "            default ${SIM_K_BEST}"
	echo "GEM5 syscall emulation options:"
	echo "        -a: specify architecture where test program is located"
       	echo "            Help: to list all program architectures"
	echo "        -p: specify test program, special progs include"
       	echo "            Help: to list all test programs"
	echo "GEM5 full system options:"
	echo "        -b: specify system binary, special binary include"
       	echo "            Help: to list all binaries"
	echo "        -d: specify disk image, special images include"
       	echo "            Help: to list all disk images"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "a:b:c:d:fhi:k:p:s:w:x:" opt
do
	case $opt in
	w) GEM5_SRC=$OPTARG;;
	f) GEM5_FULL_SYSTEM=true;;
	x) if [ "x$OPTARG" = "xHelp" ]; then
		GEM5_DEBUG_HELP=true;
	   elif [ -z $GEM5_DBG ]; then
		GEM5_DBG=${OPTARG}
	   else
		GEM5_DBG=${GEM5_DBG},${OPTARG}
	   fi;;
	c) if [ "x$OPTARG" = "xHelp" ]; then
		SIM_LIST_CHECKPOINTS=true;
	   else
		SIM_CHECKPOINT=${OPTARG}
	   fi;;
	s) SIM_STEP=$OPTARG;;
	i) SIM_INTERVAL=$OPTARG;;
	k) SIM_K_BEST=$OPTARG;;
	a) if [ "x$OPTARG" = "xHelp" ]; then
		SE_LIST_ARCHS=true;
	   else
		SE_ARCH=${OPTARG}
	   fi;;
	p) if [ "x$OPTARG" = "xHelp" ]; then
		SE_LIST_PROGS=true;
	   else
		SE_PROG=${OPTARG}
	   fi;;
	d) if [ "x$OPTARG" = "xHelp" ]; then
		FS_LIST_DISKS=true;
	   else
		FS_DISK=${OPTARG}
	   fi;;
	b) if [ "x$OPTARG" = "xHelp" ]; then
		FS_LIST_KERNS=true;
	   else
		FS_KERN=${OPTARG}
	   fi;;
	h) usage 0;;
	?) fatal_usage "Invalid option";;
	esac
done
shift $(($OPTIND - 1))

gem5_one_arch()
{
	for arch in $2
	do
		if [ $arch = $SE_ARCH ]; then
			echo $1
		fi
	done
}

gem5_ARCH()
{
	gem5_one_arch ARM "${SE_ARCH_ARM}"
	gem5_one_arch X86 "${SE_ARCH_X86}"
	gem5_one_arch RISCV "${SE_ARCH_RISCV}"
}

gem5_arch()
{
	gem5_one_arch arm "${SE_ARCH_ARM}"
	gem5_one_arch x86 "${SE_ARCH_X86}"
	gem5_one_arch riscv "${SE_ARCH_RISCV}"
}

GEM5_ARCH=`gem5_ARCH`
FS_ARCH=`gem5_arch`
GEM5=./build/${GEM5_ARCH}/gem5.opt

SIMPOINT_OPTS="--cpu-type=NonCachingSimpleCPU"
if [ ${SIM_STEP} = "gem5bbv" ]; then
	SIMPOINT_OPTS="${SIMPOINT_OPTS} --simpoint-profile --simpoint-interval ${SIM_INTERVAL}"
elif [ ${SIM_STEP} = "gem5cpt" ]; then
	SIMPOINT_OPTS="${SIMPOINT_OPTS} --take-simpoint-checkpoint=m5out/${GEM5_ARCH}.simpts,m5out/${GEM5_ARCH}.weights,${SIM_INTERVAL},0"
elif [ ${SIM_STEP} = "gem5sim" ]; then
	SIMPOINT_OPTS="${SIMPOINT_OPTS} --restore-simpoint-checkpoint --checkpoint-dir m5out/ -r ${SIM_CHECKPOINT}"
elif [ ${SIM_STEP} = "simpoint" ]; then
	(
		cd ${GEM5_SRC}/m5out
		simpoint -loadFVFile simpoint.bb.gz -inputVectorsGzipped \
			-maxK ${SIM_K_BEST} \
			-saveSimpoints ${GEM5_ARCH}.simpts \
			-saveSimpointWeights ${GEM5_ARCH}.weights
	)
	exit 0
else
	fatal_usage "Invalid argument -m ${SIM_STEP}"
fi

if [ ! -z $GEM5_DBG ]; then
	GEM5_DEBUG_FLAGS=--debug-flags=${GEM5_DBG}
fi

(
	cd ${GEM5_SRC}
	export M5_PATH="${GEM5_SRC}/fs_images/${FS_ARCH}/"

	if [ ! -z $GEM5_DEBUG_HELP ]; then
		${GEM5} --debug-help
		exit 0
	fi
	if [ ! -z $SIM_LIST_CHECKPOINTS ]; then
		(cd ${GEM5_SRC}/m5out; echo "Weight Checkpoint"; cat ${GEM5_ARCH}.weights)
		exit 0
	fi

	if [ -z $GEM5_FULL_SYSTEM ]; then
		if [ ! -z $SE_LIST_PROGS ]; then
			ls tests/test-progs
			exit 0
		fi
		if [ ! -z $SE_LIST_ARCHS ]; then
			ls tests/test-progs/${SE_PROG}/bin
			exit 0
		fi
		${GEM5} ${GEM5_DEBUG_FLAGS} \
			configs/example/se.py \
			-c tests/test-progs/${SE_PROG}/bin/${SE_ARCH}/linux/${SE_PROG} \
			${SIMPOINT_OPTS}
	else
		if [ ! -z $FS_LIST_DISKS ]; then
			ls ${M5_PATH}/disks
			exit 0
		fi
		if [ ! -z $FS_LIST_KERNS ]; then
			(cd ${M5_PATH}/binaries; ls vmlinux.*)
			exit 0
		fi
		${GEM5} ${GEM5_DEBUG_FLAGS} \
			configs/example/fs.py \
			--machine-type=VExpress_EMM64 \
			--dtb-file=vexpress.aarch64.20140821.dtb \
			--kernel=${FS_KERN} \
			--disk-image=${FS_DISK} \
			${SIMPOINT_OPTS}
	fi
)
