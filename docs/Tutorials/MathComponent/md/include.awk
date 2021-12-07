# ----------------------------------------------------------------------
# include.awk
# Resolve include commands
# ----------------------------------------------------------------------

/^!include +/ {
  file = $0
  sub(/^!include +/, "", file)
  command = "cat " file
  print "" | command
  close(command)
  next
}

{ print }
