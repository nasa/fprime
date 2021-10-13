files="
Introduction.adoc
The-MathOp-Type.adoc
The-MathOp-and-MathResult-Ports.adoc
"

redo-ifchange $files

echo '= Math Component Tutorial'
for file in $files
do
  echo
  cat $file
done
