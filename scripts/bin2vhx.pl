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
#   The memory width must be multiple of 32, i.e. a while number of words.

use strict;

my $infile;
my $outfile;
my $width = 32;
my $bigendian = 0;

# Other variables
my $words_per_row;
my $unpack_template;
my $word;
my @row = ();

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

die("ERROR: width must be a multiple of 32 bits\n") if (($width % 32) != 0);
die("ERROR: no input file\n") if (!defined($infile));
die("ERROR: no output file\n") if (!defined($outfile));

open(INFILE, "$infile") or die("ERROR: couldn't read file: $infile\n");
open(OUTFILE, ">$outfile") or die ("ERROR: couldn't write file: $outfile\n");

binmode(INFILE);
$words_per_row = $width / 32;
$unpack_template = $bigendian ? "N" : "V";

while (read(INFILE, $word, 4)) {
	unshift(@row, sprintf("%.8x", unpack($unpack_template, $word)));
	if ($#row == $words_per_row - 1) {
		print OUTFILE @row;
		print OUTFILE "\n";
		@row = ();
	}
}

if ($#row > -1) {
	unshift(@row, "00000000") while ($#row < $words_per_row - 1);
	print OUTFILE @row;
}

close(INFILE);
close(OUTFILE);
