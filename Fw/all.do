#!/bin/sh -e

#redo-ifchange update-xml

redo-ifchange $2.targets.txt
targets=`cat $2.targets.txt`
redo-ifchange $targets

