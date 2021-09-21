#!/usr/bin/env perl

use strict;
use utf8;
use warnings;
use warnings FATAL => 'uninitialized';
use experimental 'signatures';

use Getopt::Long;
# use FP::Repl::Trap; #

sub usage {
    print STDERR map{"$_\n"} @_ if @_;
    print "$0 n
";
exit (@_ ? 1 : 0);
}

our $verbose=0;
#our $opt_dry;
GetOptions("verbose"=> \$verbose,
	   "help"=> sub{usage},
	   #"dry-run"=> \$opt_dry,
	   ) or exit 1;
usage unless @ARGV==1;

sub fib($n) {
    ($n < 2) ? 1 : fib($n-1) + fib($n-2)
}

my ($n) = @ARGV;

print fib($n), "\n";

#use FP::Repl; repl;
#use Chj::ruse;
#use Chj::Backtrace; 

