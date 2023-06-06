#!/bin/bash
#
# Build CPU2006 images to the bench folder.

TOP=`pwd`
SCRIPT=`(cd \`dirname $0\`; pwd)`
SRCDIR=`(cd ${SCRIPT}/..; pwd)`
WORKING_DIR=`(cd ${SCRIPT}/../../..; pwd)`

CPU2006_BENCHMARKS=
CPU2006_CINT2006=
CPU2006_CFP2006=

usage()
{
	echo "Usage:"
	echo "`basename $0` [-b benchmark] [-c] [-r] [-t]"
	echo "Where:"
	echo " -b bench:    specify benchmark"
	echo "    cint:     all CINT2006 benchmarks"
	for b in $CPU2006_CINT2006; do
		echo "    $b"
	done
	echo "    cfp:      all CFP2006 benchmarks"
	for b in $CPU2006_CFP2006; do
		echo "    $b"
	done
	echo " -r:          force rebuild benchmarks"
	echo " -c:          force cleanup benchmarks"
	echo " -t:          force rebuild tools"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

cpu2006_iscint()
{
	matched=`cat ${CPU2006_ROOT}/benchspec/CPU2006/CINT2006.bset | grep $1`
	matched=`echo $matched | cut -d ' ' -f1`
	if [ "x$matched" = "x$1" ]; then
		echo "CINT2006"
	fi
}

cpu2006_iscfp()
{
	matched=`cat ${CPU2006_ROOT}/benchspec/CPU2006/CFP2006.bset | grep $1`
	matched=`echo $matched | cut -d ' ' -f1`
	if [ "x$matched" = "x$1" ]; then
		echo "CFP2006"
	fi
}

cpu2006_init()
{
	benchs=`cpu2006_benchmarks`
	for b in $benchs; do
		cint=`cpu2006_iscint $b`
		cfp=`cpu2006_iscfp $b`
		if [ "x$cint$cfp" = "xCINT2006" ]; then
			CPU2006_CINT2006="${CPU2006_CINT2006} $b"
		else
			CPU2006_CFP2006="${CPU2006_CFP2006} $b"
		fi
	done
}

cpu2006_elfname()
{
	b=$1
	selfn=${b##*.} # cut off the numbers ###.short_exe
	if [ $b == "483.xalancbmk" ]; then 
		selfn=Xalan #WTF SPEC???
	fi
	echo $selfn
}

cpu2006_benchmarks()
{
	dir=${CPU2006_ROOT}/benchspec/CPU2006
	files=`ls $dir`
	for f in $files; do
		if [ -d ${dir}/${f} ]; then
			echo $f
		fi
	done
}

cpu2006_tools()
{
	(
		cd ${CPU2006_ROOT}/tools/src
		./buildtools
	)
}

if [ -z $ARCH ]; then
	ARCH=riscv
fi
if [ -z $MACH ]; then
	MACH=spike64
fi
if [ -z ${CROSS_COMPILE} ]; then
	export CROSS_COMPILE=riscv64-unknown-linux-gnu-
fi
if [ -z ${CPU2006_ROOT} ]; then
	CPU2006_ROOT=${WORKING_DIR}/cpu2006
fi

cpu2006_init

while getopts "b:crt" opt
do
	case $opt in
	b) if [ "x$OPTARG" = "xdefault" ]; then
		CPU2006_BENCHMARKS="400.perlbench \
				    401.bzip2 \
				    403.gcc \
				    429.mcf \
				    445.gobmk \
				    456.hmmer \
				    458.sjeng \
				    462.libquantum \
				    464.h264ref \
				    471.omnetpp \
				    473.astar \
				    483.xalancbmk"
	   else
		CPU2006_BENCHMARKS="$CPU2006_BENCHMARKS $OPTARG"
	   fi;;
	c) CPU2006_CLEAN_BENCHES=yes;;
	r) CPU2006_BUILD_BENCHES=yes;;
	t) CPU2006_BUILD_TOOLS=yes;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

if [ -f $SCRIPT/config/config-cpu2006-$ARCH-$MACH ]; then
	CPU2006_CONFIG=$SCRIPT/config/config-cpu2006-$ARCH-$MACH
else
	CPU2006_CONFIG=$SCRIPT/config/config-cpu2006-$ARCH
fi
CPU2006_OBJDIR=$TOP/obj/cpu2006-$ARCH
if [ -z $CPU2006_BENCHMARKS ]; then
	CPU2006_BENCHMARKS=`cpu2006_benchmarks`
fi
CPU2006_INPUT=test
CPU2006_COMMANDS=commands.txt
CPU2006_DIR=${TOP}/obj/bench/opt/cpu2006

if [ "x${CPU2006_BUILD_TOOLS}" = "xyes" ]; then
	# Tune buildtools steps
	#SKIPCLEAN=1
	#SKIPMAKE=1
	#SKIPXZ=1
	#SKIPTAR=1
	#SKIPMD5=1
	#SKIPSPECINVOKE=1
	#SKIPRXP=1
	#SKIPEXPAT=1
	#SKIPPERL=1
	#SKIPPERL2=1
	#SKIPPERTEST=1
	cpu2006_tools
fi
CPU2006_ARCHIVE=yes
if [ "x${CPU2006_CLEAN_BENCHES}" = "xyes" ]; then
	echo "Cleaning cpu2006 binaries from $CPU2006_ROOT..."
	cp -f $CPU2006_CONFIG $CPU2006_ROOT/config/$ARCH.cfg
	BENCHMARKS=(${CPU2006_BENCHMARKS})
	(
		cd $CPU2006_ROOT
		. ./shrc
		time runspec --config $ARCH --size $CPU2006_INPUT --action scrub $CPU2006_BENCHMARKS
	)
	CPU2006_ARCHIVE=no
	rm -f $CPU2006_ROOT/config/$ARCH.cfg
else
	# TODO: verify if the benchmarks have already been built
	echo
fi
if [ "x${CPU2006_BUILD_BENCHES}" = "xyes" ]; then
	echo "Building cpu2006 binaries from $CPU2006_ROOT..."
	cp -f $CPU2006_CONFIG $CPU2006_ROOT/config/$ARCH.cfg
	BENCHMARKS=(${CPU2006_BENCHMARKS})
	(
		cd $CPU2006_ROOT
		. ./shrc
		time runspec --config $ARCH --size $CPU2006_INPUT --action setup $CPU2006_BENCHMARKS
	)
	CPU2006_ARCHIVE=yes
	rm -f $CPU2006_ROOT/config/$ARCH.cfg
fi
if [ "x${CPU2006_ARCHIVE}" = "xyes" ]; then
	echo "Generating cpu2006 binaries from $CPU2006_ROOT..."
	rm -rf $CPU2006_OBJDIR
	mkdir -p $CPU2006_OBJDIR
	for b in $CPU2006_BENCHMARKS; do
		echo ${b}
		CPU2006_ELF=`cpu2006_elfname $b`
		echo $CPU2006_ELF
		CPU2006_RUNDIR=${CPU2006_ROOT}/benchspec/CPU2006/$b/run
		CPU2006_INPUTDIR=${CPU2006_RUNDIR}/run_base_${CPU2006_INPUT}_${ARCH}.0000
		ls $CPU2006_RUNDIR
		ls $CPU2006_INPUTDIR
		# make a symlink to SPEC (to prevent data duplication for
		# huge input files)
		CPU2006_INPUTLNK=${CPU2006_OBJDIR}/${b}_${CPU2006_INPUT}
		if [ -d $CPU2006_INPUTLNK ]; then
			unlink $CPU2006_INPUTLNK
		fi
		ln -sf $CPU2006_INPUTDIR $CPU2006_INPUTLNK
	done
	for b in $CPU2006_BENCHMARKS; do
		echo ${b}
		CPU2006_ELF=`cpu2006_elfname $b`
		echo $CPU2006_ELF
		CPU2006_RUNDIR=${CPU2006_ROOT}/benchspec/CPU2006/$b/run
		CPU2006_INPUTDIR=${CPU2006_RUNDIR}/run_base_${CPU2006_INPUT}_${ARCH}.0000
		echo "Installing cpu2006 benchmark $b..."
		mkdir -p $CPU2006_DIR/$b
		cp -r ${SCRIPT}/cpu2006/commands $CPU2006_DIR/commands
		cp ${SCRIPT}/cpu2006/run.sh $CPU2006_DIR/run.sh
		sed -i '4s/.*/INPUT_TYPE='${CPU2006_INPUT}' #this line was auto-generated from build_cpu2006.sh/' $CPU2006_DIR/run.sh
		for f in $CPU2006_INPUTDIR/*; do
			echo $f
			if [[ -d $f ]]; then
				echo "Installing $b directory $f..."
				cp -r $f $CPU2006_DIR/$b/$(basename "$f")
			else
				echo "Installing $b file $f..."
				cp $f $CPU2006_DIR/$b/$(basename "$f")
			fi
		done
		mv ${CPU2006_DIR}/$b/${CPU2006_ELF}_base.${ARCH} \
			${CPU2006_DIR}/$b/${CPU2006_ELF}
	done
fi
