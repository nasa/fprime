#!/bin/sh -e

. ./defs.sh

redo-always
locate_uses_do "$@" 1>&2
