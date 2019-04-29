FROM ubuntu:16.04
# Copy-on build specific items 
COPY os-pkg/ubuntu-packages.sh /tmp
COPY python/pip_required_build.txt /tmp
####
# Run the install of required software.
# Note: this is done in one step to prevent extra docker image size.
####
RUN apt-get update && \
    apt-get install -y --no-install-recommends csh git build-essential && \
    yes | /tmp/ubuntu-packages.sh && \
    pip install -r /tmp/pip_required_build.txt && \
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
ENV PATH "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
ENTRYPOINT ["/bin/bash"]
