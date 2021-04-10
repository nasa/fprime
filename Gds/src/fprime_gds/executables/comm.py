"""
comm.py:

This is the F prime communications adapter. This allows the F prime ground tool suite to interact with running F prime
deployments that exist on the other end of a "wire" (some communication bus). This is done with the following mechanics:

1. An adapter is instantiated to handle "read" and "write" functions against the wire
2. A framer/deframer is instantiated in order to frame/deframe those packets as transported across the wire.
3. "Uplink" and "Downlink" threads are created to loop on data from flight (F prime) and ground (F prime ground)
   interfaces ensuring that ground data is framed and written to the wire, and flight data is deframed and sent to the
   ground side.

Note: assuming the module containing the ground adapter has been imported, then this code should provide it as a CLI
      argument, removing the need to rewrite most of this class to use something different.

@author lestarch
"""


import logging
import sys
import signal


# Required adapters built on standard tools
import fprime_gds.common.communication.adapters.base
import fprime_gds.common.communication.ground
import fprime_gds.common.communication.adapters.ip
import fprime_gds.common.logger
import fprime_gds.executables.cli

from fprime_gds.common.communication.framing import FpFramerDeframer
from fprime_gds.common.communication.updown import Downlinker, Uplinker

# Uses non-standard PIP package pyserial, so test the waters before getting a hard-import crash
try:
    import fprime_gds.common.communication.adapters.uart
except ImportError:
    pass


LOGGER = logging.getLogger("comm")


def main():
    """
    Main program, degenerates into the run loop.

    :return: return code
    """
    args, _ = fprime_gds.executables.cli.ParserBase.parse_args(
        [
            fprime_gds.executables.cli.LogDeployParser,
            fprime_gds.executables.cli.MiddleWareParser,
            fprime_gds.executables.cli.CommParser,
        ],
        description="F prime communications layer.",
        client=True,
    )
    # Create the handling components for either side of this script, adapter for hardware, and ground for the GDS side
    ground = fprime_gds.common.communication.ground.TCPGround(
        args.tts_addr, args.tts_port
    )
    adapter = args.comm_adapter

    # Set the framing class used and pass it to the uplink and downlink component constructions giving each a separate
    # instantiation
    framer_class = FpFramerDeframer
    downlinker = Downlinker(adapter, ground, framer_class())
    uplinker = Uplinker(adapter, ground, framer_class(), downlinker)

    # Open resources for the handlers on either side, this prepares the resources needed for reading/writing data
    ground.open()
    adapter.open()

    # Finally start the processing of uplink and downlink
    downlinker.start()
    uplinker.start()

    # Wait for shutdown event in the form of a KeyboardInterrupt then stop the processing, close resources, and wait for
    # everything to terminate as expected.
    def shutdown(*_):
        """ Shutdown function for signals"""
        uplinker.stop()
        downlinker.stop()
        ground.close()
        adapter.close()

    signal.signal(signal.SIGTERM, shutdown)
    signal.signal(signal.SIGINT, shutdown)
    uplinker.join()
    downlinker.join()
    return 0


if __name__ == "__main__":
    sys.exit(main())
