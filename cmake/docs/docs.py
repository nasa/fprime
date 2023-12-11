#!/usr/bin/env python3
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
import enum
import os
import re
import sys

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
    if len(sys.argv) < 3:
        print(
            "[ERROR] Please supply a directory of *.cmake files and an out directory",
            file=sys.stderr,
        )
        sys.exit(1)
    if not os.path.isdir(sys.argv[1]) or not os.path.isdir(sys.argv[2]):
        print("[ERROR] Not a directory!")
        sys.exit(2)
    outdir = os.path.abspath(sys.argv[2])
    os.chdir(sys.argv[1])
    for dirpath, dirnames, filenames in os.walk("."):
        for filename in filenames:
            if (
                ".cmake" in filename
                or filename == "CMakeLists.txt"
                or filename.endswith("CMakeLists.txt.template")
            ):
                process_file(os.path.join(dirpath, filename), outdir)


def process_file(file_name, outdir):
    """Process a file"""
    # Read a line, and output it
    out_fn = file_name
    if os.path.basename(out_fn) == "CMakeLists.txt":
        out_fn = os.path.dirname(file_name)
    out_fn = out_fn.replace(".cmake", "").replace(".template", "") + (
        "-template.md" if out_fn.endswith(".template") else ".md"
    )
    if out_fn == file_name:
        raise AssertionError("File collision imminent")
    out_fn = os.path.join(outdir, out_fn)
    os.makedirs(os.path.dirname(out_fn), exist_ok=True)
    # Open both files, and loop over all the lines reading and writing each
    with open(file_name, "r") as in_file_handle:
        with open(out_fn, "w") as out_file_handle:
            state = DocState.SEARCH
            next_state = DocState.SEARCH
            out_file_handle.write(
                "**Note:** auto-generated from comments in: {0}\n\n".format(file_name)
            )
            for line in in_file_handle.readlines():
                state = next_state
                next_state = state
                # If we see '####' the next state changes
                if line.startswith("####") and state == DocState.SEARCH:
                    next_state = DocState.HEADER
                elif line.startswith("####"):
                    next_state = DocState.SEARCH
                line = LINE_RE.sub("", line).rstrip()
                # print("State: ", state, "Next State:", next_state)
                # print(line)
                # Now output, if not searching, and not blank and not searching
                if next_state == DocState.SEARCH:
                    if state == DocState.CLOSING:
                        out_file_handle.write("\n\n")
                    continue
                if next_state == DocState.HEADER:
                    out_file_handle.write("## ")
                    next_state = DocState.CLOSING  # Header printed
                    continue
                out_file_handle.write(line)
                out_file_handle.write("\n")


if __name__ == "__main__":
    main()
