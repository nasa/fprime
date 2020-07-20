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
from __future__ import print_function
import sys
import queue
import argparse
import threading
import logging
import fprime_gds.common.logger

# Required adapters built on standard tools
import fprime_gds.common.adapters.base
import fprime_gds.common.adapters.ground
import fprime_gds.common.adapters.ip
from fprime_gds.common.adapters.framing import FpFramerDeframer

from fprime.common.models.serialize.u32_type import U32Type
from fprime_gds.common.utils.data_desc_type import DataDescType

import fprime_gds.executables.cli

# Uses non-standard PIP package pyserial, so test the waters before getting a hard-import crash
try:
    import fprime_gds.common.adapters.uart
except ImportError:
    pass


LOGGER = logging.getLogger("comm")


class Uplinker(object):
    """
    Pulls out the code useful for uplink into a single designated place. This will run as a thread, which essentially
    calls does the following:

    1. Read packets of incoming ground system data
    2. Frame packet
    3. Uplink packet
    4. Repeat 2-3 until queue of packets is drained
    5. Repeat 1-4

    Note: this class also implements an uplink handshake to the other side of the ground system, to ensure that uplinked
    items do not roll this process over via the thundering herd.
    """

    RETRY_COUNT = 3

    def __init__(self, uplink_adapter, ground, downlinker):
        """
        Uplinker requires an adapter reference to flight side (outgoing data), ground side handler(incoming data), and a
        reference to the downlinker to queue return handshake packets.
        :param uplink_adapter: adapter used to talk to the flight side of the system
        :param ground: ground handler for sourcing uplink packets from the other ground components
        :param downlinker: downlinker object to return handshaking requests
        """
        self.uplink_adapter = uplink_adapter
        self.ground = ground
        self.downlink = downlinker
        self.running = True
        self.framer = FpFramerDeframer()

    def uplink(self):
        """
        Runs the data uplink to the FSW. The data will first be read from the ground handler, framed with the framing
        tokens, and then uplinked by the flight adapter's 'write' definition. This will also retry the uplink up to
        RETRY_COUNT times in case the write failed.
        :param data: data to be framed.
        """
        data_packets = self.ground.receive_all()
        for valid_packet in filter(
            lambda packet: packet is not None and len(packet) > 0, data_packets
        ):
            framed = self.framer.frame(valid_packet)
            for retry in range(0, Uplinker.RETRY_COUNT):
                if self.uplink_adapter.write(framed):
                    self.downlink.queue_downlink(Uplinker.get_handshake(valid_packet))
                    break
            else:
                raise UplinkFailureException(
                    "Uplink failed to send {} bytes of data after {} retries".format(
                        len(framed), Uplinker.RETRY_COUNT
                    )
                )

    def stop(self):
        """ Stop the thread depends will close the ground resource which may be blocking """
        self.running = False
        self.ground.close()

    @staticmethod
    def get_handshake(packet):
        """
        Gets a handshake raw frame. It repeats the last packet.
        :param packet: packet to repeat back out
        :return: handshake raw-frame
        """
        return U32Type(DataDescType["FW_PACKET_HAND"].value).serialize() + packet

    def run(self):
        """
        Method that run the uplinker thread. This should loop-forever. Will log any uplink errors that occur, but will
        not stop upon uplink error.
        """
        while self.running:
            try:
                self.uplink()
            except UplinkFailureException as ufe:
                LOGGER.warning("Uplink exception occured: %s", (ufe))
            # Shutdown exception handling, only keep exception when running
            except OSError:
                if self.running:
                    raise


class UplinkFailureException(Exception):
    """
    After all retries were complete, uplink has still failed
    """


class Downlinker(object):
    """
    Handles the actions associated with downlinking. This boils down to the following steps:

    1. Reading raw data from the raw data adapter facing the flight-side of the system
    2. Appending any new data to pool of available data
    3. Deframing all packets available from pool of available data
    4. Sending all deframed packets out to ground system
    5. Repeat steps 1-4
    """

    def __init__(self, downlink_adapter, ground):
        """
        Downlinker requires an adapter reference to flight side (incoming data), and ground side handler(outgoing data).
        :param uplink_adapter: adapter used to talk to the flight side of the system
        :param ground: ground handler for sinking downlink packets to the other ground components
        :param downlinker: downlinker object to return handshaking requests
        """
        self.downlink_adapter = downlink_adapter
        self.ground = ground
        self.running = True
        self.deframer = FpFramerDeframer()
        self.pool = b""
        self.enqueued = queue.Queue()

    def downlink(self):
        """
        Runs the actual downlink of data.
        """
        # Read the downlink data for a full (maximum) frame, and process if non-zero. No retries, as retry is implicit.
        self.pool += self.downlink_adapter.read()
        frames, self.pool = self.deframer.deframe_all(self.pool, no_copy=True)
        # Add all enqueued items. Implemented as a try-catch as "empty" makes no guarentees, so a try-catch is required.
        try:
            while not self.enqueued.empty():
                frames.append(self.enqueued.get_nowait())
        except queue.Empty:
            pass
        # Send out all frames found to GDS
        self.ground.send_all(frames)

    def stop(self):
        """ Stop the thread depends will close the ground resource which may be blocking """
        self.running = False
        self.downlink_adapter.close()

    def queue_downlink(self, frame):
        """
        Enqueues a frame to send as part of downlink. This should not require deframing.
        :param frame: frame to enqueue
        """
        self.enqueued.put(frame)

    def run(self):
        """
        Method that run the downlinker thread. This should loop-forever.
        """
        while self.running:
            self.downlink()


def parse_args(args):
    """
    Parse the arguments to this application, then return the constructed namespace argument.
    :param args: list of arguments to parse
    :return: namespace argument
    """
    parser = argparse.ArgumentParser(
        description="Connects data from F prime flight software to the GDS tcp server",
        # Setup this parser to handle MiddleWare arguments
        parents=[fprime_gds.executables.cli.MiddleWareParser.get_parser()],
    )

    # Add a parser for each adapter
    subparsers = parser.add_subparsers(
        help="Type of adapter used for processing", dest="subcommand"
    )
    for (
        adapter_name
    ) in fprime_gds.common.adapters.base.BaseAdapter.get_adapters().keys():
        adapter = fprime_gds.common.adapters.base.BaseAdapter.get_adapters()[
            adapter_name
        ]
        # Check adapter real quick before moving on
        if not hasattr(adapter, "get_arguments") or not callable(
            getattr(adapter, "get_arguments", None)
        ):
            LOGGER.error(
                "'%s' does not have 'get_arguments' method, skipping.", (adapter_name)
            )
            continue
        subparse = subparsers.add_parser(adapter_name)
        # Add arguments for the parser
        for argument in adapter.get_arguments().keys():
            subparse.add_argument(*argument, **adapter.get_arguments()[argument])
    try:
        args, extras = parser.parse_known_args(args)
        # Convert list of extra arguments to arg/value dict
        extras_it = iter(extras)
        extras_dict = dict(zip(extras_it, extras_it))
        fprime_gds.common.logger.configure_py_log(
            extras_dict["logs"], "comm-adapter.log"
        )
    except ValueError as exc:
        print("[ERROR] {}".format(exc), file=sys.stderr)
        parser.print_help(sys.stderr)
        sys.exit(-1)
    return args


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
    # First setup the ground handler to talk to the ground system.
    ground = fprime_gds.common.adapters.ground.TCPGround(args.tts_addr, args.tts_port)
    # Create an adapter from input arguments in order to talk to the flight deployment.
    adapter = args.comm_adapter
    # Create uplink and downlink handlers
    downlinker = Downlinker(adapter, ground)
    uplinker = Uplinker(adapter, ground, downlinker)
    # Open resources and fail if ground is unavailable
    ground.open()
    # Try to open adapted, but ignore failure as it should reconnect
    adapter.open()
    # Start-up threads to handle uplink and downlink
    down_thread = threading.Thread(target=downlinker.run)
    up_thread = threading.Thread(target=uplinker.run)
    down_thread.start()
    up_thread.start()
    # Join on the threads before exiting
    try:
        down_thread.join()
        up_thread.join()
    except KeyboardInterrupt:
        downlinker.stop()
        uplinker.stop()
    down_thread.join()
    up_thread.join()
    return 0


if __name__ == "__main__":
    sys.exit(main())
