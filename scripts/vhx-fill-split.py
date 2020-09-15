#!/usr/bin/python3

import sys
import os
import getopt

LINE_WIDTH_DEF = 32
LINE_COUNT_DEF = (1024*1024)
SPLIT_COUNT_DEF = 2
GROUP_SIZE_DEF = 2

def usage():
	print("vhx-fill-split.py -i input-hex [-w width-in-byte] [-l line-count] [-g group-size] [-s split-count]")
	print("  -w width-in-byte")
	print("     Data size in byte per hex line. Default:", str(LINE_WIDTH_DEF))
	print("  -l line-count")
	print("     Total line count after filling. Default:", str(LINE_COUNT_DEF))
	print("  -w group-size")
	print("     Line count per group in splitting. Default:", str(GROUP_SIZE_DEF))
	print("  -s split-count")
	print("     How many banks to split. Default:", str(SPLIT_COUNT_DEF))

def output_to_file(output_fd, output_line):
	output_fd.writelines([output_line + "\n"])

def main():
	input_file = ""
	output_file_list = []
	output_fd_list = []
	line_width = LINE_WIDTH_DEF
	line_count = LINE_COUNT_DEF
	group_size = GROUP_SIZE_DEF
	split_count = SPLIT_COUNT_DEF
	i_line_input = 0
	i_line_filled = 0

	try:
		opts, args = getopt.getopt(sys.argv[1:], "hi:l:g:s:w:")
	except getopt.GetoptError as err:
		print(err)
		usage()
		sys.exit(2)
	for o, a in opts:
		if o in ("-h", "--help"):
			usage()
			sys.exit()
		elif o == "-i":
			input_file = a
		elif o == "-w":
			line_width = int(a)
		elif o == "-l":
			line_count = int(a)
		elif o == "-g":
			group_size = int(a)
		elif o == "-s":
			split_count = int(a)
	if (len(input_file) <= 0):
		usage()
		sys.exit(2)
	
	# Open input file
	try:
		input_fd = open(input_file, 'r')
	except IOError:
		print("Error: Failed to open input ", input_file)
		sys.exit(2)
	print("Input file:", input_file)

	print("Line width in byte:", str(line_width))
	print("Line count final:", str(line_count))
	print("Lines per group:", str(group_size))
	
	# open output files
	for i in range(0, split_count):
		output_file_list.append(input_file + "-" + str(i))
		try:
			output_fd = open(output_file_list[i], 'w')
		except IOError:
			print("Error: Failed to open output ", output_file_list[i])
			sys.exit(2)
		output_fd_list.append(output_fd)
		print("Output file:", output_file_list[i])

	# Input origin hex file, fill zero lines if necessary and split to multiple files
	line_width_char = line_width * 2
	zero_line = ""
	for i in range(0, line_width_char):
		zero_line += "0"
	while (True):
		input_line = input_fd.readline().strip('\n')
		if (not input_line): # End of file
			print("End of input")
			break

		if (len(input_line) < 8): 	# Invliad line
			print("Invalid line " + str(i_line_input) + " " + input_line)
			break
		if (input_line[0] == '@'): 	# Offset
			new_offset = int(("0x" + input_line[1:]), 16)
			print("New offset " + hex(new_offset))
			while (i_line_filled < new_offset):
				i_output = int(i_line_filled / group_size) % split_count
				output_to_file(output_fd_list[i_output], zero_line)
				i_line_filled += 1
		elif (len(input_line) != line_width_char): # Invalid line
			print("Invalid line " + str(i_line_input) + " " + input_line)
			break
		else:						# Valid data line
			i_output = int(i_line_filled / group_size) % split_count
			output_to_file(output_fd_list[i_output], input_line)
			i_line_filled += 1
		i_line_input += 1

	# Append zero lines if necessary
	while (i_line_filled < line_count):
		i_output = int(i_line_filled / group_size) % split_count
		output_to_file(output_fd_list[i_output], zero_line)
		i_line_filled += 1
	
	# Clean up
	input_fd.close()
	for fd in output_fd_list:
		fd.close()

if __name__ == "__main__":
	main()
