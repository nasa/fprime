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
    mkdir -p /opt/ && chown fprime:fprime /opt/ && \
    git clone --quiet https://github.com/raspberrypi/tools.git /opt/rpi/tools && \
    apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* && \
    pip3 install --no-cache-dir fprime-tools fprime-gds
# Change user and group of the virtual environment
# Make fprime user default user
ENV HOST fprime
ENV USER fprime
ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8
USER fprime
ENTRYPOINT ["/bin/bash"]
