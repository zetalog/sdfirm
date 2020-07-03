#!/usr/bin/python3

import sys

group_size = 4

if (len(sys.argv) < 2):
	print("Usage:")
	print("\t" + sys.argv[0] + " input-file [group-size]")
	print("\tgroup-size: default = " + str(group_size))
	sys.exit(1)
if (len(sys.argv) >= 3):
	group_size = int(sys.argv[2])
	if (group_size < 0 or group_size > 128):
		print("Invalid group_size = " + sys.argv[2])
		sys.exit(1)

input_filename = sys.argv[1]
output_filename = input_filename + "-swap" + str(group_size) + "bytes"

try:
	input_fd = open(input_filename, "rb")
except IOError:
	printf("Error: Failed to open input file " + input_filename)
	sys.exit(1)
try:
	output_fd = open(output_filename, "wb")
except IOError:
	printf("Error: Failed to open output file " + output_filename)
	sys.exit(1)

byte_list = list(input_fd.read())
input_fd.close()

input_byte_cnt = len(byte_list)
group_cnt = int((input_byte_cnt + group_size -1) / group_size)
append_byte_cnt = group_cnt * group_size - input_byte_cnt
output_byte_cnt = input_byte_cnt + append_byte_cnt

print("Input size  = " + str(input_byte_cnt))
print("Group size  = " + str(group_size))
print("Append size = " + str(append_byte_cnt))
print("Output size = " + str(output_byte_cnt))

for i in range(append_byte_cnt):
	byte_list.append(0)
i = 0
while (i < output_byte_cnt):
	for j in range(int(group_size / 2)):
		tmp = byte_list[i + j]
		byte_list[i + j] = byte_list[i + group_size -1 - j]
		byte_list[i + group_size -1 - j] = tmp
	i += group_size

output_fd.write(bytearray(byte_list))
output_fd.close()

print("Done")
