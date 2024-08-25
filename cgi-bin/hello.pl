#! /bin/perl

$dateString = gmtime();
$content = "<html><head><title>Demo Perl Script</title></head><body>GMT date/time $dateString</body></html>\r";
print "HTTP/1.0 200 OK\r";
print "Content-type: text/html\r";
print "Content-length: " . length($content) . "\r";
print "\r";
print $content;
print "\r"