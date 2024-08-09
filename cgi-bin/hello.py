#! /usr/bin/python3

import os
import datetime
def hello():
	envp = os.environ
	print("Content-Type: text/html")
	if "CONTENT_LENGTH" in envp:
		length = int(envp["CONTENT_LENGTH"])
		
		print("<h1>POST data: {}</h1>".format(length))
	## touch or append to a file to check if the script is called
	with open("hello.txt", "a") as f:
		f.write(f"Hello {datetime.datetime.now()}\n")
	

# will be called from CGI script

hello()