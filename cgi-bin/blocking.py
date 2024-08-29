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
	sleep(30)
	content = "<h1>A script that DOES sleep: {}</h1>".format(datetime.datetime.now())
	response = "HTTP/1.1 200 OK\r\n"
	response += "Content-Type: text/html\r\n"
	response += "Content-Length: {}\r\n".format(len(content + "\r"))
	response += "\r\n"
	response += content
	print(response)

hello()