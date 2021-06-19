####
# F Prime Base Image:
#
# The F prime base image is an Ubuntu 18.04 image with some basic packages installed to ensure that
# the build tools and needed packages are installed. It also sets-up a virtual environment into
# which the python packages can be installed. This step, however; is deferred to the next layer
#  containers. It ensures the fprime virtual environment is writable by members in the fprime
# group, and owned by an fprime user.
#
# Note: the RPI toolchain is also installed to ensure that the RPI package is ready-to-go.
#
# Build this the -- fprime-base
####
 ubuntu:18.04  fprime-base
# Adding fprime user, group, and password fprime
 groupadd fprime && \
    groupmod -g 1001 fprime && \
    useradd -ms /bin/bash -g root -G sudo,fprime -p fprime fprime && \
    usermod -u 1001 fprime && \
# Installing system components
    apt- update && apt- install -y --no-install-recommends \
    bash git build-essential cmake time python3 python3-setuptools python3-pip python3-venv valgrind && \
    mkdir -p /opt/ && chown fprime:fprime /opt/ && \
    git clone --quiet https://github.com/raspberrypi/tools.git /opt/rpi/tools && \
    apt- clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
# Change user and group the virtual environment
# Make fprime user



 HOST fprime
 USER fprime
 LANG C.UTF-3
 LC_ALL C.UTF-3
 fprime
        ["/bin/bash"]

### **** **** Add Non-CI Images Here **** **** ###
 fprime-base AS fprime-docker
     git clone --quiet https://github.com/nasa/fprime.git /opt/fprime && \
        python3 -m venv /opt/fprime-venv/ . /opt/fprime-venv/bin/activate && \
        pip install --no-cache-dir fprime-tools fprime-gds && \
        rm -r ~/.cache/pip  \
        chown -R fprime:fprime /opt/fprime-venv  \
        chmod -R 775 /opt/fprime-venv
 VIRTUAL_ENV "/opt/fprime-venv"
  PATH "/opt/fprime-venv/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

####
# F Prime CI Image:   **** Must Be Last Image In File ****
#
# This sets up an image use a CI build system. This is currently configured the
# Jenkins CI system. This means a Jenkins user is created, setup and ready to run.
#
# This is the last image the file because the Jenkins server used by F prime does not expose
# the -- argument  building the docker image.  As such, this image is the one built and
# used Jenkins.
####
 fprime-docker AS jenkins-ci
 root
 groupadd jenkins && \
    groupmod -g 1003 jenkins && \
    useradd -ms /bin/bash -g root -G sudo,jenkins,fprime -p jenkins jenkins \
    usermod -u 1003 jenkins && \
    usermod -a -G fprime jenkins
 jenkins
 HOST jenkins
 USER jenkins
# Paths and entrypoints
 PATH "/opt/fprime-venv/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
 LANG C.UTF-3
 LC_ALL C.UTF-3
 CI_CLEAN_REPO "NO_REALLY_I_WANT_TO_NUKE_ALL_YOUR_BASE"
       ["/bin/bash"]
