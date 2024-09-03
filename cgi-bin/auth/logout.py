import os

def logout():
    """
    Logs the user out by deleting the session cookie.

    Returns:
        str: The HTTP response containing the redirect to the login page.
    """
    response = "HTTP/1.1 302 Found\r\n"
    response += "Location: /cgi-bin/auth/login.py\r\n"
    response += "Set-Cookie: session=; Max-Age=0; HttpOnly; Path=/\r\n"
    print(response)

logout()