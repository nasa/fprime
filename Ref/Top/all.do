#!/bin/sh -e

. ./defs.sh

redo-ifchange xml cpp
update cpp/*.{hpp,cpp} xml/*.xml
