import cgitb
import os
import uuid

cgitb.enable()
db = "./cgi-bin/auth/users.csv"

def login(envp):
	session = uuid.uuid4().hex
	# TODO: implement login logic here
	return session
	
def register():
    """
    Retrieves the login page based on the request method.

    Returns:
        str: The HTTP response containing the login page.

    Raises:
        KeyError: If the 'REQUEST_METHOD' key is not found in the environment variables.
    """
    envp = os.environ
    method = envp['REQUEST_METHOD']
    response = ''
    if method == 'GET':
        session = login(envp)
        if not session:
            response = "HTTP/1.1 401 Unauthorized\r\n"
            response += "WWW-Authenticate: Basic realm=\"Secure Area\"\r\n"
        else:
            with open('./apps/auth/index.html') as f:
                content = f.read()
                response = "HTTP/1.1 200 OK\r\n"
                response += "Set-Cookie: session=" + session +"; Max-Age=3600; Secure; HttpOnly\r\n"
                response += "Content-Type: text/html\r\n"
                response += "Content-Length: {}\r\n".format(len(content + "\r"))
                response += "\r\n"
                response += content
    else:
        response = "HTTP/1.1 405 Method Not Allowed\r\n"
    print(response)

register()
