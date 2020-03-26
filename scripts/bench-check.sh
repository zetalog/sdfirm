#!/bin/bash

SPIKE_DIR=../riscv-isa-sim/build
spike=${SPIKE_DIR}/spike
sub_arch=64
arch=riscv${sub_arch}
spike_isa=RV${sub_arch}IMAFD # Without C (compressed) extension
dump=${arch}-unknown-elf-objdump

key_pass="_t_pass"
key_fail="_t_fail"

function verify_elf()
{
	elf_file=$1
	dump_file=${elf_file}.S
	log_file=${elf_file}.log
	report_file=${elf_file}.txt
	> $report_file

	echo "--------------------------------"
	echo "Verify" $elf_file
	echo "--------------------------------"

	echo "Getting PCs of exit for each tests"
	grep_keys="-e ${key_pass}>: -e ${key_fail}>:"
	$dump -D $elf_file > $dump_file
	pc_pass=`grep -e $key_pass $dump_file | cut -d ' ' -f 1`
	pc_fail=`grep -e $key_fail $dump_file | cut -d ' ' -f 1`
	echo "PCs-Pass" $pc_pass
	echo "PCs-Fail" $pc_fail

	tag_smp_off="SMPn"
	if [[ "$elf_file" == *"$tag_smp_off"* ]]
	then
		spike_smp=1
	else
		spike_smp=4
	fi
	echo "Running over Spike with --isa =" ${spike_isa} "-p"${spike_smp}
	#$spike --isa=${spike_isa} -p$spike_smp -l $elf_file 2> $log_file
	$spike --isa=${spike_isa} -p$spike_smp -g $elf_file 2> $log_file

	echo "Checking Spike log"
	echo "--------------------------------" >> $report_file
	echo "Checking result of $elf_file" >> $report_file
	echo "--------------------------------" >> $report_file
	for pc in $pc_pass;
	do
		if [ ${#pc} -eq 16 ]
		then
			pc=`echo $pc | cut -b 9-16`
		fi
		grep $pc $log_file > /dev/null
		if [ $? -eq 0 ]
		then
			echo "Pass PC" $pc >> $report_file
		else
			echo "No Pass PC" $pc >> $report_file
		fi
	done
	for pc in $pc_fail;
	do
		if [ ${#pc} -eq 16 ]
		then
			pc=`echo $pc | cut -b 9-16`
		fi
		grep $pc $log_file
		if [ $? -eq 0 ]
		then
			echo "Fail PC" $pc >> $report_file
		fi
	done
	echo "" >> $report_file
}

echo "================================"
echo "    Verify over Spike"
echo "================================"

if [ $# -eq 1 ]
then
	elf=$1
	verify_elf $elf
	exit 0
fi

for elf in `ls sdfirm-*.elf`;
do
	verify_elf $elf
done

echo "================================"
echo "    Show checking result"
echo "================================"

for elf in `ls sdfirm-*.elf`;
do
	cat $elf.txt
done


