'''
F' GDS Testing Main

This module allows the user to run the F' test case using F' style unittest symantics. Usually,
a user runs python unittests using the following command:

python -m unittest <test module/suite>

This can be replaced with the following command:

python -m fprime.gse.testing <test module/suite>

@author mstarch
'''
from __future__ import print_function
import os
import sys
import errno
import argparse
import unittest

import fprime.gse.testing.runner
import fprime.gse.testing.support

from fprime.gse.testing import TESTING_LOG_DIR

def arg_parser(log_dir):
    '''
    Sets up the argument parsing for this module
    @param log_dir: log directory default
    '''
    parser = argparse.ArgumentParser(description="Fprime/unittest integration test run program. " +
                                     "used to setup standard logging, enable F' features and " +
                                     "provides an interactive mode used to step through tests.")
    parser.add_argument("-i", '--interactive', help='Run in interacive test mode', default=False,
                        action="store_true")
    parser.add_argument("-n", '--no-color', help='Run without colorizer', default=False,
                        action="store_true")
    parser.add_argument("-l", '--log-dir', help='Set logging directory', default=log_dir)
    parser.add_argument("-t", '--test-bed', help='Testbed value enabling testbed dependent tests',
                        default="default")
    parser.add_argument("-c", '--config', help='Configuration key value to add to testbed config',
                        nargs=2, action="append", required=False)
    parser.add_argument("--verbose", "-v", action='count')
    parser.add_argument("--list-only", action='store_true', default=False)
    parser.add_argument("test")
    return parser.parse_args()

def main():
    '''
    Main function, used to provide a relatively simple interface to the FPrimeTestRunner enabling
    standard python unittest runs and an interactive mode for walking through tests.
    '''
    args = arg_parser(TESTING_LOG_DIR)
    try:
        os.makedirs(args.log_dir)
    except OSError as exc:
        if exc.errno != errno.EEXIST or not os.path.isdir(args.log_dir):
            raise
    #Setup testbed configuration directory
    tb_config = {
        "name": args.test_bed,
    }
    if args.config != None:
        for key, val in args.config:
            tb_config[key] = val
    #Open up a logging test runner
    with open(os.path.join(args.log_dir, args.test), "w") as log:
        forker = fprime.gse.testing.support.StreamForker([sys.stderr, log])
        trunner = fprime.gse.testing.runner.FPrimeTestRunner(verbosity=args.verbose,
                                                             stream=forker,
                                                             interactive=args.interactive,
                                                             testbed=tb_config,
                                                             no_color=args.no_color,
                                                             list_only=args.list_only)
        #Run using standard pyunit
        try:
            argsv = [sys.argv[0], args.test]
            unittest.main(module=None, testRunner=trunner, verbosity=args.verbose, argv=argsv)
        except AttributeError as ate:
            if not args.test.split(".")[-1] in str(ate):
                raise
            test_help = ".TestSet.test_".join(args.test.split(".test_"))
            argsv = [sys.argv[0], test_help]
            unittest.main(module=None, testRunner=trunner, verbosity=args.verbose, argv=argsv)

if __name__ == "__main__":
    main()
