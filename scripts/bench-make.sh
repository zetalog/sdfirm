#!/bin/bash

#
# Directory
#
SDFIRM_DIR=.

#
# Target platform
#
sub_arch=64
arch=riscv${sub_arch}
base_config=${SDFIRM_DIR}/arch/riscv/configs/bench_rv${sub_arch}_defconfig

#
# Bench configuration
#
bench_list="hanoitower md5"

config_key_0="RISCV_SV39"
config_ext_0="RISCV_SV48"

config_key_1="RISCV_M=y"
config_ext_1="RISCV_M=n"

config_key_2="SMP=y"
config_ext_2="SMP=n"

config_key_3="SPINLOCK_RAW"
config_ext_3="SPINLOCK_GENERIC"

#
# Function
#
function extend_config()
{
	if [ $# -lt 2 ]
	then
		echo "Error: Invalid parameter for extend_config"
		exit 1
	fi
	key=$1
	ext_list=$@
	echo "Extend config for" $key ":" $ext_list
	curr_config=`ls config-*`
	for f in $curr_config;
	do
		for ext in $ext_list;
		do
			tag=`echo $ext | awk -F "_" '{print $NF}'`
			tag=${tag/=/}
			new_config=$f-$tag
			key_del="$ext is"
			echo "Generate config" $new_config
			cp -f $f .config
			sed -i s/$key/$ext/ .config
			sed /$key_del/d .config
			make oldconfig
			mv .config $new_config
			diff -ru $base_config $new_config
		done
		echo "Drop original config" $f
		rm -f $f
	done
}

#
# Main process
#
echo "================================"
echo "    Prepare..."
echo "================================"

batch_command=""
for bench in $bench_list; do
	batch_command+="bench sync all "$bench" 1 2 1; "
done

echo "Batch commands =" $batch_command
if [ -f $base_config ]
then
	echo "Base config =" $base_config
else
	echo "Error: Base config not found" $base_config
	exit -1
fi

echo "Clean up..."
rm -f config-*
rm -f sdfirm-*.elf*

echo "================================"
echo "    Generate configs..."
echo "================================"

cd $SDFIRM_DIR
echo "Update base config..."
cp $base_config .config
make oldconfig >> /dev/null
sed -i s/help/"$batch_command"/ .config
cp .config ./config-rv${sub_arch}
echo "Generate various config..."
extend_config $config_key_0 $config_ext_0
#extend_config $config_key_1 $config_ext_1
#extend_config $config_key_2 $config_ext_2
#extend_config $config_key_3 $config_ext_3
echo "New config:"
ls -l ./config-*

echo "================================"
echo "    Build ELFs..."
echo "================================"

cd $SDFIRM_DIR
for f in `ls config-*`;
do
	time_stamp=`date +%Y%m%d.%H%M%S`
	prefix=${f/config/sdfirm}
	elf_file=${prefix}-$time_stamp.elf
	config_file=${elf_file}.config
	echo "Build " $elf_file 
	cp $f .config
	make clean > /dev/null
	make > /dev/null
	mv sdfirm $elf_file
done
echo "Results:"
ls -l sdfirm-*.elf

