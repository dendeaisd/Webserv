# Webserv

## Description

**Webserv** is a fully custom HTTP server written in **C++17**, designed to provide a deep understanding of the Hypertext Transfer Protocol (HTTP). This project was built from scratch to handle HTTP requests and responses, implement core web server functionality, and include features like file uploads, CGI execution, and serving static websites.

Our implementation includes all mandatory requirements and a comprehensive suite of bonus features for an enhanced user experience. Designed to be resilient, non-blocking, and efficient, **Webserv** complies with HTTP 1.1 standards and has been tested extensively against browsers and benchmarking tools like Siege.

## Table of Contents
1. [Description](#description)
2. [Lifecycle](#lifecycle)
3. [Prerequisites](#prerequisites)
4. [Installation](#installation)
5. [Features](#features)
6. [Configuration](#configuration)
7. [Technical Details](#technical-details)
8. [Testing and Benchmarking](#benchmarking-with-siege)
9. [Curl Examples](#curl-with-detailed-reporting)


# Lifecycle
![](default/img/Webserv%20Lifecycle%20Overview.png)

## Prerequisites

- **C++17 Compiler**: Ensure a compatible compiler is installed.
- **Make**: Required for building the project.
- **Docker**: Optional, for containerized deployment.

## Installation

1. **Clone the Repository**:

```bash
git clone https://github.com/dendeaisd/Webserv.git
cd Webserv
```

2. **Build the Server**:
  
```bash
make
```

3. **Run the Server**:
```bash
./webserv [configuration file]
```
If no configuration file is specified, it defaults to `./configs/default.conf`.

### Docker Deployment

To run the server in a Docker container:
```bash
make docker-run
```

# Features

- **HTTP Methods**: Supports GET, POST, PUT, etc. methods
- **Non-Blocking I/O**: Utilizes `poll()` for efficient client handling.
- **Static File Serving**: Delivers HTML, CSS, JavaScript, and image files.
- **File Uploads**: Allows clients to upload files to the server and download them.
- **CGI Support**: Executes scripts in various languages (e.g., Python, PHP, Ruby) based on configuration.
- **Custom Error Pages**: Provides user-friendly error messages.
- **Cookies and Session Management**: Implements stateful sessions; demonstrated with a To-Do application.


## 🌟Default Landing Page

Upon starting the server, the default landing page (**IRIDESCENT**) is accessible at:
```bash
http://localhost:[port]
```

![Screenshot 2024-11-26 024810](https://github.com/user-attachments/assets/2166f445-1773-478c-8b7c-82e1da4cc0d6)

Here you can see example of the features of the project like trying our `ToDo App`, the `uploads` page or various scripts showcasing the functionality of the CGI, when you open the menu on the top-right side of the page.

## 📂 Serving Static Files

Example: Visit a static file served by the server.

```bash
http://localhost:[port]/index.html
```

Add your static HTML, CSS, and JS files in the directory defined in the `root` configuration.

## 📤 File Uploads
Upload files directly through the Uploads interface on the default page or via HTTP POST.
```bash
http://localhost:[port]/upload
```
## 💻 CGI Execution
Run dynamic scripts written in various languages (e.g., Python, Perl, Ruby, Bash). Examples are available directly on the default page. Select a script to see its execution in real-time.

Examples:

- Python Script: `http://localhost:[port]/cgi-bin/hello.py`
 - Ruby Script: `http://localhost:[port]/cgi-bin/hello.rb`

## 📝 To-Do App
Explore a fully functional To-Do application that showcases cookie-based session management. Add, remove, or manage tasks, with session persistence.
```bash
http://localhost:[port]/todo
```

## 🚦 Custom Error Pages
Trigger custom error responses by accessing routes with missing permissions or non-existent resources.

Examples:

![Screenshot 2024-11-26 033634](https://github.com/user-attachments/assets/25283d63-86e2-4982-a747-5512a0b31752)

![Screenshot 2024-11-26 033606](https://github.com/user-attachments/assets/1fe2b6da-9909-43d5-8858-d86ae4b9ab52)


![Screenshot 2024-11-26 033706](https://github.com/user-attachments/assets/ef0b91c9-c8d8-45dc-af24-1e8364cb473b)

## Configuration

The server's behavior is controlled by a configuration file that defines various settings, such as ports, error pages, routes, and upload locations. Below is an example of a configuration file and its features:

### Example Configuration: `default.conf`


```conf
# Define server block
server {
  listen 8080;
  server_name localhost;

  # Root directory for the server
  root /var/www/html;

  # Default error pages
  error_page 404 /error_pages/404.html;
  error_page 403 /error_pages/403.html;

  # Route configuration
  location / {
    autoindex on;
    index index.html;
  }

  # File upload settings
  location /upload {
    upload_store /var/www/upload;
    upload_pass /cgi-bin/upload.cgi;
  }
}
```
## Key configurations options

- `listen`: Specifies the port the server will listen to.
- `server_name`: Defines the host or domain name.
- `root`: Sets the root directory from which files will be served.
- `error_page`: Maps HTTP error codes (e.g., 404, 403) to custom error pages.
- `location`: Defines specific routing behaviors:
- `autoindex`: Enables directory listing if no index file is found.
- `index`: Specifies the default file to serve when a directory is accessed.
- `upload_store` and `upload_pass`: Configure file upload handling.
This file serves as a template to demonstrate how to use the configuration system.

Additional configurations can be added to enable more advanced features like for example CGI handling.

## Technical Details

The `Webserv` project is built using **C++17**, following object-oriented programming principles and modular design. Below is an overview of its core structure and how the server operates.

### Entry Point: `main.cpp`

The server starts in `main.cpp`, where it:
1. **Processes the Command-Line Arguments**:
   - Accepts an optional configuration file path as an argument.
   - Defaults to `./configs/default.conf` if no argument is provided.
   - Validates the presence of the configuration file using `access()`.

2. **Parses the Configuration File**:
   - The `ParseConfigFile` class is used to read and validate the configuration file.
   - If the configuration is invalid or missing, the program exits with an error message.

3. **Initializes Components**:
   - **Logging**: Configures logging using the `Log` singleton.
   - **CGI Manager**: Configures CGI routes via the `CGIFileManager` class.

4. **Starts the Server**:
   - Creates and runs an instance of the `Server` class, passing the parsed configuration.

### Exception Handling

`main.cpp` includes error handling to capture and log:
- **Socket Errors** (e.g., binding or listening issues).
- **Poll Manager Errors** (e.g., issues with non-blocking I/O).
- **General Exceptions** for any unforeseen issues during runtime.

### Modular Design

The project employs a modular structure for clear separation of concerns:
- **Networking**: Managed by the `Server` class and supporting components.
- **Configuration**: The `ParseConfigFile` class handles configuration parsing and validation.
- **Logging**: The `Log` singleton enables debug-level logging for easier troubleshooting.
- **CGI Management**: The `CGIFileManager` handles dynamic content and script execution.

# To bypass chrome CORS policy

Paste this in chrome and disable the attr: `chrome://flags/#block-insecure-private-network-requests`

reference: https://developer.chrome.com/blog/cors-rfc1918-feedback


# Benchmarking with siege
```bash
make docker-run
make re
./webserv
siege -b -c 100 -t 1m -T 2S http://127.0.0.1:8080/benchmark
```

# Curl with detailed reporting

let's agree, curl is cute
```bash
curl -v -s -w "\n-------\n\nDNS Lookup: %{time_namelookup}\nConnect: %{time_connect}\nStart Transfer: %{time_starttransfer}\nTotal: %{time_total}\n" http://localhost:8080/cgi-bin/blocking.py

curl -o /dev/null -s -w "DNS Lookup: %{time_namelookup}\nConnect: %{time_connect}\nStart Transfer: %{time_starttransfer}\nTotal: %{time_total}\n" http://localhost:8080/cgi-bin/blocking.py
```
