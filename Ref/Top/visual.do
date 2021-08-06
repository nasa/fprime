#!/bin/sh -e

redo-ifchange RefTopologyAppAi.xml
mkdir $3
cd $3
fpl-extract-xml ../RefTopologyAppAi.xml
for file in `ls *.xml`
do
  base=`basename $file .xml`
  fpl-convert-xml $file > $base.txt
  fpl-write-eps < $base.txt > $base.eps
  fpl-layout < $base.txt > $base.json
done

