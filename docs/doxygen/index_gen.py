#!/usr/bin/env python3
"""
index_gen.py:

Generates a markdown index for all the `.md` files in a directory (recursively). This prints out
to stdout by default, therefore redirects are needed to write out to a file.

Usage:
    index_gen.py <directory> <title> <excludes>

    directory: The directory to generate the index for
    title: The title to use for the header of the generated markdown
    excludes: A list of directories to exclude from the index (comma-separated)

@author thomas-bc
"""

import os
import sys
from pathlib import Path
import argparse


parser = argparse.ArgumentParser(
    description="Generates a markdown index for all the `.md` files in a directory (recursively)."
)
parser.add_argument(
    "directory", type=str, help="The directory to generate the index for"
)
parser.add_argument(
    "title", type=str, help="The title to use for the header of the generated file"
)
parser.add_argument(
    "excludes", type=str, help="Comma-separated list of directories to exclude"
)
args = parser.parse_args()


def main():
    if not Path(args.directory).exists():
        print(f"[ERROR] {args.directory} is not a directory!")
        sys.exit(2)
    # Print the index header (stdout is redirected to index.md)
    print(f"# {args.title}")
    os.chdir(args.directory)
    for path in sorted_listdir(Path(".")):
        if path.name not in args.excludes.split(","):
            process_path(path)


def process_path(path: Path):
    """Process a path recursively, printing out the index"""
    depth = len(path.relative_to(Path(".")).parts)
    if path.is_dir():
        if any([path.name.endswith(".md") for path in sorted_listdir(path)]):
            print(f"#{'#' * depth} {path}")
        for subpath in sorted_listdir(path):
            process_path(subpath)
    elif path.is_file() and path.suffix == ".md":
        print(f"- [{path.stem}]({path.relative_to(Path('.'))})")


def sorted_listdir(path: Path):
    """
    Return a sorted list of paths in a directory. List is sorted by files first, then
    directories. Files first is required to ensure the index is correctly generated
    """
    return sorted([path for path in path.iterdir() if path.is_file()]) + sorted(
        [path for path in path.iterdir() if path.is_dir()]
    )


if __name__ == "__main__":
    main()
