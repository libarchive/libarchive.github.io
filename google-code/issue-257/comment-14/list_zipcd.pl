#!/usr/bin/perl -w

use strict;

my ($f, $buf) = ();
my ($magic, $magic2) = ();

if ($#ARGV == -1) {
	$f = "STDIN";
} elsif ($#ARGV == 0) {
	open($f, "< $ARGV[0]") or die "Can't read file $ARGV[0]\n";
} else {
	print "use: checkzip_cd [file.zip]\n";
	exit 0;
}

# SFX zip doesn't have signature at the beginning of file
##read $f, $buf, 4;
##$magic = unpack("V", $buf);
##die "Not a ZIP\n" if $magic != 0x4034b50; # PK\03\04

seek $f, -22, 2 or die "File is too small\n";
my $eocd_start = tell($f);
read $f, $buf, 4+2+2 or die "Cannot read EOCD header\n";

# XXX $magic2 isn't really a magic, but limitation of zip reader
($magic, $magic2) = unpack("VV", $buf);
if ($magic != 0x6054b50 or $magic2 != 0 ) { # PK\05\06\0\0\0\0
	my $eocd_search_size = 0;
	$eocd_search_size = 32 * 1024 if $eocd_start > 32 * 1024;

	seek $f, -1 * $eocd_search_size, 2 or die "Cannot seek\n";
	$eocd_start = tell($f);
	read $f, $buf, $eocd_search_size;

	if ($buf =~ /PK\05\06\0\0\0\0/) {
		$eocd_start+= $-[0];
	} else {
		die "Cannot find EOCD header\n";
	}
}

seek $f, $eocd_start+4+4, 0 or die "Cannot seek to EOCD header\n";
read $f, $buf, 2+2+4+4;

my ($cd_record_num, undef, $cd_size, $cd_start) = unpack("vvVV", $buf);
#my $adjust = $eocd_start - ($cd_start + $cd_size); # adjust CD_start for SFX case
#$cd_start+= $adjust;
die "cd_start > cd_eocd\n" if $cd_start > $eocd_start;
printf "%x\n", $cd_start;
seek $f, $cd_start, 0 or die "Cannot seek to first CD record\n";
read $f, $buf, 4 or die die "Cannot read first CD record\n";
$magic = unpack("V", $buf);
if ($magic != 0x2014b50) { # PK\01\02
	read $f, $buf, $eocd_start - $cd_start - 4;
	if ($buf =~ /PK\01\02/) {
		$cd_start+= $-[0];
	} else {
		die "Cannot find first CD header\n";
	}
}

printf "eocd_start=%x cd_start=%x cd_size=%x\n",
	$eocd_start, $cd_start, $cd_size;

my ($fixed_len, $file_len, $extra_len, $comment_len)=(46,0,0,0);
$cd_start-= 46;	# correct offset for the first CD header

for (my $i=0; $i < $cd_record_num; $i++) {
	seek $f, $cd_start+$fixed_len+$file_len+$extra_len+$comment_len, 0 or
		die "Cannot seek to CD record\n";
	$cd_start = tell($f);
	read $f, $buf, 4+2+2+2+2+2+2+4+4+4+2+2+2 or die "Cannot read CD record\n";

	($magic, undef, undef, undef, undef, undef, undef, undef, undef, undef,
		$file_len, $extra_len, $comment_len) = unpack("VvvvvvvVVVvvv", $buf);
	printf "cd_record=%x cd_size=%x\n", $cd_start,
		$fixed_len+$file_len+$extra_len+$comment_len;
	die "No CD sig\n" if $magic != 0x2014b50; # PK\01\02
}

close $f;
