#!/bin/bash
#
# Build CPU2006 images to the bench folder.

TOP=`pwd`
SCRIPT=`(cd \`dirname $0\`; pwd)`
WORKING_DIR=`(cd ${SCRIPT}/../../..; pwd)`

CPU2006_BENCHMARKS=
CPU2006_CINT2006=
CPU2006_CFP2006=
CPU2006_DATA="test ref train"
CPU2006_REPORTABLE=yes
CPU2006_METRICS=speed
CPU2006_COPIES=4
CPU2006_OUTPUT_ROOT=/opt/cpu2006

usage()
{
	echo "Usage:"
	echo "`basename $0` [-b benchmark] [-c] [-r] [-h] [-t]"
	echo "              [-d data] [-m metrics]"
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
	echo " -d data:     specify the size of input data set"
	echo "    all:      data used by all runs (default)"
	echo "    ref:      real data set for all result reporting"
	echo "    test:     data for a simple test of an functional executable"
	echo "    train:    data for feedback-directed optimization"
	echo " -m metrics:  specify the testing metrics"
	echo "    speed:    SPECspeed testing metrics (default)"
	echo "    rate:     SPECrate testing metrics (copies:4)"
	echo " -c:          force cleanup benchmarks"
	echo " -r:          force rebuild benchmarks"
	echo " -h:          force rebuild host tools"
	echo " -t:          force rebuild target tools"
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
	if [ $b == "482.sphinx3" ]; then
		selfn=sphinx_livepretend
	fi
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

cpu2006_patch_tools()
{
	(
		cd ${CPU2006_ROOT}
		if [ ! -f ${CPU2006_ROOT}/tools/src/.BUILDTOOLS_PATCHED ]; then
			patch -p 1 < ${SCRIPT}/cpu2006/perl-skiptest.patch
			patch -p 1 < ${SCRIPT}/cpu2006/make-compile.patch
			patch -p 1 < ${SCRIPT}/cpu2006/perl-compile.patch
			touch ${CPU2006_ROOT}/tools/src/.BUILDTOOLS_PATCHED
		fi
	)
}

# Do not invoke this due to the failure in building target perl with a
# duplicate and configured host perl source tree.
# This feature is never invoked by build_image.sh and can only be used
# directly via build_cpu2006.sh.
# Note that, before using it please patch buildtools with
# cpu2006/perl-skiptest.patch.
cpu2006_build_host_tools()
{
	(
		cd ${CPU2006_ROOT}/tools/src
		./buildtools
	)
}

cpu2006_build_target_tools()
{
	export SKIPTOOLSRM=1
	if [ -z $SKIPMAKE ] && [ -z $SKIPXZ ] && \
	   [ -z $SKIPTAR ] && [ -z $SKIPMD5 ] && \
	   [ -z $SKIPSPECINVOKE ] && [ -z $SKIPRXP ] && \
	   [ -z $SKIPEXPAT ] && [ -z $SKIPPERL ]; then
		echo "Removing CPU2006 tools...\n"
		rm -rf ${CPU2006_BUILD}/tools/output
		rm -rf ${CPU2006_OUTPUT_ROOT}/tools/output
	fi
	if [ -z "$SKIPCOPY" ]; then
		rm -rf ${CPU2006_BUILD}/bin
		rm -rf ${CPU2006_OUTPUT_ROOT}/bin
	fi
	mkdir -p ${CPU2006_BUILD}/bin

	SPECBINFILES="configpp convert_to_development dumpsrcalt extract_config extract_flags extract_raw flag_dump flags_dump makesrcalt port_progress rawformat relocate runspec specdiff specpp toolsver verify_md5"
	SPECBINDIRS="fonts formats formatter modules.specpp scripts.misc test"
	cp -f ${CPU2006_ROOT}/bin/*.pl ${CPU2006_BUILD}/bin/
	cp -f ${CPU2006_ROOT}/bin/*.pm ${CPU2006_BUILD}/bin/
	for f in $SPECBINFILES; do
		cp -f ${CPU2006_ROOT}/bin/$f ${CPU2006_BUILD}/bin/
	done
	for d in $SPECBINDIRS; do
		cp -rf ${CPU2006_ROOT}/bin/$d ${CPU2006_BUILD}/bin/
	done
	cp -f ${CPU2006_ROOT}/bin/packagetools ${CPU2006_BUILD}/bin/
	cp -f ${CPU2006_ROOT}/bin/redistributable ${CPU2006_BUILD}/bin/
	cp -f ${CPU2006_ROOT}/bin/version.txt ${CPU2006_BUILD}/bin/
	cp -f $CPU2006_ROOT/shrc ${CPU2006_BUILD}/shrc

	${SCRIPT}/cpu2006/buildtools
}

if [ -z $ARCH ]; then
	export ARCH=riscv
fi
if [ -z $MACH ]; then
	export MACH=spike64
fi
if [ -z ${CROSS_COMPILE} ]; then
	export CROSS_COMPILE=riscv64-unknown-linux-gnu-
fi
if [ -z ${CPU2006_ROOT} ]; then
	export CPU2006_ROOT=${WORKING_DIR}/cpu2006
fi
if [ -z ${CPU2006_BUILD} ]; then
	export CPU2006_BUILD=${CPU2006_OUTPUT_ROOT}
	#export CPU2006_BUILD=${WORKING_DIR}/obj/cpu2006-${ARCH}
fi

cpu2006_init

while getopts "b:cd:m:rht" opt
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
	d) if [ "x$OPTARG" = "xall" ]; then
		CPU2006_DATA="test ref train"
	   else
		CPU2006_DATA=$OPTARG
	   fi;;
	m) CPU2006_METRICS=$OPTARG;;
	c) CPU2006_CLEAN_BENCHES=yes;;
	r) CPU2006_BUILD_BENCHES=yes;;
	h) CPU2006_BUILD_HOST_TOOLS=yes;;
	t) CPU2006_BUILD_TARGET_TOOLS=yes;;
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
if [ -z $CPU2006_BENCHMARKS ]; then
	CPU2006_BENCHMARKS=`cpu2006_benchmarks`
fi
CPU2006_COMMANDS=commands.txt
CPU2006_DIR=${TOP}/obj/bench${CPU2006_OUTPUT_ROOT}

cpu2006_patch_tools

if [ "x${CPU2006_BUILD_HOST_TOOLS}" = "xyes" ]; then
	(
	# Tune buildtools steps
	#export SKIPTOOLSRM=1
	#export SKIPCLEAN=1
	#export SKIPMAKE=1
	#export SKIPXZ=1
	#export SKIPTAR=1
	#export SKIPMD5=1
	#export SKIPSPECINVOKE=1
	#export SKIPRXP=1
	#export SKIPEXPAT=1
	#export SKIPPERL=1
	#export SKIPPERL2=1
	#export SKIPCOPY=1
	echo "CPU2006: Building host tools from $CPU2006_ROOT..."
	cpu2006_build_host_tools
	)
fi

if [ "x${CPU2006_BUILD_TARGET_TOOLS}" = "xyes" ]; then
	(
	# Tune buildtools steps
	#export SKIPTOOLSCP=1
	#export SKIPCLEAN=1
	#export SKIPMAKE=1
	#export SKIPXZ=1
	#export SKIPTAR=1
	#export SKIPMD5=1
	#export SKIPSPECINVOKE=1
	#export SKIPRXP=1
	#export SKIPEXPAT=1
	#export SKIPPERL=1
	#export SKIPCOPY=1
	echo "CPU2006: Building target tools from $CPU2006_ROOT..."
	cpu2006_build_target_tools
	)
fi
CPU2006_ARCHIVE=yes
CPU2006_OPTS="--config $ARCH"
if [ "x${CPU2006_METRICS}" = "xrate" ]; then
	CPU2006_OPTS="${CPU2006_OPTS} --rate ${CPU2006_COPIES}"
fi
if [ "x${CPU2006_CLEAN_BENCHES}" = "xyes" ]; then
	echo "CPU2006: Cleaning target tests from $CPU2006_ROOT..."
	cp -f $CPU2006_CONFIG $CPU2006_ROOT/config/$ARCH.cfg
	BENCHMARKS=(${CPU2006_BENCHMARKS})
	(
		cd $CPU2006_ROOT
		. ./shrc
		echo "============================================="
		echo "runspec $CPU2006_OPTS --action scrub $CPU2006_BENCHMARKS"
		echo "============================================="
		time runspec $CPU2006_OPTS --action scrub $CPU2006_BENCHMARKS
	)
	CPU2006_ARCHIVE=no
	rm -f $CPU2006_ROOT/config/$ARCH.cfg
else
	# TODO: verify if the benchmarks have already been built
	echo
fi
if [ "x${CPU2006_BUILD_BENCHES}" = "xyes" ]; then
	echo "CPU2006: Building target tests from ${CPU2006_ROOT}..."
	echo "Please make sure ${CPU2006_OUTPUT_ROOT} is writeable for current user $USER!"

	rm -rf ${CPU2006_OUTPUT_ROOT}/benchspec

	rm -f $CPU2006_ROOT/config/$ARCH.cfg*
	cp -f $CPU2006_CONFIG $CPU2006_ROOT/config/$ARCH.cfg
	BENCHMARKS=(${CPU2006_BENCHMARKS})
	(
		cd $CPU2006_ROOT
		. ./shrc
		for s in ${CPU2006_DATA}; do
			echo "============================================="
			echo "runspec $CPU2006_OPTS --action setup --size $s $CPU2006_BENCHMARKS"
			echo "============================================="
			time runspec $CPU2006_OPTS --action setup --size $s $CPU2006_BENCHMARKS
		done

		# Installing benbchmarks
		cd $CPU2006_OUTPUT_ROOT
		for b in ${CPU2006_BENCHMARKS}; do
			for s in ${CPU2006_DATA}; do
				CPU2006_RUNBASE=/benchspec/CPU2006/${b}/run/run_base_${s}_${ARCH}.0000
				CPU2006_HOSTDIR=${CPU2006_OUTPUT_ROOT}${CPU2006_RUNBASE}
				CPU2006_TARGETDIR=${CPU2006_DIR}${CPU2006_RUNBASE}
				if [ -d ${CPU2006_HOSTDIR} ]; then
				(
					echo "Generating ${b}.sh..."
					echo ${CPU2006_HOSTDIR}
					cd ${CPU2006_HOSTDIR}
					echo "#!/bin/sh" > ./${b}.sh
					specinvoke -nn >> ./${b}.sh
					chmod +x ./${b}.sh
					ls
				)
				fi
			done
		done
		mkdir -p ${CPU2006_OUTPUT_ROOT}/config
		cp -f $CPU2006_ROOT/config/$ARCH.cfg \
			${CPU2006_OUTPUT_ROOT}/config/$ARCH.cfg
	)
	CPU2006_ARCHIVE=yes
fi
if [ "x${CPU2006_ARCHIVE}" = "xyes" ]; then
	mkdir -p $CPU2006_DIR
	mkdir -p ${CPU2006_DIR}/config
	mkdir -p ${CPU2006_DIR}/Docs
	mkdir -p ${CPU2006_DIR}/tools/output

	# Copy files from CPU2006_ROOT
	echo "Installing cpu2006 host tools from $CPU2006_ROOT..."
	cp -rf $CPU2006_ROOT/config/flags ${CPU2006_DIR}/config/flags
	cp -rf $CPU2006_ROOT/Docs/flags ${CPU2006_DIR}/Docs/flags
	cp -rf $CPU2006_ROOT/Docs/sysinfo ${CPU2006_DIR}/Docs/sysinfo
	cp -f $CPU2006_ROOT/MANIFEST ${CPU2006_DIR}/MANIFEST
	cp -f $CPU2006_ROOT/version.txt ${CPU2006_DIR}/version.txt

	# Copy cross tests from CPU2006_OUTPUT_ROOT to overwrite host tools
	echo "Installing cpu2006 target tests from $CPU2006_OUTPUT_ROOT..."
	BENCHMARKS=(${CPU2006_BENCHMARKS})
	for b in $CPU2006_BENCHMARKS; do
		for s in ${CPU2006_DATA}; do
			CPU2006_RUNBASE=/benchspec/CPU2006/${b}/run/run_base_${s}_${ARCH}.0000
			CPU2006_HOSTDIR=${CPU2006_OUTPUT_ROOT}${CPU2006_RUNBASE}
			CPU2006_TARGETDIR=${CPU2006_DIR}${CPU2006_RUNBASE}
			# Install run directory
			if [ -d ${CPU2006_HOSTDIR} ]; then
				echo "Installing $b..."
				mkdir -p ${CPU2006_TARGETDIR}
				cp -rf ${CPU2006_HOSTDIR}/* ${CPU2006_TARGETDIR}/
				(
					cd ${CPU2006_TARGETDIR}
					cmds=`ls *.cmd`
					for f in $cmds; do
						echo "Installing ${CPU2006_TARGETDIR}/${f}..."
						sed -i "s%${CPU2006_ROOT}%${CPU2006_OUTPUT_ROOT}%g" ./$f
					done
					sed -i "s%${CPU2006_ROOT}%${CPU2006_OUTPUT_ROOT}%g" ./${b}.sh
				)
			fi
		done
	done
	cp -f $CPU2006_OUTPUT_ROOT/config/$ARCH.cfg ${CPU2006_DIR}/config/$ARCH.cfg

	# Copy cross toolsfrom CPU2006_OUTPUT_ROOT to overwrite host tools
	cp -f $CPU2006_BUILD/SUMS.tools ${CPU2006_DIR}/SUMS.tools
	cp -f $CPU2006_BUILD/shrc ${CPU2006_DIR}/shrc
	cp -rf $CPU2006_BUILD/bin ${CPU2006_DIR}/bin
	(
		cd ${CPU2006_DIR}/bin
		scps="convert_to_development specdiff flags_dump printpath.pl flag_dump extract_raw makesrcalt configpp rawformat dumpsrcalt printkids.pl specpp toolsver verify_md5 runspec extract_flags extract_config port_progress specperldoc"
		for f in $scps; do
			echo "Installing ${CPU2006_DIR}/bin/${f}..."
			sed -i "s%${CPU2006_ROOT}%${CPU2006_OUTPUT_ROOT}%g" ./$f
		done
	)
	mkdir -p ${CPU2006_DIR}/tools/output
	cp -rf $CPU2006_BUILD/tools/output/bin ${CPU2006_DIR}/tools/output/
	(
		cd ${CPU2006_DIR}/tools/output/bin
		scps="perldoc"
		for f in $scps; do
			echo "Installing ${CPU2006_DIR}/tools/output/bin/${f}..."
			sed -i "s%${CPU2006_ROOT}%${CPU2006_OUTPUT_ROOT}%g" ./$f
		done
	)
fi
