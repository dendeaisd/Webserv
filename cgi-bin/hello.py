#! /usr/bin/python3

import os
import datetime

def hello():
	envp = os.environ
	content = "<h1>A script that doesn't sleep: {}</h1><p>{}</p>".format(datetime.datetime.now(), envp)
	response = "HTTP/1.1 200 OK\r\n"
	response += "Content-Type: text/html\r\n"
	response += "Content-Length: {}\r\n".format(len(content + "\r"))
	response += "\r\n"
	response += content
	print(response)

hello()
