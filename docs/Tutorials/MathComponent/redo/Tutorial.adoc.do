files="
Introduction.adoc
The-MathOp-Type.adoc
The-MathOp-and-MathResult-Ports.adoc
The-MathSender-Component.adoc
The-MathReceiver-Component.adoc
Updating-the-Ref-Topology.adoc
Running-the-Ref-Deployment.adoc
"

redo-ifchange $files

echo '= Math Component Tutorial
:toc: left
:toclevels: 3'
for file in $files
do
  echo
  cat $file
done
