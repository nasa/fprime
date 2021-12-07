#!/usr/bin/env awk -f

# ----------------------------------------------------------------------
# tags.awk
# Insert anchor tags for Markdown sections
# ----------------------------------------------------------------------

BEGIN {
  level = 0
  in_code_block = 0
}

$1 ~ /^```/ && in_code_block == 0 { in_code_block = 1; print; next }
$1 ~ /^```/ && in_code_block == 1 { in_code_block = 0; print; next }

$1 ~ /^##+$/ && !in_code_block {
  level = length($1)
  level_tag = ""
  for (i = 2; i <= NF; ++i) {
    if (level_tag == "")
      level_tag = $i
    else
      level_tag = level_tag "-" $i
  }
  gsub(/[,:]/, "", level_tag)
  level_tags[level] = level_tag
  tag = ""
  for (i = 1; i <= level; ++i) {
    if (tag == "")
      tag = level_tags[i]
    else
      tag = tag "_" level_tags[i]
  }
  print "<a name=\"" tag "\"></a>"
}

{ print $0 }
