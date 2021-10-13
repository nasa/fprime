files=`awk '
/^include::/ {
  file = $0
  sub(/^include::/, "", file)
  sub(/\[\]/, "", file)
  print file
}' Tutorial.adoc`
redo-ifchange $files Tutorial.adoc
asciidoctor -vn -o $3 Tutorial.adoc
