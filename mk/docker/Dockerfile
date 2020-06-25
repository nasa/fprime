####
# F Prime Base Image:
#
# The F prime base image is an Ubuntu 18.04 image with some basic packages installed to ensure that
# the build tools and needed packages are installed. It also sets-up a virtual environment into
# which the python packages can be installed. This step, however; is deferred to the next layer
# of containers. It ensures the fprime virtual environment is writable by members in the fprime
# group, and owned by an fprime user.
#
# Note: the RPI toolchain is also installed to ensure that the RPI package is ready-to-go.
#
# Build this with the --target fprime-base
####
FROM ubuntu:18.04 AS fprime-base
# Adding fprime user, group, and password fprime
RUN groupadd fprime && \
    groupmod -g 1001 fprime && \
    useradd -ms /bin/bash -g root -G sudo,fprime -p fprime fprime && \
    usermod -u 1001 fprime && \
# Installing system components
    apt-get update && apt-get install -y --no-install-recommends \
    bash git build-essential cmake time python3 python3-setuptools python3-pip python3-venv valgrind && \
    mkdir -p /opt/ && \
    git clone --quiet https://github.com/raspberrypi/tools.git /opt/rpi/tools && \
    apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
# Change user and group of the virtual environemnt
# Make fprime user default user
ENV HOST fprime
ENV USER fprime
ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8
ENTRYPOINT ["/bin/bash"]

### **** **** Add Non-CI Images Here **** **** ###
## Example docker container with F prime code copied in:
#FROM fprime-base AS fprime-docker
#    COPY . /opt/fprime
#    RUN python3 -m venv /opt/fprime-venv/ && . /opt/fprime-venv/bin/activate && \
#        pip install wheel && \
#        pip install /opt/fprime/Fw/Python/ && \
#        pip install /opt/fprime/Gds/ && \
#        rm -r ~/.cache/pip && \
#        chown -R fprime:fprime /opt/fprime-venv && \
#        chmod -R 775 /opt/fprime-venv
#ENV VIRTUAL_ENV "/opt/fprime-venv"
#ENV PYTHONHOME "/opt/fprime-venv"
#ENV PATH "/opt/fprime-venv/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

####
# F Prime CI Image:   **** Must Be Last Image In File ****
#
# This sets up an image for use within a CI build system. This is currently configured for the
# Jenkins CI system. This means a Jenkins user is created, setup and ready to run.
#
# This is the last image in the file because the Jenkins server used by F prime does not expose
# the --target argument for building the docker image.  As such, this image is the one built and
# used for Jenkins.
####
FROM fprime-base AS jenkins-ci
RUN groupadd jenkins && \
    groupmod -g 1003 jenkins && \
    useradd -ms /bin/bash -g root -G sudo,jenkins,fprime -p jenkins jenkins && \
    usermod -u 1003 jenkins
ENV HOST jenkins
ENV USER jenkins
# Paths and entrypoinys
ENV PATH "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8
ENV CI_CLEAN_REPO "NO_REALLY_I_WANT_TO_NUKE_ALL_YOUR_BASE"
ENTRYPOINT ["/bin/bash"]
