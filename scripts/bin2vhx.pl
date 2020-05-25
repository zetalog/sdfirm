#!/usr/bin/perl -w
#
# USAGE:
#   bin2vhx.pl <input_file> <output_file> [--be | --le] [--width=<data_width>
#
# DESCRIPTION:
#   Converts a binary image into a Verilog memory initialization file that can
#   be read using $readmemh.
#   Where
#     --le:    Image is in little-endian format (this is the default.)
#     --be:    Image is in big-endian format
#     --width: Width in bits of the target memory array, default 32
#   The memory width must be multiple of 8, i.e. a while number of bytes.

use strict;

my $infile;
my $outfile;
my $width = 32;
my $bigendian = 0;

# Other variables
my $bytes_per_row;
my $byte;
my @row = ();
my $result;

# parse arguments
foreach (@ARGV)
{
	if (/^--((\w+)(=(\d+))?)/) {
		if (lc($2) eq "be") {
			$bigendian = 1;
		} elsif (lc($2) eq "le") {
			$bigendian = 0;
		} elsif (lc($2) eq "width" and defined($4)) {
			$width = $4;
		} else {
			die("ERROR: invalid argument: $1\n");
		}
	} else {
		if (!defined($infile)) {
			$infile = $_;
		} elsif (!defined($outfile)) {
			$outfile = $_;
		} else {
			die("ERROR: too many arguments: $_\n");
		}
	}
}

die("ERROR: width must be a multiple of 8 bits\n") if (($width % 8) != 0);
die("ERROR: no input file\n") if (!defined($infile));
die("ERROR: no output file\n") if (!defined($outfile));

open(INFILE, "$infile") or die("ERROR: couldn't read file: $infile\n");
open(OUTFILE, ">$outfile") or die ("ERROR: couldn't write file: $outfile\n");

binmode(INFILE);
$bytes_per_row = $width / 8;

while () {
	$result = read(INFILE, $byte, 1);
	die("ERROR: couldn't read file: $infile\n") if ($result < 0);
	last if ($result == 0);
	if ($bigendian) {
		push(@row, sprintf("%.2x", unpack("C*", $byte)));
	} else {
		unshift(@row, sprintf("%.2x", unpack("C*", $byte)));
	}
	if ($#row == $bytes_per_row - 1) {
		print OUTFILE @row;
		print OUTFILE "\n";
		@row = ();
	}
}
if ($#row > -1) {
	if ($bigendian) {
		push(@row, "00") while ($#row < $bytes_per_row - 1);
	} else {
		unshift(@row, "00") while ($#row < $bytes_per_row - 1);
	}
	print OUTFILE @row;
}

close(INFILE);
close(OUTFILE);
