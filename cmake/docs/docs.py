#!/usr/bin/env python
"""
docs.py:

Generate CMake documentation as *.md files. This removes CMake code, and replaces the
comments with simple GitHub Markdown. It looks for blocks following the pattern:

```
####
# <documentation ...>
# <documentation ...>
# ...
####
```

@author mstarch
"""
import os
import re
import sys
import enum

LINE_RE = re.compile(r"^#{1,4} ?")

class DocState(enum.IntEnum):
    """
    Holds the documentation generation states.
    """
    SEARCH = 0
    HEADER = 1
    CLOSING = 2


def main():
    """
    Main function used to run this program.
    """
    if len(sys.argv) < 2:
        print("[ERROR] Please supply a *.cmake file to generate documentation.",
              file=sys.stderr)
        sys.exit(1)
    # Read a line, and output it
    out_fn = os.path.basename(sys.argv[1])
    if out_fn == "CMakeLists.txt":
        out_fn = os.path.basename(os.path.dirname(sys.argv[1]))
    out_fn = out_fn.replace(".cmake", "") + ".md"

    # Open both files, and loop over all the lines reading and writing each
    with open(sys.argv[1], "r") as in_file_handle:
        with open(out_fn, "w") as out_file_handle:
            state = DocState.SEARCH
            next_state = DocState.SEARCH
            out_file_handle.write("**Note:** auto-generated from comments in: {0}\n\n"
                                  .format(sys.argv[1]))
            for line in in_file_handle.readlines():
                state = next_state
                next_state = state
                # If we see '####' the next state changes
                if line.startswith("####") and state == DocState.SEARCH:
                    next_state = DocState.HEADER
                elif line.startswith("####"):
                    next_state = DocState.SEARCH
                line = LINE_RE.sub("", line).rstrip()
                #print("State: ", state, "Next State:", next_state)
                #print(line)
                # Now output, if not searching, and not blank and not searching
                if next_state == DocState.SEARCH:
                    if state == DocState.CLOSING:
                        out_file_handle.write("\n\n")
                    continue
                if next_state == DocState.HEADER:
                    out_file_handle.write("## ")
                    next_state = DocState.CLOSING # Header printed
                    continue
                out_file_handle.write(line)
                out_file_handle.write("\n") 

if __name__ == "__main__":
    main()
