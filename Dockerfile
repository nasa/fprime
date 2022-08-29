FROM ubuntu:latest as fprime
WORKDIR /usr/src/

# This is a massive install, avoid putting lines above
RUN apt-get update
RUN apt-get -y install gcc-arm-none-eabi

# Install fprime deps
RUN apt-get update
RUN apt-get -y install git cmake default-jre python3 python3-pip
COPY "./requirements.txt" .
RUN pip install -r "/usr/src/requirements.txt"

ENV PATH=${PATH}:~/.local/bin
RUN git config --global --add safe.directory /usr/src

COPY . .