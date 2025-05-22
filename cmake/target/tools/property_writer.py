import argparse
import sys
from pathlib import Path


DEFINE = "define_property({})"
SET = "set_property({})"
APPEND = "include(utilities)\nappend_list_property({})"


def parse_args():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(description="Write properties to a file.")
    parser.add_argument(
        "directive",
        type=str,
        help="Directive: SET, APPEND, or DEFINE",
        choices=["SET", "DEFINE", "APPEND"],
    )
    parser.add_argument(
        "values", type=str, nargs="+", help="Value(s) to write to property call"
    )
    parser.add_argument(
        "--file", type=Path, help="File to write the property to", default=None
    )
    parser.add_argument(
        "--append",
        action="store_true",
        help="Append to the file instead of overwriting",
        default=False,
    )

    parsed = parser.parse_args()
    parsed.file = (
        sys.stdout
        if parsed.file is None
        else open(parsed.file, "a" if parsed.append else "w")
    )
    return parsed


def main():
    """Main function to handle command line arguments and write properties."""
    args = parse_args()
    format_string = SET if args.directive == "SET" else DEFINE
    print(format_string.format(" ".join(args.values)), file=args.file)


if __name__ == "__main__":
    main()
