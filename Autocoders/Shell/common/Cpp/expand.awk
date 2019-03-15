# ----------------------------------------------------------------------
# expand.awk
# ----------------------------------------------------------------------
# Expand shell commands inline
# Preserve indentation, except for preprocessor directives
# ----------------------------------------------------------------------

/^[ \t]*`/ {
  split($0, fields, "`")
  indent = fields[1]
  command = fields[2] " | sed 's/^/"indent"/' | sed 's/^ *#/#/'"
  print "" | command
  close(command)
  next
}

{ print }
