import argparse
import difflib
import os
import sys

from fprime_ac.utils.buildroot import set_build_roots


def main():
    """Process arguments: file old, file new, module deps to see if any mod deps has changed"""
    set_build_roots(os.environ["BUILD_ROOT"])
    parser = argparse.ArgumentParser(
        description="Has a module dependency changed within the locs file."
    )
    parser.add_argument("new_locs", help="New locs file")
    parser.add_argument("prev_locs", help="Original locs file")
    parser.add_argument("file_deps", nargs="+", help="Module deps")

    args_ns = parser.parse_args()

    if not os.path.exists(args_ns.new_locs):
        print(f"[ERROR] Failed to open {args_ns.new_locs}")
        sys.exit(1)
    # Previous files not generated
    if not os.path.exists(args_ns.prev_locs):
        print("No pervious locations")
        sys.exit(1)

    with open(args_ns.prev_locs, "r") as prev_locs_fh:
        prev_lines = prev_locs_fh.readlines()
    with open(args_ns.new_locs, "r") as new_locs_fh:
        new_lines = new_locs_fh.readlines()

    diff_output = list(difflib.unified_diff(prev_lines, new_lines, n=0, lineterm="\n"))
    subtracted_lines = [
        diff
        for diff in diff_output
        if diff.startswith("-") and not diff.startswith("---")
    ]
    relative_mod_paths = [line.split()[-1].strip('"') for line in subtracted_lines]
    subtractions = [
        os.path.abspath(os.path.join(os.path.dirname(args_ns.new_locs), rel_path))
        for rel_path in relative_mod_paths
    ]
    changed = []
    for subtraction in subtractions:
        if subtraction in args_ns.file_deps:
            changed.append(os.path.basename(subtraction))
    if changed:
        print(f"{', '.join(changed)}")
        sys.exit(2)
    sys.exit(0)


if __name__ == "__main__":
    main()
