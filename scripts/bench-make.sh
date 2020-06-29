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
base_config=${SDFIRM_DIR}/arch/riscv/configs/vaisra_nocon_defconfig

# Debug option
debug_config_only="y"
debug_config_only="n"

#
# Bench configuration
#
bench_list="hanoitower md5"
bench_list="cfft"

bench_mode="single"
#bench_mode="batch"
#bench_mode="all"

config_key_0="RISCV_SV39"
config_ext_0="RISCV_SV48"

config_key_1="RISCV_M=y"
config_ext_1="RISCV_M=n"

#config_key_2="SMP=y"
#config_ext_2="SMP=n"

#config_key_3="SPINLOCK_RAW"
#config_ext_3="SPINLOCK_GENERIC"

#config_key_4=""
#config_ext_4=""
#config_key_5=""
#config_ext_5=""
#config_key_6=""
#config_ext_6=""
#config_key_7=""
#config_ext_7=""

#
# Function
#

# Single Mode:
# - Each test in a singel ELF
# - Memory size is reduced
function init_config_single()
{
	for bench in $bench_list; do
		batch_command="bench sync all "$bench" 1 2 1"
		cp $base_config .config
		config_off=${bench^^}"=n"
		config_on=${bench^^}"=y"
		sed -i s/$config_off/$config_on/ .config
		make oldconfig >> /dev/null
		sed -i s/help/"$batch_command"/g .config
		#sed -i s/MEM1_SIZE=0x2000000/MEM1_SIZE=0x1600000/ .config
		cp .config ./config-rv${sub_arch}-${bench}
	done
}

# Batch Mode
# - All tests as a batch of commands in one ELF
function init_config_batch()
{
	batch_command=""
	for bench in $bench_list; do
		batch_command+="bench sync all "$bench" 1 2 1; "
	done
	echo "Batch commands =" $batch_command
	cp $base_config .config
	for bench in $bench_list; do
		config_off=${bench^^}"=n"
		config_on=${bench^^}"=y"
		sed -i s/$config_off/$config_on/ .config
	done
	make oldconfig >> /dev/null
	sed -i s/help/"$batch_command"/ .config
	cp .config ./config-rv${sub_arch}-batch
}

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
			other_replace_src="$ext is not"
			other_replace_dst="$key is not"
			echo "Generate config" $new_config
			cp -f $f .config
			sed -i s/"$key"/"$ext"/ .config
			sed -i s/"$other_replace_src"/"$other_replace_dst"/ .config
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

if [ -f $base_config ]
then
	echo "Base config =" $base_config
else
	echo "Error: Base config not found" $base_config
	exit -1
fi
echo "Bench mode =" $bench_mode

echo "Clean up..."
rm -f config-*
rm -f sdfirm-*.elf*

echo "================================"
echo "    Generate configs..."
echo "================================"

cd $SDFIRM_DIR
if [ "$bench_mode" == "batch" ]
then
	echo "Create base config (Batch Mode)..."
	init_config_batch
elif [ "$bench_mode" == "single" ]
then
	echo "Create base config (Single Mode)..."
	init_config_single
elif [ "$bench_mode" == "all" ]
then
	echo "Create base config (both Single and Batch Modes)..."
	init_config_batch
	init_config_single
else
	echo "Error: Invalid bench Mode" $bench_mode
	exit 1
fi

echo "Generate various config..."
if [ ! -z $config_key_0 ]
then
	extend_config $config_key_0 $config_ext_0
fi
if [ ! -z $config_key_1 ]
then
	extend_config $config_key_1 $config_ext_1
fi
if [ ! -z $config_key_2 ]
then
	extend_config $config_key_2 $config_ext_2
fi
if [ ! -z $config_key_3 ]
then
	extend_config $config_key_3 $config_ext_3
fi
if [ ! -z $config_key_4 ]
then
	extend_config $config_key_4 $config_ext_4
fi
if [ ! -z $config_key_5 ]
then
	extend_config $config_key_5 $config_ext_5
fi
if [ ! -z $config_key_6 ]
then
	extend_config $config_key_6 $config_ext_6
fi
if [ ! -z $config_key_7 ]
then
	extend_config $config_key_7 $config_ext_7
fi

echo "New config:"
ls -l ./config-*

if [ "$debug_config_only" == "y" ]
then
	echo "Generate configs only"
	exit 0
fi

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

