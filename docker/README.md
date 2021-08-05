# F´Docker

Docker is a containerization service. It allows users to pack dependencies into a "container" and
run it as a packaged application. It functions like a lightweight VM, but at its core it wraps
OS calls allowing for more efficient sharing of resources with the host server.

F´ uses docker to wrap the dependencies for building and running F´. From this, F´can be deployed
to servers and computers easily in a packaged standard format. This is specifically useful for
things like Continuous Integration where some unknown worker will need to build and run F´ without
the need to ensure every worker has F´ build dependencies installed.

## Docker Build and Publish Instructions

Our Dockerfile is organized into substages. The first stage is setup with just the tools installed
and the entrypoint is setup to be a generic `bash` prompt. This stage may be used for projects
that want their developers using Docker as an environment.  

### Building Docker Images

This section will describe building each Docker image in turn. In the end, the user will have built
the `latest` docker images and these will be available for users to run.

> **Note:** this base does not require a context (files from the local machine) to build but docker
will "upload" one anyway. Thus we `cd` into the directory in order to assure that we are uploading
as few files as possible.

**Build fprime-base**
```
cd fprime/docker
docker build --target fprime-base -t fprime-base:latest .
```

### Publishing Docker Image to DockerHub

DockerHub hosts images that can be built upon and downloaded directly.  F´ publishes our fprime-base
image for users to download, and CI systems to build upon. Publishing can be done with the following
commands.

**Publish Latest Docker Image**
```
docker tag fprime-base:latest nasafprime/fprime-base:latest
docker login
docker push nasafprime/fprime-base:tagname
```

## Dockerfile

Docker uses a Dockerfile to describe the container that will be built. It is like a recipe for
setting up a blank machine (Ubuntu, etc) into a dependency compliant machine. The F´ supplied
Dockerfile uses Ubuntu 18.04 as a base and installs the required dependencies needed to build F´.
The result is a containerized F´ builder for use with CI.

### Dockerfile Explained

Dependencies are installed in one step. This includes an update and install of Ubuntu
packages, running the ubuntu-packages script, and install the pip dependencies. Finally, the the
app-cache is cleaned. Doing all this in one step prevents excessive Docker image size bloat by
reducing the number of temporary file system layers.

## Manual Docker Usage

Should a user need to run this docker image for purposes other than CI, docker can be run manually to
create an F´container. The user will be root when running the `fprime-base` image. Please ensure that
the fprime-base image has been built or downloaded as latest. If downloaded, run with the tag
`nasafprime/fprime-base:latest` instead of just `fprime-base:latest`. An fprime checkout should be
mapped into the container.

> **Note:** the user/developer should understand Docker UUIDs and the implications in mounting volumes
into a running Docker container because this differs across host OSes.

### Run the F´Docker Image Manually
```bash
fprime>docker run -it -v <path to fprime checkout>:/fprime fprime-base:latest
docker> cd /fprime
... Install an F´ venv ...
... F´ commands here ...
``` 
Here the container is run interactively with a pseudo-terminal and F´checkout is mounted to
`/fprime`. The user can then do an F´ install and then run F´ commands with the RPI tools and OS
dependencies installed.
