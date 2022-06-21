FROM ubuntu:16.04
ARG my_arg
RUN apt-get -y update \
  && apt-get -y install build-essential \
  && apt-get -y install cmake \
  && apt-get -y install libgtest-dev \
  && apt-get clean
COPY / /app
RUN mkdir -p ExternalLibs/gTest \
  && cd ExternalLibs/gTest \
  && cmake /usr/src/gtest \
  && make 
RUN cd /app/build && cmake ..  && make $my_arg


