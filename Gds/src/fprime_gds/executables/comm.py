"""
Comm.py:

Communications application. This will run the communications layer.
"""
from __future__ import print_function
import sys
import argparse
import threading
import logging
import fprime_gds.common.logger

import fprime_gds.common.adapters.base
import fprime_gds.common.adapters.sender
import fprime_gds.common.adapters.ip
import fprime_gds.executables.cli

LOGGER = logging.getLogger("comm")
# Uses non-standard PIP package pyserial, so test the waters
try:
    import fprime_gds.common.adapters.uart
except ImportError:
    pass


def parse_args(args):
    """
    Parse the arguments to this application, then return the constructed namespace argument.
    :param args: list of arguments to parse
    :return: namespace argument
    """
    parser = argparse.ArgumentParser(description="Connects data from F prime flight software to the GDS tcp server")
    # Setup this parser to handle MiddleWare arguments
    fprime_gds.executables.cli.MiddleWareParser.add_args(parser)

    # Add a parser for each adapter
    subparsers = parser.add_subparsers(help="Type of adapter used for processing", dest="subcommand")
    for adapter_name in fprime_gds.common.adapters.base.BaseAdapter.get_adapters().keys():
        adapter = fprime_gds.common.adapters.base.BaseAdapter.get_adapters()[adapter_name]
        # Check adapter real quick before moving on
        if not hasattr(adapter, "get_arguments") or not callable(getattr(adapter, "get_arguments", None)):
            LOGGER.error("'{}' does not have 'get_arguments' method, skipping.".format(adapter_name))
            continue
        subparse = subparsers.add_parser(adapter_name)
        # Add arguments for the parser
        for argument in adapter.get_arguments().keys():
            subparse.add_argument(*argument, **adapter.get_arguments()[argument])
    args = parser.parse_args(args)
    try:
        extras = fprime_gds.executables.cli.refine(parser, args)
        fprime_gds.common.logger.configure_py_log(extras["logs"], "comm-adapter.log")
    except ValueError as exc:
        print("[ERROR] {}".format(exc), file=sys.stderr)
        parser.print_help(sys.stderr)
        sys.exit(-1)
    return args


def main(args=sys.argv):
    """
    Main program, degenerates into the run loop
    :param args: arguments
    :return: return code
    """
    args = parse_args(args[1:])
    sender = fprime_gds.common.adapters.sender.TCPSender(args.tts_addr, args.tts_port)
    # Create an adapter from input
    adapter = fprime_gds.common.adapters.base.BaseAdapter.get_adapters()[args.subcommand]
    adapted = adapter(sender, **fprime_gds.common.adapters.base.BaseAdapter.process_arguments(adapter, args))

    # Start-up threads
    threading.Thread(target=adapted.run_uplink).start()
    threading.Thread(target=adapted.run_downlink).start()

if __name__ == "__main__":
    sys.exit(main())
