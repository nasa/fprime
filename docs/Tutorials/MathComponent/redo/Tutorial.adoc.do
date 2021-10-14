. ./defs.sh

redo-ifchange $FILES

echo '= Math Component Tutorial
:toc: left
:toclevels: 3'
for file in $FILES
do
  echo
  cat $file
done
