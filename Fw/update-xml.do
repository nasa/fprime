#!/bin/sh -e

for target in `find . -name update-xml.do -mindepth 2 -maxdepth 2`
do
  dir=`dirname $target`
  echo $dir/update-xml
done | xargs redo-ifchange
