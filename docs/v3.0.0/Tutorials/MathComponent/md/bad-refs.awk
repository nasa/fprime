# ----------------------------------------------------------------------
# Check for refs that are not of the form
# #... (local ref) or http... (internet URL)
# The most common error here is to forget the # at the start of a local ref
# ----------------------------------------------------------------------

/<a href="/ {
  n = split($0, fields, /<a href="/)
  for (i = 2; i <= n; ++i) {
    field = fields[i]
    sub(/"[^"]*$/, "", field)
    if (!(field ~ /^#/) && !(field ~ /^http/))
      print field " at line " NR
  }
}
