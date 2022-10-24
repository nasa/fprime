#!/usr/bin/env awk -f

# ----------------------------------------------------------------------
# sections.awk
# Add hierarchical section numbers to Markdown headers
# ----------------------------------------------------------------------

BEGIN {
  MAX_LEVELS = 10
  in_code_block = 0
}

$1 ~ "^```" { in_code_block = !in_code_block }

/^##+ / && !in_code_block {
  new_level = length($1) - 1
  if (new_level > MAX_LEVELS) {
    print "sections.awk: too many levels (" new_level ")" > "/dev/stderr"
    exit(1)
  }
  ++levels[new_level]
  for (i = new_level + 1; i <= MAX_LEVELS; ++i)
    levels[i] = 0
  printf("%s ", $1)
  for (i = 1; i <= new_level; ++i) {
    printf("%d.", levels[i])
  }
  line = $0
  sub(/^#+ +/, "", line)
  printf(" %s\n", line)
  next
}

{ print }
