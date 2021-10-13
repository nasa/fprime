files=`awk '
/^include::/ {
  file = $0
  sub(/^include::/, "", file)
  sub(/\[\]/, "", file)
  print file
}' Tutorial.adoc`
redo-ifchange $files Tutorial.adoc

awk '
/^include::/ {
  file = $0
  sub(/^include::/, "", file)
  sub(/\[\]/, "", file)
  command = "cat " file
  print "" | command
  close(command)
  next
}

{ print }' Tutorial.adoc
