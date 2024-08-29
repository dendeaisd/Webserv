import cgitb
import os

cgitb.enable()
def get_todo_app():
	envp = os.environ
	method = envp['REQUEST_METHOD']
	response = ''
	if method == 'GET':
		with open('./apps/todo/index.html') as f:
			content = f.read()
			response = "HTTP/1.1 200 OK\r\n"
			response += "Content-Type: text/html\r\n"
			response += "Content-Length: {}\r\n".format(len(content + "\r"))
			response += "\r\n"
			response += content
	else:
		response = "HTTP/1.1 405 Method Not Allowed\r\n"
	print(response)

get_todo_app()
