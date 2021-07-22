FROM nasafprime/fprime-base:latest

# Copies your code file from your action repository to the filesystem path `/` of the container
USER root
RUN  apt-get update && apt-get install -y --no-install-recommends doxygen && \
     apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* && pip3 --no-cache-dir install fprime-tools fprime-gds
COPY entrypoint.sh /entrypoint.sh
COPY autodoc.bash /autodoc.bash
USER fprime

# Code file to execute when the docker container starts up (`entrypoint.sh`)
ENTRYPOINT ["/entrypoint.sh"]
