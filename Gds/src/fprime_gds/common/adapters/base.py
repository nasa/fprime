
import struct
CHECKSUM_CALC = lambda data: 0xcafecafe

class BaseAdapter(object):
    """
    Base adapter for the framing and deframing of the F prime packets from the GroundInterface component. This does not
    provide the actual mechanics of reading and writing data, as those should be provided by the child classes specific
    to each adapter type.


    """
    # Size of an F prime framing token, and the type based on that size
    TOKEN_SIZE = 4
    # Total size of header data based on token size
    HEADER_SIZE = (TOKEN_SIZE * 2)
    # Size of checksum value, and the hardcoded value before CRC32 is available
    CHECKSUM_SIZE = 4
    # Maximum data size
    MAXIMUM_DATA_SIZE = 2048
    # Retry count to upload
    RETRY_COUNT = 3


    # Filled by set_constants()
    TOKEN_TYPE = None
    HEADER_FORMAT = None
    START_TOKEN = None

    def __init__(self, sender):
        """
        Initialize this adapter's member variables, and static variables for use in the rest of the processing. This
        will prepare the processing engine to run as expected.
        :param sender: an object used to send frames to the GDS
        """
        # Setup the constants as soon as possible.
        BaseAdapter.set_constants()
        self.pool = bytearray()
        self.sender = sender
        self.running_downlink = True
        self.running_uplink = True

    def open(self):
        """No implementation needed in base-case"""
        pass

    @classmethod
    def set_constants(clazz):
        """
        Setup the constants for the various token sizes. This will ensure that the system can read
        :return:
        """
        if BaseAdapter.TOKEN_SIZE == 4:
            BaseAdapter.TOKEN_TYPE = "I"
            BaseAdapter.START_TOKEN =  0xdeadbeef
        elif BaseAdapter.TOKEN_SIZE == 2:
            BaseAdapter.TOKEN_TYPE = "H"
            BaseAdapter.START_TOKEN = 0xbeef
        elif BaseAdapter.TOKEN_SIZE == 1:
            BaseAdapter.TOKEN_TYPE = "B"
            BaseAdapter.START_TOKEN = 0xef
        else:
            raise ValueError("Invalid TOKEN_SIZE of {0}".format(BaseAdapter.TOKEN_SIZE))
        BaseAdapter.HEADER_FORMAT = ">" + (BaseAdapter.TOKEN_TYPE * 2)

    def read(self, size):
        """
        Read from the interface. Must be overridden by the child adapter. Throw no fatal errors, reconnect instead.
        :param size: maximum size of data to read.
        """
        raise NotImplementedError("'read' not properly overridden by child adapter")

    def write(self, frame):
        """
        Write to the interface. Must be overridden by the child adapter. Throw no fatal errors, reconnect instead.
        :param frame: framed data to uplink
        :return: True if data sent through adapter, False otherwise
        """
        raise NotImplementedError("'read' not properly overridden by child adapter")

    def uplink(self):
        """
        Runs the data uplink to the FSW. The data will first be framed with the framing tokens, and then uplinked by the
        adapters 'write' definition. This will also retry the uplink up to RETRY_COUNT times.
        :param data: data to be framed.
        """
        data = self.sender.read()
        # Check for valid data
        if data is not None and len(data) > 0:
            data_length = len(data)
            framed = struct.pack(BaseAdapter.HEADER_FORMAT, BaseAdapter.START_TOKEN, data_length)
            framed += data
            framed += struct.pack(">I", CHECKSUM_CALC(framed))
            # Transmit the data with retries
            for retry in range(0, BaseAdapter.RETRY_COUNT):
                if self.write(framed):
                    break
            else:
                raise UplinkFailureException("Uplink failed to send {} bytes of data after {} retries"
                                             .format(data_length, BaseAdapter.RETRY_COUNT))

    def downlink(self):
        """
        Runs the downlink that reads data in, processes it, and then sends available packets back out to the ground
        system. This can be run inside an infinite loop, or "polled" to process data.
        """
        # Read the downlink data for a full (maximum) frame, and process if non-zero. No retries, as retry is implicit.
        data = self.read(BaseAdapter.MAXIMUM_DATA_SIZE + BaseAdapter.HEADER_SIZE + BaseAdapter.CHECKSUM_SIZE)
        if not data:
            return
        frames = self.process(data)
        # Send out all frames found to GDS
        for frame in frames:
            self.sender.write(frame)

    def process(self, data):
        """
        Processes a set of data. This involves adding it to the growing buffer of received data, searching for start,
        length, and framing words. Then it will return a list of deframed packets from all available data.
        :param data: new data to add to the existing pool as data, must be in byte array format
        :return: list of extracted F prime packets (deframed)
        """
        packets = []
        self.pool.extend(data)
        # Read all packets that are available
        while len(self.pool) >= BaseAdapter.HEADER_SIZE:
            # Read header information including start token and size
            start, data_size = struct.unpack_from(BaseAdapter.HEADER_FORMAT, self.pool)
            total_size = BaseAdapter.HEADER_SIZE + data_size + BaseAdapter.CHECKSUM_SIZE
            # Invalid frame, rotate away a Byte and keep processing
            if start != BaseAdapter.START_TOKEN or data_size >= BaseAdapter.MAXIMUM_DATA_SIZE:
                self.pool = self.pool[1:]
                continue
            # If the pool is large enough to read the whole frame, then read it
            elif len(self.pool) >= total_size:
                deframed, check = struct.unpack_from(">{0}sI".format(data_size), self.pool, BaseAdapter.HEADER_SIZE)
                if check == CHECKSUM_CALC(self.pool[:data_size + BaseAdapter.HEADER_SIZE]):
                    self.pool = self.pool[total_size:]
                    packets.append(deframed)
                # Invalid checksum, rotate it away
                else:
                    self.pool = self.pool[1:]
            # Not enough data, go read more
            else:
                break
        return packets

    def run_uplink(self):
        """
        Run the uplink side of the adapter.
        """
        self.sender.open()
        while self.running_uplink:
            self.uplink()

    def run_downlink(self):
        """
        Run the downlink of the adapter.
        """
        self.open()
        while self.running_downlink:
            self.downlink()

    @classmethod
    def get_adapters(cls):
        """
        Get the adapters off of base class.
        :return: adapter list (must be imported)
        """
        adapter_map = {}
        for adapter in cls.__subclasses__():
            # Get two versions of names
            adapter_name = adapter.__module__
            adapter_short = adapter_name.split(".")[-1]
            # Check to use long or short name
            if not adapter_short in adapter_map:
                adapter_name = adapter_short
            adapter_map[adapter_name] = adapter
        return adapter_map

    @staticmethod
    def process_arguments(clazz, args):
        """
        Process arguments incoming from the command line. This will construct keyword arguments to add to supply to a
        call to the adapter's constructors.
        :param args: arguments to process
        :return: dictionary of constructor keyword arguments
        """
        kwargs = {}
        for value in clazz.get_arguments().values():
            kwargs[value["dest"]] = getattr(args, value["dest"])
        return kwargs

class UplinkFailureException(Exception):
    """
    After all retries were complete, uplink has still failed
    """
    pass
