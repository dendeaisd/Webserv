#! /usr/bin/python3

import os
import cgi
import cgitb
import logging
import datetime
from time import sleep

cgitb.enable()
logging.basicConfig(filename="blocking.log", level=logging.DEBUG)
def hello():
	envp = os.environ
	sleep(20)
	content = "<h1>A script that DOES sleep: {}</h1>".format(envp['USER'])
	response = "HTTP/1.1 200 OK\r\n"
	response += "Content-Type: text/html\r\n"
	response += "Content-Length: {}\r\n".format(len(content + "\r"))
	response += "\r\n"
	response += content
	print(response)
	## touch or append to a file to check if the script is called
	with open("hello.txt", "a") as f:
		f.write(f"Hello {datetime.datetime.now()}\n")
	

# will be called from CGI script

hello()