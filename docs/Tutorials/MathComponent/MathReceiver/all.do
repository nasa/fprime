#!/bin/sh -e

. ./defs.sh

redo-ifchange xml
update xml/*.xml
