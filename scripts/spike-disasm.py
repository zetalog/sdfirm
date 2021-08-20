#!/usr/bin/python3

import sys
import re

dis_file = sys.argv[1]
input_file = sys.argv[2]
output_file = sys.argv[3]

with open (dis_file, 'r') as disassembly:
    dict_label = {}
    for line in disassembly.readlines():
        line.strip()
        dis_line_info = re.search(r'^([0-9a-f]+) <([\._0-9a-zA-Z]+)>:', line)
        if (dis_line_info != None):
            dis_pc = int(dis_line_info.group(1), 16)
            dis_label = dis_line_info.group(2)
            dict_label[dis_pc] = dis_label

#for key in dict_label.keys():
#    print(hex(key) + '- ' + dict_label[key])

trace_with_label = []
with open (input_file, 'r') as trace_without_label:
    for line in trace_without_label.readlines():
        spike_inst_info = re.search(r'^0x([0-9a-f]+) \(0x[0-9a-f]+\) ', line)
        if (spike_inst_info != None):
            spike_pc_str = spike_inst_info.group(1)
            if (spike_pc_str == ''):
                spike_pc_str = '-1'
            spike_pc = int(spike_pc_str, 16)
            if spike_pc in dict_label:
                trace_with_label.append('***** ' + dict_label[spike_pc] + ' *****\n' + line)
            else:
                trace_with_label.append(line)

with open(output_file, 'w') as trace_output:
    for entry in trace_with_label:
            print(entry, file=trace_output, end='')
