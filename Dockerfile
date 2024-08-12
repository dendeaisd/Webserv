FROM gcc:latest

WORKDIR /usr/src/app

COPY . .

RUN apt-get update && apt-get install -y cmake git

RUN git clone https://github.com/google/googletest.git
RUN mkdir googletest/build && cd googletest/build && cmake .. && make && make install

RUN make
RUN make run_tests

EXPOSE 8080

CMD ["./webserv"]