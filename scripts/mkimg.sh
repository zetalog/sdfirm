#!/bin/bash

# Usage:
#   mkimg.sh input-root-dir output-image-file
# Default:
#   mkimg.sh ./img-root ./gpt.img

SCRIPT=`(cd \`dirname $0\`; pwd)`
#
# User parameter
# 
input_dir=${SCRIPT}/img-root
output_img=${SCRIPT}/gpt.img
if [[ $# -gt 0 ]]
then
	input_dir=$1
fi
if [[ $# -gt 1 ]]
then
	output_img=$2
fi

img_size_kbyte=4096
let img_size=$img_size_kbyte*1024

#
# Macro defination
#
BLOCK_SIZE=1024
PAD_BYTES=2
GUID_PREFIX="73646669-726D-6470-7500-0000"

let block_cnt=$img_size/$BLOCK_SIZE
part_align=$BLOCK_SIZE

if [[ ! -d $input_dir ]]
then
	echo "Error: Input directory $input_dir not exists."
	exit 0
fi
if [[ -f $output_img ]]
then
	echo "Error: Output file $output_img exists. Remove it first!"
	exit 0
fi

#
# Get a list of  input files
#
cd $input_dir
input_list=`find . -maxdepth 1 -type f | cut -d / -f 2`
cd -
echo "============================================================"
echo "Creating GPT Image..."
echo "input directory: $input_dir"
echo "input files: $input_list"
echo "output file: $output_img"
echo "image size: $img_size"
echo "block count: $block_cnt"

#
# Create an empty image
#
dd if=/dev/zero of=$output_img bs=$BLOCK_SIZE count=$block_cnt > /dev/null
echo "------------------------------------------------------------"
echo "Initializing GPT table..."
sgdisk --clear $output_img > /dev/null

part_num=1
for f in $input_list
do
	file_size=`wc --bytes $input_dir/$f | cut -d " " -f 1`
	let part_size=($file_size+$part_align-1)/$part_align*$part_align
	let part_size_kbyte=$part_size/1024
	let pad_size=$part_size-$file_size
	pad_size_str=`printf %04x $pad_size`
	part_num_str=`printf %04x $part_num`
	part_guid="$GUID_PREFIX""$part_num_str""$pad_size_str"
	part_name=$f
	echo "------------------------------------------------------------"
	echo "Add file GPT partition..."
	echo "partition${part_num}: $part_num"
	echo "UUID: $part_guid"
	echo "file size: $file_size"
	echo "pad size: $pad_size(0x$pad_size_str)"
	echo "partition size: $part_size bytes, $part_size_kbyte KB"
	sgdisk --new 0:0:+${part_size_kbyte}KB \
		--change-name 0:$part_name \
		--partition-guid 0:$part_guid $output_img > /dev/null
	let part_num+=1
done
# One more partition for all remaining space
echo "------------------------------------------------------------"
echo "Adding global GPT partition..."
sgdisk --new 0:0:-0 $output_img > /dev/null

echo "------------------------------------------------------------"
echo "GPT partition information:"
parted $output_img unit B print

#
# Copy file data in
#
part_num=1
for f in $input_list
do
	file_size=`wc --bytes $input_dir/$f | cut -d " " -f 1`
	part_info=`parted $output_img unit B print 2> /dev/null | sed '1,/Number/d' | head -n $part_num | tail -n 1`
	part_start=`echo $part_info tr -s ' ' | cut -d ' ' -f 2 | cut -d 'B' -f 1`
	let dd_seek=$part_start
	let part_end=$dd_seek+$file_size
	echo "------------------------------------------------------------"
	echo "Copying file..."
	echo "partition$part_num: $f"
	echo "seek: $dd_seek"
	echo "start: $part_start"
	echo "end: $part_end"
	echo "size: $file_size"
	dd if=$input_dir/$f of=$output_img bs=1 count=$file_size \
		seek=$dd_seek conv=notrunc > /dev/null
	let part_num+=1
done

echo "------------------------------------------------------------"
echo "Final partition information:"
fdisk -l $output_img
echo "============================================================"
