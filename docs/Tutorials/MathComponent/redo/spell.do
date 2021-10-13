. ./defs.sh

exec 1>&2

for file in $FILES
do
  ispell $file
done
