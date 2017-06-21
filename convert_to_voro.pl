#---------------------------------------------------------------------
#Danielle McDermott
#6/19/2017
#
#converts file format to voro++ friendly format
#
#
#---------------------------------------------------------------------

#!/usr/bin/perl

use strict;
use warnings;
use Data::Dumper qw(Dumper);

#use Text::CSV;
#my csv = Text::CSV->new({ sep_char => ',' });

#regex black magic hiding in subroutines
sub ltrim { my $s = shift; $s =~ s/^\s+//;       return $s };
sub rtrim { my $s = shift; $s =~ s/\s+$//;       return $s };
sub  trim { my $s = shift; $s =~ s/^\s+|\s+$//g; return $s };

###hackish command line args

print "convert_to_voro.pl $ARGV[0]=input_file $ARGV[1]=output_file\n";


 open(my $in,  "<",  $ARGV[0])  or die "Can't open $ARGV[0]: $!";

 #open a data file to write to:
 open(my $out, ">",  $ARGV[1]) or die "Can't open $ARGV[1]: $!";

my $i = 0; #loop variables i and j
my $j = 0;

my $radius;
#my $x;
#my $y;
    
while ( my $line = <$in>) { # assigns each line in turn to $_      

    #print $line;
    #$line = trim($line);
    #print $line;
    
    #my @temp = split(' ',$line);  #CSV split
    my ($id, $type, $x, $y, $vx, $vy, $speed) = split(' ',$line);  #CSV split
    my $z = 0.0;
    
    ###some debug statements
    #print Dumper \@temp;
    #print split(' ',$line);

    if($type == 1){
	$radius = 0.7;
    }
    elsif($type == 2){
	
	$radius = 0.5;
    }

    print $out sprintf("%d %5.5f %5.5f %5.5f %5.5f\n",$id, $x, $y, $z,$radius);

    #exit;
   } ###end while loop

    close $in or die "$in: $!";

    close $out or die "$out: $!";
 

