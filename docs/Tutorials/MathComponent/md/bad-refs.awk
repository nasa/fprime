/<a href="/ {
  n = split($0, fields, /<a href="/)
  for (i = 2; i <= n; ++i) {
    field = fields[i]
    sub(/"[^"]*$/, "", field)
    if (!(field ~ /^#/) && !(field ~ /^http/))
      print field " at line " NR
  }
}
