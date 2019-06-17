#!/bin/sh

SCRIPT=`(cd \`dirname $0\`; pwd)`
GEM5_SRC=~/workspace/gem5
GEM5_FLG=

while getopts "ds:" opt
do
	case $opt in
	d) GEM5_FLG=--debug-flags=Exec;;
	s) GEM5_SRC=$OPTARG;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

SDFIRM_SRC=`(cd ${SCRIPT}/..; pwd)`
GEM5_SRC=`(cd ${GEM5_SRC}; pwd)`

cp ${SDFIRM_SRC}/sdfirm.strip ${GEM5_SRC}/fs_images/arm/binaries/boot_emm.arm64

(
	cd ${GEM5_SRC}
	export M5_PATH="${GEM5_SRC}/fs_images/arm/:${GEM5_SRC}"

	./build/ARM/gem5.opt ${GEM5_FLG} \
		configs/example/fs.py \
		--kernel=vmlinux.aarch64.20140821 \
		--machine-type=VExpress_EMM64 \
		--dtb-file=vexpress.aarch64.20140821.dtb \
		--disk-image=linaro-minimal-aarch64.img
)
