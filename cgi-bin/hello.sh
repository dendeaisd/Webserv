#! /bin/sh

content="<html><head><title>GIANT SHELL</title></head><body>Amazing shell script that does nothing other than looking cute in the codebase.</body></html>"
echo "HTTP/1.0 200 OK"
echo "Server: $(uname -a)"
echo "Date: $(date)"
echo "Content-type: text/html"
echo "Content-length: ${#content}"
echo ""
echo $content