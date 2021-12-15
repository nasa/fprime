#!/usr/bin/env awk -f

# ----------------------------------------------------------------------
# toc.awk
# Generate table of contents
# Must generate tags first
# ----------------------------------------------------------------------

BEGIN {
  in_code_block = 0
  print "## Table of Contents"
  print ""
}

$1 ~ "^```" { in_code_block = !in_code_block }

/^##+ / && !in_code_block {
  indent_level = length($1) - 2
  tag = prev_line
  sub(/^[^"]*"/, "", tag)
  sub(/"[^"]*$/, "", tag)
  header = $0
  sub(/^#* */, "", header)
  for (i = 1; i <= indent_level; ++i)
    printf("  ")
  print "* <a href=\"#" tag "\">" header "</a>"
}

{
  prev_line = $0
}
