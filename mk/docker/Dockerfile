FROM ubuntu:18.04
COPY Fw/Python /tmp/Python
COPY Gds /tmp/Gds
####
# Run the install of required software.
# Note: this is done in one step to prevent extra docker image size.
####
RUN apt-get update && \
    apt-get install -y --no-install-recommends bash git build-essential cmake time python \
                    python-setuptools python-pip python3 python3-pip python3-venv && \
    mkdir -p /opt/ && python3 -m venv /opt/fprime-venv/ && . /opt/fprime-venv/bin/activate && \
    pip install wheel && \
    pip install /tmp/Python && pip install /tmp/Gds[test-api-xls] && \
    git clone --quiet https://github.com/raspberrypi/tools.git /opt/tools && \
    apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

####
# Jenkins:
#   This container is used to build as part of a Jenkins-CI setup. The following lines add in
# the required items to support a Jenkins build.
####
# Jenkins user setup
RUN groupadd jenkins && \
    groupmod -g 1003 jenkins && \
    useradd -ms /bin/bash -g root -G sudo,jenkins -p jenkins jenkins && \
    usermod -u 1003 jenkins
# Jenkins environment
ENV HOST jenkins
ENV USER jenkins
# Paths and entrypoinys
ENV VIRTUAL_ENV "/opt/fprime-venv"
ENV PATH "/opt/fprime-venv/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
ENV LANG C.UTF-8
ENTRYPOINT ["/bin/bash"]
