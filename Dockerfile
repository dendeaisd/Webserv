FROM gcc:latest

WORKDIR /usr/src/app


RUN apt-get update && apt-get install -y valgrind siege ufw tmux vim

# RUN git clone https://github.com/google/googletest.git
# RUN mkdir googletest/build && cd googletest/build && cmake .. && make && make install

# RUN make || true

EXPOSE 8080 8081 8082

# CMD ["./webserv"]