import os
import logging

logging.basicConfig(filename="hello.log", level=logging.DEBUG)
def return_info_in_html():
	envp = os.environ
	content = "<h1>Environment Variables</h1>"
	content += "<table>"
	for key, value in envp.items():
		content += "<tr><td>{}</td><td>{}</td></tr>".format(key, value)
	content += "</table>"
	response = "HTTP/1.1 200 OK\r\n"
	response += "Content-Type: text/html\r\n"
	response += "Content-Length: {}\r\n".format(len(content + "\r"))
	response += "\r\n"
	response += content
	print(response)

return_info_in_html()
