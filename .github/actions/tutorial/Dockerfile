FROM nasafprime/fprime-base

USER root

RUN  apt-get update && apt-get install -y --no-install-recommends doxygen && \
     apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* && pip3 --no-cache-dir install fprime-tools fprime-gds

COPY gps-build /gps-build

ENTRYPOINT ["/gps-build"]