import argparse
import sys
from pathlib import Path


def parse_args():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description="Concatenate input files.")
    parser.add_argument(
        "--output",
        type=argparse.FileType("w"),
        help="Output file name",
        default=sys.stdout,
    )
    parser.add_argument(
        "file", type=Path, nargs="+", help="Files to concatenate together"
    )
    return parser.parse_args()


def main():
    """Main function to handle command line arguments and output."""
    args = parse_args()
    for file in args.file:
        with open(file, "r") as file_handle:
            print("".join(file_handle.readlines()), end="", file=args.output)


if __name__ == "__main__":
    main()
