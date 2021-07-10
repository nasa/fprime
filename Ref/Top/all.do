#!/bin/sh -e

. ./defs.sh

redo-ifchange xml cpp
update xml/*.xml
# Don't update cpp for now
