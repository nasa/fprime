# F´Docker

Docker is a containerization service. It allows users to pack dependencies into a "container" and
run it as a packaged application. It functions like a light-weight VM, but at its core it wraps
OS calls allowing for more efficient sharing of resources on a server.

F´used docker to wrap the dependencies for building and running F´. From this, F´can be deployed
to servers and computers easily in a packaged standard format. This is specifically useful for
things like Continuous Integration where some unknown worker will need to build and run F´without
the hassel of ensuring every worker has F´build dependencies installed.

## Dockerfile

Docker uses a Dockerfile to describe the container that will be built. It is like a recipe for
setting up a blank machine (Ubuntu, etc) into a dependency compliant machine. The F´supplied
Dockerfile uses Ubuntu 16.04 as a base and installs the required dependencies needed to build F´.
The result is a containerized F´builder for use with CI.

### Dockerfile Explained

The current Dockerfile is shown below. The first step is to COPY in two scripts into the container
to aid the setup. These are put in `/tmp` and can be used during further setup scripts.

Next the dependencies are installed in one step. This includes an update and install of Ubuntu
packages, running the ubuntu-packages script, and install the pip dependencies. Finally, the the
app-cache is cleaned. Doing all this in one step prevents excessive Docker image size bloat.

Lastly, the user "jenkins" is created, given root permissions, and set as the user for when the
container is run. The PATH and ENTRYPOINT is set for when the container runs.

```Dockerfile
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
``` 

## Manual Docker Usage

Should a user need to run this docker image for purposes other than CI, docker can be run manually to
create an F´container. The user will still be Jenkins, and should be changed if this is unacceptable.

**Note:** docker is assumed to be installed. The user should under stand Docker UUIDs and the
implications in mounting volumens into Docker.

### Build the F´Docker Image
```bash 
fprime>docker build -t fprime -f docker/Dockerfile mk/
``` 
Here the Docker container is built using "mk/" as context and specifying the F´dockerfile.

### Run the F´Docker Image Manually
```bash
fprime>docker run -it -v <fprime checkout>:/fprime fprime
docker> cd /fprime
... F´commands here ...
``` 
Here the container is run interactively with a pseudo-terminal and F´checkout is mounted to
`/fprime`. The user can then do normal F´commands.