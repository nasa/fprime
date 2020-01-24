from __future__ import print_function

import serial
import fprime_gds.common.adapters.base


class SerialAdapter(fprime_gds.common.adapters.base.BaseAdapter):
    """
    Supplies a data source adapter that is pulling data off from a UART wire using PySerial. This is setup using a
    device handle and a baudrate for the given serial device.
    """
    def __init__(self, sender, device, baud, timeout=0, reconnect=True):
        """
        Initialize the serial adapter using the default settings. This does not open the serial port, but sets up all
        the internal variables used when opening the device.
        """
        super(SerialAdapter, self).__init__(sender)
        self.device = device
        self.baud = baud
        self.reconnect = reconnect
        self.timeout = timeout
        self.serial = None

    def open(self):
        """
        Opens the serial port based on previously supplied settings. If the port is already open, then close it first.
        Then open the port up again.
        """
        self.close()
        self.serial = serial.Serial(self.device, self.baud, timeout=self.timeout)

    def close(self):
        """
        Close the serial device, and ignore any errors that might arrive when attempting that closure.
        """
        try:
            if self.serial is not None:
                self.serial.close()
        finally:
            self.serial = None

    def write(self, frame):
        """
        Send a given framed bit of data by sending it out the serial interface. It will attempt to reconnect if there is
        was a problem previously. This function will return true on success, or false on error.
        :param frame: framed data packet to send out
        :return: True, when data was sent through the UART. False otherwise.
        """
        try:
            if self.serial is None and self.reconnect:
                self.open()
            written = self.serial.write(frame)
            # Not believed to be possible to not send everything without getting a timeout exception
            assert written == len(frame)
            return True
        except serial.serialutil.SerialException:
            self.close()
        return False

    def read(self, size):
        """
        Read up to a given count in bytes from the UART adapter. This may return less than the full requested size but
        is expected to return some data.
        :param size: upper bound of data requested
        :return: data successfully read
        """
        try:
            if self.serial is None and self.reconnect:
                self.open()
            return self.serial.read(size)
        except serial.serialutil.SerialException as exc:
            self.close()
        return None

    @classmethod
    def get_arguments(cls):
        """
        Returns a dictionary of flag to argparse-argument dictionaries for use with argparse to setup arguments.
        :return: dictionary of flag to argparse arguments for use with argparse
        """
        return {
            ("-a", "--device"): {
                "dest": "device",
                "type": str,
                "default": "/dev/ttyACM0",
                "help": "UART device representing the FSW. Default: %(default)s"
            },
            ("-b", "--baud"): {
                "dest":"baud",
                "type":int,
                "default": 115200,
                "help": "Baud rate of the serial device. Default: %(default)s"
            }
        }
