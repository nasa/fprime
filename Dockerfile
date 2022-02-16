FROM ubuntu:latest as fprime
WORKDIR /usr/src/
# Update package service
RUN apt-get update

# TZdata input was blocking docker build stage so this line circumvents
RUN DEBIAN_FRONTEND=noninteractive TZ=Australia/Melbourne apt-get -y install tzdata

# Install tools
RUN apt-get install apt-transport-https curl gnupg -yqq locales

# Set up scala
RUN echo "deb https://repo.scala-sbt.org/scalasbt/debian all main" | tee /etc/apt/sources.list.d/sbt.list
RUN echo "deb https://repo.scala-sbt.org/scalasbt/debian /" | tee /etc/apt/sources.list.d/sbt_old.list
RUN curl -sL "https://keyserver.ubuntu.com/pks/lookup?op=get&search=0x2EE0EA64E40A89B84B2DF73499E82A75642AC823" | gpg --no-default-keyring --keyring gnupg-ring:/etc/apt/trusted.gpg.d/scalasbt-release.gpg --import
RUN chmod 644 /etc/apt/trusted.gpg.d/scalasbt-release.gpg
RUN export LC_ALL=en_US.UTF-8
RUN export LANG=en_US.UTF-8
RUN locale-gen en_US.UTF-8

# Install fprime deps
RUN apt-get update
RUN apt-get -y install git cmake default-jre python3 python3-pip sbt scala

# Build and compile fpp dep
RUN git clone https://github.com/fprime-community/fpp.git . \
    && cd compiler \
    && export FPP_SBT_FLAGS='--batch -Dsbt.server.forcestart=true' \
    && ./install /usr/local/bin/fpp

ENV FPP_INSTALL_DIR=/usr/local/bin/fpp
ENV PATH=${PATH}:${FPP_INSTALL_DIR}

# Copy files over and update tools
RUN pip install --upgrade fprime-tools fprime-gds
COPY . .
