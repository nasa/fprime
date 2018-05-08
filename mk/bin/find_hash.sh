#!/bin/sh

grep -h -- $1 `dirname $0`/../../mk/hash/*.hash
