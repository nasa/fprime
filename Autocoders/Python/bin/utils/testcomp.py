#!/usr/bin/env python
import os
import sys
import fprime_ac.utils.ac_cli_helpers


def main(args=sys.argv):
    """
    Main program to wrap production of dictionaries
    """
    parsed = fprime_ac.utils.ac_cli_helpers.parse_args(args[1:], specs=["input_files", "build_root"],
                                                       desc="F′ test implementation generator")
    for input_file in parsed.input_files:
        cwd = os.path.dirname(input_file)
        fprime_ac.utils.ac_cli_helpers.run_cli("--build_root", "-u", input_file, cwd=cwd)
        

if __name__ == "__main__":
    main()
