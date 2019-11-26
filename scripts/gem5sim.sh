#!/bin/bash
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
#                [-g simpoint_slice.gem5] [-o simpoint_slice.S]
#                [-s step] [-c checkpoint] [-i interval] [-m]
#                [-a architecture] [-p program] [-O program-parameters]
#                [-b binary] [-d disk]
#
# DESCRIPTION:
#     This script makes SimPoint based GEM5 simulation operations easier.
#     The simulation steps can be specified by [-s] and should be executed
#     in the following order:
#     gem5bbv:
#       Generate basic block vectors using GEM5.
#     simpoint:
#       Generate simulation simpoints using SimPoint, it accepts GEM5
#       generated basic block vectors.
#     gem5cpt:
#       Generate GEM5 checkpoints using GEM5, it accepts SimPoint generated
#       simulation simpoints.
#     gem5sim:
#       Simulate checkpoint using GEM5, it accepts GEM5 generated
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
#   $ ./gem5sim.sh -s gem5bbv -a arm64 -p hello -i 1000
#   $ ./gem5sim.sh -s simpoint
#   $ ./gem5sim.sh -s gem5cpt -a arm64 -p hello -i 1000
#   $ ./gem5sim.sh -c Help
#   Checkpoint Simpoint(BBV) Weight ClusterLabel
#   1 0 0.027027 1
#   2 1 0.027027 2
#   3 2 0.027027 5
#   4 3 0.027027 6
#   5 5 0.810811 3
#   6 14 0.0810811 0
#
#   To simulate the highest weight checkpoint (5 in the above example):
#   $ ./gem5sim.sh -s gem5sim -a arm64 -p hello -c 5 -x Exec

SCRIPT=`(cd \`dirname $0\`; pwd)`
SDFIRM=`(cd ${SCRIPT}/..; pwd)`
GEM5_SRC=~/workspace/gem5
GEM5_DBG=
GEM5_DEBUG_HELP=
GEM5_DEBUG_FLAGS=
GEM5_FULL_SYSTEM=
SIM_STEP=gem5sim
SIM_INTERVAL=1000000
SIM_CHECKPOINT=00
SIM_K_BEST=30
SIM_LIST_CHECKPOINTS=
SIM_MICRO_OP=
SE_ARCH=arm
SE_PROG=hello
SE_OPTS=""
SE_LIST_PROGS=
SE_LIST_ARCHS=
FS_DISK=linaro-minimal-aarch64.img
FS_KERN=vmlinux.aarch64.20140821
FS_LIST_DISKS=
FS_LIST_KERNS=
SE_ARCH_X86="x86 i386 x86_64"
SE_ARCH_ARM="arm thumb arm32 arm64 aarch32 aarch64"
SE_ARCH_RISCV="riscv rv32 rv64"
SIMPOINT_SLICE_FILE=${SDFIRM}/init/gem5/simpoint_slice.S

usage()
{
	echo "Usage:"
	echo "`basename $0` [-w gem5] [-x flag] [-f]"
	echo "	[-g .gem5_file] [-o .S_file]"
	echo "  [-s step] [-c checkpoint] [-i interval] [-k best] [-m]"
	echo "  [-a architecture] [-p program]"
	echo "  [-b binary] [-d disk]"
	echo "Where:"
	echo "GEM5 global options:"
	echo "        -w: specify GEM5 home source, also working directory"
	echo "            default ${GEM5_SRC}"
	echo "        -x: add GEM5 debug flags, special flags include:"
	echo "            Help: to list all debug flags"
	echo "        -f: use GEM5 full system (fs.py) mode"
	echo "            default syscall emulation (se.py) mode"
	echo "GEM5 sdfirm build options:"
	echo "        -g: use kconfig generated simpoint_slice.gem5"
	echo "        -o: output path of generated simpoint slice"
	echo "            default path is init/gem5/simpoint_slice.S"
	echo "GEM5 simpoint options:"
	echo "        -s: specify GEM5 simulation step, the steps are ordered as:"
	echo "            gem5bbv: to generate basic block vectors w/ GEM5"
	echo "            simpoint: to generate simpoints w/ GEM5 generated bbv"
	echo "            gem5cpt: to generate simpoint checkpoints w/ GEM5"
	echo "            gem5sim: to simulate checkpoint w/ GEM5"
	echo "        -c: specify GEM5 checkpoint for gem5sim step"
	echo "            Help: to list all simulatable checkpoints"
	echo "        -i: specify simpoint interval, for gem5bbv/gem5cpt steps"
	echo "            default ${SIM_INTERVAL}"
	echo "        -k: Specify K for K's best for simpoint step"
	echo "            default ${SIM_K_BEST}"
	echo "        -m: Allow to take checkpoint at middle of micro opcode"
	echo "GEM5 syscall emulation options:"
	echo "        -a: specify architecture where test program is located"
	echo "            Help: to list all program architectures"
	echo "        -p: specify test program, special progs include"
	echo "            Help: to list all test programs"
	echo "        -O: specify parameters for test programi, if any"
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

while getopts "a:b:c:d:fg:hi:k:mo:O:p:s:w:x:" opt
do
	case $opt in
	w) GEM5_SRC=$OPTARG;;
	f) GEM5_FULL_SYSTEM=true;;
	x) if [ "x$OPTARG" = "xHelp" ]; then
		GEM5_DEBUG_HELP=true
	   elif [ -z $GEM5_DBG ]; then
		GEM5_DBG=${OPTARG}
	   else
		GEM5_DBG=${GEM5_DBG},${OPTARG}
	   fi;;
	g) source $OPTARG;;
	o) SIMPOINT_SLICE_FILE=$OPTARG;;
	c) if [ "x$OPTARG" = "xHelp" ]; then
		SIM_LIST_CHECKPOINTS=true
	   else
		SIM_CHECKPOINT=${OPTARG}
	   fi;;
	s) SIM_STEP=$OPTARG;;
	i) SIM_INTERVAL=$OPTARG;;
	k) SIM_K_BEST=$OPTARG;;
	a) if [ "x$OPTARG" = "xHelp" ]; then
		SE_LIST_ARCHS=true
	   else
		SE_ARCH=${OPTARG}
	   fi;;
	p) if [ "x$OPTARG" = "xHelp" ]; then
		SE_LIST_PROGS=true
	   else
		SE_PROG=${OPTARG}
	   fi;;
	O) SE_OPTS=${OPTARG};;
	d) if [ "x$OPTARG" = "xHelp" ]; then
		FS_LIST_DISKS=true
	   else
		FS_DISK=${OPTARG}
	   fi;;
	b) if [ "x$OPTARG" = "xHelp" ]; then
		FS_LIST_KERNS=true
	   else
		FS_KERN=${OPTARG}
	   fi;;
	m) SIM_MICRO_OP=true;;
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
			break
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

sanity_bbv()
{
	if [ ! -f simpoint.bb.gz ]; then
		fatal_usage "simpoints not found, execute -s gem5bbv first"
	fi
}

sanity_simpoints()
{
	(
		cd ${GEM5_SRC}/m5out
		if [ ! -f ${GEM5_ARCH}.weights ]; then
			fatal_usage "${GEM5_ARCH}.weights not found, execute -s simpoint first"
		fi
		if [ ! -f ${GEM5_ARCH}.simpts ]; then
			fatal_usage "${GEM5_ARCH}.simpts not found, execute -s simpoint first"
		fi
	)
}

sanity_cpt()
{
	sanity_simpoints
	# TODO: NYI, leave it for gem5 exceptions
}

simpoints()
{
	sanity_simpoints
	n=0
	while read w c;
	do
		weights[$n]=$w
		checkpoints[$n]=$c
		let n++
	done < ${GEM5_ARCH}.weights

	n=0;
	max_s=-1
	min_s=-1
	while read b c;
	do
		simpoints[$n]=$b
		if [ $max_s -lt 0 -o $max_s -lt ${simpoints[$n]} ]; then
			max_s=${simpoints[$n]}
		fi
		if [ $min_s -lt 0 -o $min_s -gt ${simpoints[$n]} ]; then
			min_s=${simpoints[$n]}
		fi
		if [ ${checkpoints[$n]} != $c ]; then
			echo "Wrong simpoints at $n"
		fi
		let n++
	done < ${GEM5_ARCH}.simpts
	max_n=$n

	echo 'Checkpoint Simpoint(BBV) Weight ClusterLabel'
	c=$min_s
	n=0
	while [ $n -lt $max_n ]
	do
		m=0
		while [ $m -lt $max_n ]
		do
			if [ ${simpoints[$m]} -eq $c ]; then
				break
			fi
			let m++
		done
		echo "$((n+1)) ${simpoints[$m]} ${weights[$m]} ${checkpoints[$m]}"
		let n++
		x=$c
		c=$max_s
		m=0
		while [ $m -lt $max_n ]
		do
			v=${simpoints[$m]}
			if [ $v -gt $x -a $v -lt $c ]; then
				c=$v
			fi
			let m++
		done
	done
}

GEM5_ARCH=`gem5_ARCH`
FS_ARCH=`gem5_arch`
GEM5=./build/${GEM5_ARCH}/gem5.opt
M5_PATH="${GEM5_SRC}/fs_images/${FS_ARCH}"

if [ ! -z $GEM5_DEBUG_HELP ]; then
	(cd $GEM5_SRC; ${GEM5} --debug-help)
	exit 0
fi
if [ ! -z $SE_LIST_PROGS ]; then
	(cd $GEM5_SRC; ls tests/test-progs)
	exit 0
fi
if [ ! -z $SE_LIST_ARCHS ]; then
	(cd $GEM5_SRC; ls tests/test-progs/${SE_PROG}/bin)
	exit 0
fi
if [ ! -z $FS_LIST_DISKS ]; then
	(cd $GEM5_SRC; ls ${M5_PATH}/disks)
	exit 0
fi
if [ ! -z $FS_LIST_KERNS ]; then
	(cd ${M5_PATH}/binaries; ls vmlinux.*)
	exit 0
fi
if [ ! -z $SIM_LIST_CHECKPOINTS ]; then
	(cd ${GEM5_SRC}/m5out; simpoints)
	exit 0
fi

SIMPOINT_OPTS="--cpu-type=NonCachingSimpleCPU"
# --at-instruction:
# It actually might be no use other than psychological comfort in simpoint
# simulation. We actually want to avoid the following checkpoint where a
# partial uop is bumped up at the beginning of a checkpoint:
#  info: Entering event queue @ 2958500.  Starting simulation...
#  2959000: system.switch_cpus T0 : @_IO_file_xsputn+380.1  :   addxi_uop   sp, sp, #64  : IntAlu :  D=0x0000007ffffefc80
#  2959500: system.switch_cpus T0 : @_IO_file_xsputn+384    :   ret                      : IntAlu :
if [ -z ${SIM_MICRO_OP} ]; then
	echo "WARN: --at-instruction may not work with --restore-simpoint-checkpoint."
	# NOTE: The option of restore-simpoint-checkpoint requires a
	#       special GEM5 fix to enable --at-instruction option.
	# if [ ${SIM_STEP} != "gem5sim" ]; then
		SIMPOINT_OPTS="${SIMPOINT_OPTS} --at-instruction"
	# fi
fi
if [ ${SIM_STEP} = "gem5bbv" ]; then
	SIMPOINT_OPTS="${SIMPOINT_OPTS} --simpoint-profile --simpoint-interval ${SIM_INTERVAL}"
elif [ ${SIM_STEP} = "gem5cpt" ]; then
	sanity_simpoints
	SIMPOINT_OPTS="${SIMPOINT_OPTS} --take-simpoint-checkpoint=m5out/${GEM5_ARCH}.simpts,m5out/${GEM5_ARCH}.weights,${SIM_INTERVAL},0"
elif [ ${SIM_STEP} = "gem5sim" ]; then
	sanity_cpt
	SIMPOINT_OPTS="${SIMPOINT_OPTS} --enable-simpoint-slicing --restore-simpoint-checkpoint --checkpoint-dir m5out/ -r ${SIM_CHECKPOINT}"
elif [ ${SIM_STEP} = "simpoint" ]; then
	(
		cd ${GEM5_SRC}/m5out
		sanity_bbv
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
	if [ -z $GEM5_FULL_SYSTEM ]; then
		if [ "$SE_OPTS" = "" ]; then
			CMD_SE_OPTIS=""
		else
			CMD_SE_OPTIS="-o $SE_OPTS"
		fi
		echo "SE mode: ${SE_ARCH} ${SE_PROG}"
		${GEM5} ${GEM5_DEBUG_FLAGS} \
			configs/example/se.py \
			-c tests/test-progs/${SE_PROG}/bin/${SE_ARCH}/linux/${SE_PROG} \
			$CMD_SE_OPTIS \
			${SIMPOINT_OPTS}
	else
		echo "FS mode: ${FS_ARCH} ${FS_KERN}"
		# So we only supports ARM64
		${GEM5} ${GEM5_DEBUG_FLAGS} \
			configs/example/fs.py \
			--machine-type=VExpress_EMM64 \
			--dtb-file=vexpress.aarch64.20140821.dtb \
			--kernel=${FS_KERN} \
			--disk-image=${FS_DISK} \
			${SIMPOINT_OPTS}
	fi
)

if [ ${SIM_STEP} = "gem5sim" ]; then
	if [ ! -z ${SIMPOINT_SLICE_FILE} ]; then
		source_file=`ls ${GEM5_SRC}/m5out/cpt.simpoint_$(($SIM_CHECKPOINT - 1))_*/simpoint_slice.S 2>&1`
		if [ $? != 0 ] ; then
			source_file=`ls ${GEM5_SRC}/m5out/cpt.simpoint_0$(($SIM_CHECKPOINT - 1))_*/simpoint_slice.S`
		fi
		cp -f ${source_file} ${SIMPOINT_SLICE_FILE}
	fi
fi
