#!/bin/sh -e

for target in `find . -name clean.do -mindepth 2 -maxdepth 2`
do
  dir=`dirname $target`
  echo $dir/clean
done | xargs redo
