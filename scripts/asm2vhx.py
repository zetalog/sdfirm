#!/bin/python3.5

import sys

file_in_name = sys.argv[1]
file_out_name = sys.argv[2]
width = int(sys.argv[3])

if (width % 32) != 0:
        print('ERROR: Data width must be a multiple of 32 bits')

n = width//32
width_byte = widht//8

with open(file_in_name, 'r') as file_in:
    addr = []
    opcode_ori = []
    for line in file_in.readlines():
        sign_1 = line.find(':\t')
        sign_2 = line.find(' \t')
        if (sign_1 != -1) and (sign_2 != -1):
            addr.append(int(line[0:sign_1].strip(' '), 16))
            opcode_ori.append(line[(sign_1+2):sign_2].strip(' '))

loop = 1
addr_head = addr[0]
label = 1
opcode_rearrange = []
while loop == 1:
    if addr_head > max(addr):
        break
    else:
        i = 0
        instr_data = []
        instr_valid = []
        while i < n:
            if (addr_head + i * 4) in addr:
                instr_data.append(opcode_ori[addr.index(addr_head + i * 4)])
                instr_valid.append(1)
            else:
                instr_data.append('00000000')
                instr_valid.append(0)
            i += 1
        if instr_valid.count(0) == n:
            label = 1
        else:
            if label == 1:
                if addr_head == 0:
                    opcode_rearrange.append('@0' + '\n')
                else:
                    opcode_rearrange.append('@' + hex(addr_head//width_byte[2:] + '\n')
            label = 0
            j = n
            rearrange_line = ''
            while j > 0:
                rearrange_line = rearrange_line + instr_data[j - 1]
                j -= 1
            opcode_rearrange.append(rearrange_line + '\n')
    addr_head = addr_head + width_byte

with open(file_out_name, 'w') as file_out:
    for i in opcode_rearrange:
        print(i, file = file_out, end = '')
