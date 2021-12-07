/^include::/ {
  file = $0
  sub(/^include::/, "", file)
  sub(/\[\]/, "", file)
  command = "cat " file
  print "" | command
  close(command)
  next
}

{ print }
