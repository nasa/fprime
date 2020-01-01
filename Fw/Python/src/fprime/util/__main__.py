"""
fprime.util.__main__:

This acts as the main entry point for the fprime.util module. This allows users to run fprime.util as an entry point.
This will include the build_helper scripts and run them.
"""
import sys
import fprime.util.build_helper


def main():
    """ Run wrapper, to point a console_script at """
    fprime.util.build_helper.utility_entry(args=sys.argv[1:])


if __name__ == "__main__":
    main()
