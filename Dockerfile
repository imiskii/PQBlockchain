FROM ubuntu:22.04

RUN apt-get -y update
RUN apt-get -y upgrade
RUN apt-get install -y valgrind
RUN apt-get install -y linux-tools-6.5.0-28-generic
RUN mkdir /PQB
RUN mkdir /PQB/tmp
RUN mkdir /PQB/tmp/confs
RUN mkfifo /PQB/tmp/pipe

COPY build/src/App/pqb /PQB
COPY build/src/App/acc-generator /PQB
COPY build/src/App/commandRunner /PQB
COPY scripts/confs /PQB/tmp/confs
COPY scripts/commands.txt /PQB/tmp

WORKDIR /PQB