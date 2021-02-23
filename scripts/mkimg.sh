#!/bin/bash
#
# Usage:
#   mkimg.sh [input-dir] [output-file]
# Default:
#   mkimg.sh ./img-root ./gpt.img

SCRIPT=`(cd \`dirname $0\`; pwd)`
MKIMG_PRINT=yes
MKIMG_INPUT=${SCRIPT}/img-root
MKIMG_OUTPUT=${SCRIPT}/gpt.img
MKIMG_BLKSZ=1024
MKIMG_PADSZ=2
MKIMG_KBYTES=32768
MKIMG_GUID="73646669-726D-6470-7500-0000"

usage()
{
	echo "Usage:"
	echo "`basename $0` [-p] [input] [output] [capacity-KB]"
	echo "Where:"
	echo " -p:     print partition information"
	echo " input:  specify input binary directory"
	echo " output: specify output image file"
	echo " capacity: specify storage capacity in KB(1024B). Default 32768KB (32MB)"
	exit $1
}

fatal_usage()
{
	echo $1
	usage 1
}

while getopts "p" opt
do
	case $opt in
	p) MKIMG_PRINT=yes;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

if [[ $# -gt 0 ]]
then
	MKIMG_INPUT=$1
fi
if [[ $# -gt 1 ]]
then
	MKIMG_OUTPUT=$2
fi
if [[ $# -gt 2 ]]
then
	MKIMG_KBYTES=$3
fi

let img_size=${MKIMG_KBYTES}*1024
let block_cnt=${img_size}/${MKIMG_BLKSZ}
part_align=${MKIMG_BLKSZ}

if [[ ! -d ${MKIMG_INPUT} ]]
then
	echo "Error: Input directory ${MKIMG_INPUT} not exists."
	usage 1
fi
if [[ -f ${MKIMG_OUTPUT} ]]
then
	echo "Error: Output file ${MKIMG_OUTPUT} exists. Remove it first!"
	usage 1
fi

# Get a list of  input files
cd ${MKIMG_INPUT}
input_list=`find . -maxdepth 1 -type f | cut -d / -f 2 | sort`
cd -
echo "============================================================"
echo "Creating GPT Image..."
echo "input directory: ${MKIMG_INPUT}"
echo "input files:"
echo "${input_list}"
echo "output file: ${MKIMG_OUTPUT}"
echo "image size: ${img_size}"
echo "block count: ${block_cnt}"

# Create an empty image
dd if=/dev/zero of=${MKIMG_OUTPUT} bs=${MKIMG_BLKSZ} \
	count=${block_cnt}
echo "------------------------------------------------------------"
echo "Initializing GPT table..."
sgdisk --clear ${MKIMG_OUTPUT}

part_num=1
for f in ${input_list}
do
	file_size=`wc --bytes ${MKIMG_INPUT}/${f} | cut -d " " -f 1`
	let part_size=(${file_size}+${part_align}-1)/${part_align}*${part_align}
	let part_size_kbyte=${part_size}/1024
	let pad_size=${part_size}-${file_size}
	pad_size_str=`printf %04x ${pad_size}`
	part_num_str=`printf %04x ${part_num}`
	part_guid="${MKIMG_GUID}""${part_num_str}""${pad_size_str}"
	echo "------------------------------------------------------------"
	echo "Add file GPT partition..."
	echo "partition${part_num}: ${f}"
	echo "UUID: ${part_guid}"
	echo "file size: ${file_size}"
	echo "pad size: ${pad_size}(0x${pad_size_str})"
	echo "partition size: ${part_size}bytes, ${part_size_kbyte}KB"
	sgdisk --new 0:0:+${part_size_kbyte}KB \
		--change-name 0:${f} --partition-guid 0:${part_guid} \
		${MKIMG_OUTPUT}
	let part_num+=1
done
# One more partition for all remaining space
echo "------------------------------------------------------------"
echo "Adding global GPT partition..."
sgdisk --new 0:0:-0 ${MKIMG_OUTPUT}

if [ "x${MKIMG_PRINT}" = "xyes" ]; then
	echo "------------------------------------------------------------"
	echo "GPT partition information:"
	parted ${MKIMG_OUTPUT} unit B print
fi

#
# Copy file data in
#
part_num=1
for f in ${input_list}
do
	file_size=`wc --bytes ${MKIMG_INPUT}/${f} | cut -d " " -f 1`
	part_info=`parted ${MKIMG_OUTPUT} unit B print 2>/dev/null | sed '1,/Number/d' | head -n ${part_num} | tail -n 1`
	part_start=`echo ${part_info} tr -s ' ' | cut -d ' ' -f 2 | cut -d 'B' -f 1`
	let dd_seek=${part_start}
	let part_end=${dd_seek}+${file_size}
	echo "------------------------------------------------------------"
	echo "Copying file..."
	echo "partition${part_num}: ${f}"
	echo "seek: ${dd_seek}"
	echo "start: ${part_start}"
	echo "end: ${part_end}"
	echo "size: ${file_size}"
	dd if=${MKIMG_INPUT}/${f} of=${MKIMG_OUTPUT} bs=1 count=${file_size} \
		seek=${dd_seek} conv=notrunc
	let part_num+=1
done

if [ "x${MKIMG_PRINT}" = "xyes" ]; then
	echo "------------------------------------------------------------"
	echo "Final partition information:"
	fdisk -l ${MKIMG_OUTPUT}
fi
echo "============================================================"
