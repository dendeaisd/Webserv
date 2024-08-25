#! /bin/ruby

content = "<html><head><title>Demo Ruby Script</title></head><body><h1>What a ruby script it is huh...</h1></body></html>"

puts "HTTP/1.1 200 OK"
puts "Content-type: text/html"
puts "Content-length: #{content.length}"
puts ""
puts content