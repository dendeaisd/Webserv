import cgitb
import os
import uuid
import csv
import datetime
import json
import logging
import hashlib

cgitb.enable()
logging.basicConfig(filename="./cgi-bin/auth/auth.log", level=logging.DEBUG)

db = "./cgi-bin/auth/users.csv"

def hash_string(input_string):
    sha256 = hashlib.sha256()
    sha256.update(input_string.encode('utf-8'))
    hashed_string = sha256.hexdigest()
    return hashed_string

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
        sessions = cookies.split(';')
        if len(sessions) > 1:
            return None
        for cookie in sessions:
            key, value = cookie.split('=')
            if key.strip() == 'session':
                return value
    return ''

def commit_to_db(data):
    """
    Writes the data to the database file.

    Args:
        data (list): The data to be written to the database file.
    """
    with open(db, 'w') as f:
        writer = csv.DictWriter(f, fieldnames=['username', 'password', 'session', 'last_login'])
        writer.writeheader()
        writer.writerows(data)

def auth(username, password):
    data = []
    with open(db, 'r') as f:
        reader = csv.DictReader(f)
        data = list(reader)
        for row in data:
            if row['username'] == username and row['password'] == hash_string(password):
                session = uuid.uuid4().hex
                row['session'] = session
                row['last_login'] = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                commit_to_db(data)
                return session
    return None

def login():
    """
    Retrieves the login page based on the request method.

    Returns:
        str: The HTTP response containing the login page.

    Raises:
        KeyError: If the 'REQUEST_METHOD' key is not found in the environment variables.
    """
    envp = os.environ
    method = envp['REQUEST_METHOD']
    logging.debug(envp)
    response = ''
    if method == 'GET':
        session = get_session_cookie(envp)
        if session:
            # Redirect to the todo app if the user is already logged in
            response = "HTTP/1.1 302 Found\r\n"
            response += "Location: /cgi-bin/todo/app.py\r\n"
            response += f"Set-Cookie: session={session}; Max-Age=3600; HttpOnly; Path=/\r\n"
            response += "Content-Length: 0\r\n"
            response += "\r\n"
        else:
            with open('./apps/auth/index.html') as f:
                content = f.read()
                response = "HTTP/1.1 200 OK\r\n"
                response += "Content-Type: text/html\r\n"
                response += "Content-Length: {}\r\n".format(len(content + "\r"))
                response += "Set-Cookie: session=; Max-Age=0; HttpOnly; Path=/\r\n"
                response += "\r\n"
                response += content
    elif method == 'POST':
        # Add your code for handling POST requests here
        body = json.loads(os.read(0, int(envp['CONTENT_LENGTH'])))
        session = auth(body.get('username'), body.get('password'))
        if session:
            response = "HTTP/1.1 302 Found\r\n"
            response += "Location: /cgi-bin/todo/app.py\r\n"
            response += f"Set-Cookie: session={session}; Max-Age=3600; HttpOnly; Path=/\r\n"
            response += "Content-Length: 0\r\n"
            response += "\r\n"
        else:
            response = "HTTP/1.1 401 Unauthorized\r\n"
    else:
        response = "HTTP/1.1 405 Method Not Allowed\r\n"
    logging.debug(response)
    print(response)

login()
