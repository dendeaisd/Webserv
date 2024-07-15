# The Web

The Hypertext Transfer Protocol (HTTP) is defined by a set of RFCs (Request for Comments) documents. Here's a brief summary of the key RFCs for HTTP:

1. RFC 1945 - HTTP/1.0: This RFC describes the first version of HTTP, which is a simple protocol for raw data transfer across the Internet. HTTP/1.0 is stateless, meaning each request from client to server must contain all the information needed to understand and respond to the request.

2. RFC 2616 - HTTP/1.1: This RFC introduces improvements over HTTP/1.0, including persistent connections, chunked transfer-coding, and additional cache control mechanisms. It also introduces the OPTIONS method and the 100 Continue status.

3. RFC 7230-7235 - HTTP/1.1 (revision): These RFCs are a detailed revision of HTTP/1.1 (RFC 2616). They clarify ambiguities and introduce few new features. The protocol remains mostly the same, but these RFCs provide a more accurate description of the protocol.

4. RFC 7540 - HTTP/2: This RFC describes a major revision of HTTP. It does not modify the semantics of HTTP, but changes how data is framed and transported between the client and server. HTTP/2 introduces a binary protocol that allows for multiple concurrent requests/responses to be multiplexed over a single connection.

5. RFC 8297 - HTTP Early Hints: This RFC introduces the 103 Early Hints status code, which can be used to return link relation type headers that indicate what resources will be needed to render the requested page.

6. RFC 8470 - HTTP/2 Origin Frames: This RFC describes the ORIGIN HTTP/2 frame, which allows a server to indicate what origins are available on the same connection.

7. RFC 7541 - HPACK: This RFC describes HPACK, a compression format for efficiently representing HTTP header fields in HTTP/2.

8. RFC 8441 - WebSockets over HTTP/2: This RFC describes how the WebSocket protocol can be used over an HTTP/2 connection.

9. RFC 8833 - HTTP/3: This RFC describes HTTP/3, the next major version of HTTP. HTTP/3 uses QUIC, a transport protocol that provides improved performance characteristics compared to TCP, as its underlying transport protocol.

HTTP status codes are three-digit numbers returned by servers to indicate the status of a web request. Here are some of the most common ones:

1. 1xx (Informational): These are informational codes indicating that the request was received and understood. They are rarely used in practice.
    - 100 Continue: The server has received the request headers, and the client should proceed to send the request body.

2. 2xx (Successful): These codes indicate that the request was successfully received, understood, and accepted.
    - 200 OK: The request has succeeded. The information returned with the response depends on the method used in the request.
    - 201 Created: The request has been fulfilled and resulted in a new resource being created.
    - 204 No Content: The server has fulfilled the request but does not need to return a response body.

3. 3xx (Redirection): These codes indicate that further action needs to be taken by the user agent in order to fulfill the request.
    - 301 Moved Permanently: The URL of the requested resource has been changed permanently. The new URL is given in the response.
    - 302 Found: This response code means that the URI of the requested resource has been changed temporarily.
    - 304 Not Modified: The resource has not been modified since the version specified by the request headers.

4. 4xx (Client Error): These codes are intended for situations in which the client seems to have erred.
    - 400 Bad Request: The server could not understand the request due to invalid syntax.
    - 401 Unauthorized: The request requires user authentication.
    - 403 Forbidden: The server understood the request, but it refuses to authorize it.
    - 404 Not Found: The server can't find the requested resource.

5. 5xx (Server Error): These codes indicate cases in which the server is aware that it has encountered an error or is otherwise incapable of performing the request.
    - 500 Internal Server Error: The server encountered an unexpected condition that prevented it from fulfilling the request.
    - 502 Bad Gateway: The server was acting as a gateway or proxy and received an invalid response from the upstream server.
    - 503 Service Unavailable: The server is currently unable to handle the request due to a temporary overloading or maintenance of the server.

HTTP defines a set of request methods, often referred to as HTTP verbs, to indicate the desired action to be performed on the identified resource. Here are some of the most common ones:

1. GET: The GET method requests a representation of the specified resource. Requests using GET should only retrieve data and should have no other effect.
2. POST: The POST method is used to submit an entity to the specified resource, often causing a change in state or side effects on the server.
3. PUT: The PUT method replaces all current representations of the target resource with the request payload.
4. DELETE: The DELETE method deletes the specified resource.
5. HEAD: The HEAD method asks for a response identical to that of a GET request, but without the response body.
6. OPTIONS: The OPTIONS method is used to describe the communication options for the target resource.
7. PATCH: The PATCH method is used to apply partial modifications to a resource.
8. CONNECT: The CONNECT method establishes a network connection to the resource (usually for use with a proxy or load balancer).

Each of these methods can be used for different purposes in the context of an HTTP request, and the method used can affect how the server processes the request.

## Resources

- [HTTP/1.1](https://www.rfc-editor.org/info/rfc9112) (Recommended)
- [Evolution of HTTP](https://www.youtube.com/watch?v=z7x2Qk0j3rk) (Video, recommended)
- [Introduction to NGINX](https://www.youtube.com/watch?v=9t9Mp0BGnyI) (Recommended)
- [QUIC and HTTP3](https://www.rfc-editor.org/rfc/inline-errata/rfc9114.html) (not significant for 42 webserv)
- [HTTP/2](https://www.rfc-editor.org/info/rfc9113) (Great doc and can be used for webserv but the project mentions 1.1 explicitly)
- [List of all RFC’s related to HTTP](https://www.rfc-editor.org/search/rfc_search_detail.php?page=All&title=http&pubstatus[]=Any&pub_date_type=any&sortkey=Number&sorting=ASC) (exhaustive list, probably won’t need)

## I/O

- [I/O multiplexing 1](https://www.youtube.com/watch?v=wB9tIg209-8)
- [I/O multiplexing 2](https://medium.com/@rrakshith007/io-multiplexing-and-its-advantages-8c75584079d1)
- [Select/Poll](https://stackoverflow.com/questions/26420947/what-are-the-underlying-differences-among-select-epoll-kqueue-and-evport)
- [I/O multiplexing 3](https://nima101.github.io/io_multiplexing)

## Performance and Testing

- [Siege for stress testing](https://manpages.ubuntu.com/manpages/trusty/man1/siege.1.html) (it’s repo is [here](https://github.com/JoeDog/siege))
- Writing our own test cases in python or go as recommended in the project doc.
