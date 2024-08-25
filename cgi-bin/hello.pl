#!/usr/bin/perl
use strict;
use warnings;

my $dateString = gmtime();
my $content = "<html><head><title>Demo Perl Script</title></head><body><h2>GMT date/time $dateString</h2></body></html>\r\n";

print "HTTP/1.0 200 OK\r\n";
print "Content-type: text/html\r\n";
print "Content-length: " . length($content) . "\r\n";
print "\r\n";
print $content;
