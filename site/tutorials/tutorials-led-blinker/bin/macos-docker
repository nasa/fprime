#!/bin/sh

# Detect the project root directory
export SOURCE_DIR=`dirname $0`
export PROJECT_ROOT=`cd "${SOURCE_DIR}/.."; pwd`

USER_ID=`id -u`
GROUP_ID=`id -g`

docker run --platform=linux/amd64 --net host -e USER=$USER -u "${USER_ID}:${GROUP_ID}" -v "${PROJECT_ROOT}:/project" -it \
    nasafprime/fprime-arm:latest
