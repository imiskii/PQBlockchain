FROM ubuntu:22.04

RUN  apt-get -y update && apt-get -y upgrade
RUN apt-get install -y valgrind
RUN mkdir /PQB
RUN mkdir /PQB/tmp
RUN mkdir /PQB/tmp/confs


COPY build/src/App/pqb /PQB
COPY build/src/App/acc-generator /PQB
COPY scripts/confs /PQB/tmp/confs

WORKDIR /PQB

RUN ./acc-generator < tmp/confs/confs.txt
RUN rm tmp/log.txt