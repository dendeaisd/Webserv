import cgitb
import os
import logging

cgitb.enable()

logging.basicConfig(filename="./cgi-bin/todo/todo.log", level=logging.DEBUG)

def get_session_cookie(envp):
    """
    Retrieves the session cookie from the environment variables.

    Args:
        envp (dict): The environment variables.

    Returns:
        str: The session cookie if found, otherwise an empty string.
    """
    if 'HTTP_COOKIE' in envp:
        cookies = envp['HTTP_COOKIE']
        for cookie in cookies.split(';'):
            key, value = cookie.split('=')
            if key.strip() == 'session':
                return value
    return ''


def get_todo_app():
	envp = os.environ
	session = get_session_cookie(envp)
	if not session:
		response = "HTTP/1.1 302 Found\r\n"
		response += "Location: /cgi-bin/auth/login.py\r\n"
		print(response)
		return
	method = envp['REQUEST_METHOD']
	response = ''
	if method == 'GET':
		with open('./apps/todo/index.html') as f:
			content = f.read()
			response = "HTTP/1.1 200 OK\r\n"
			response += "Content-Type: text/html\r\n"
			response += "Content-Length: {}\r\n".format(len(content + "\r"))
			response += f"Set-Cookie: session={session}; Max-Age=3600; HttpOnly; Path=/\r\n"
			response += "\r\n"
			response += content
	else:
		response = "HTTP/1.1 405 Method Not Allowed\r\n"
	print(response)


get_todo_app()
