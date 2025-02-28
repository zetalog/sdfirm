#!/bin/bash

SOC_NAME=mysoc

SCRIPT=`(cd \`dirname $0\`; pwd)`
#target path
ROOT_PATH=/home/$(whoami)/snode-mt/$(whoami)/${SOC_NAME}
#ROOT_PATH=/home/$(whoami)/test
DATE=`date +%Y%m%d%H%M`
HEX_PATH=${ROOT_PATH}/${DATE}
FPGA_PATH=/snode-mt/$(whoami)/${SOC_NAME}/${DATE}
#source path
BIN_NAME=${SOC_NAME}
SRC_BIN=${SCRIPT}/../../obj/sdfirm-riscv/arch/riscv/boot/sdfirm.bin
BIN_HEX=${HEX_PATH}/${BIN_NAME}.txt
#target script
MEM_DOWNLOAD=${HEX_PATH}/download.sh
#address, UNIT
ADDR_BIN=0x1000

mkdir -p ${HEX_PATH}

#function 
adjust_file_length() {
	local file="$1"

	if [ ! -f "$file" ]; then
		echo "'$file' is not exist"
		return 0
	fi

	local file_length
	file_length=$(stat -c%s "$file")

	local remainder
	remainder=$((file_length % 16))

	local adjusted_length
	if [ $remainder -ne 0 ]; then
		adjusted_length=$((file_length + (16 - remainder)))
	else
		adjusted_length=$file_length
	fi

	echo "$adjusted_length"
}

echo "====== target path in FPGA server: ${FPGA_PATH}"

python3 binary_to_text.py ${SRC_BIN} ${BIN_HEX}
f_len_bin=$(adjust_file_length "${SRC_BIN}")

hex_addr_bin=$(echo "obase=16; $((ADDR_BIN / 16))" | bc)

echo "memory download -instance asic_top.u_ap_top.u_mesh_sub.axi4_sm -file ${FPGA_PATH}/${BIN_NAME}.txt -address 0x$hex_addr_bin -count ${f_len_bin}
" > ${MEM_DOWNLOAD}
