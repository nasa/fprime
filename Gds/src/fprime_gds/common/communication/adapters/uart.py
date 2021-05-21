"""
uart.py:

This is the adapter for projects that would choose to use a serial UART interface for sending data from an F prime
deployment. This handles sending and receiving data from the things like 'LinuxSerialDriver' and other standard UART
drivers.

@author lestarch
"""

import logging

import fprime_gds.common.communication.adapters.base

import serial
from serial.tools import list_ports


LOGGER = logging.getLogger("serial_adapter")


class SerialAdapter(fprime_gds.common.communication.adapters.base.BaseAdapter):
    """
    Supplies a data source adapter that is pulling data off from a UART wire using PySerial. This is setup using a
    device handle and a baudrate for the given serial device.
    """

    BAUDS = [
        50,
        75,
        110,
        134,
        150,
        200,
        300,
        600,
        1200,
        1800,
        2400,
        4800,
        9600,
        19200,
        38400,
        57600,
        115200,
        230400,
        460800,
        500000,
        576000,
        921600,
        1000000,
        1152000,
        1500000,
        2000000,
        2500000,
        3000000,
        3500000,
        4000000,
    ]

    def __init__(self, device, baud):
        """
        Initialize the serial adapter using the default settings. This does not open the serial port, but sets up all
        the internal variables used when opening the device.
        """
        self.device = device
        self.baud = baud
        self.serial = None

    def open(self):
        """
        Opens the serial port based on previously supplied settings. If the port is already open, then close it first.
        Then open the port up again.
        """
        self.close()
        self.serial = serial.Serial(self.device, self.baud)
        return self.serial is not None

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
            if self.serial is None:
                self.open()
            written = self.serial.write(frame)
            # Not believed to be possible to not send everything without getting a timeout exception
            assert written == len(frame)
            return True
        except serial.serialutil.SerialException as exc:
            LOGGER.warning("Serial exception caught: %s. Reconnecting.", (str(exc)))
            self.close()
        return False

    def read(self, timeout=0.500):
        """
        Read up to a given count in bytes from the UART adapter. This may return less than the full requested size but
        is expected to return some data.

        :param timeout: timeout for reading data from the serial.
        :return: data successfully read
        """
        data = b""
        try:
            if self.serial is None:
                self.open()
            # Read as much data as possible, while ensuring to block if no data is available at this time. Note: as much
            # data is read as possible to avoid a long-return time to this call. Minimum data to read is one byte in
            # order to block this function while data is incoming.
            self.serial.timeout = timeout
            data = self.serial.read(1)  # Force a block for at least 1 character
            while self.serial.in_waiting:
                data += self.serial.read(
                    self.serial.in_waiting
                )  # Drain the incoming data queue
        except serial.serialutil.SerialException as exc:
            LOGGER.warning("Serial exception caught: %s. Reconnecting.", (str(exc)))
            self.close()
        return data

    @classmethod
    def get_arguments(cls):
        """
        Returns a dictionary of flag to argparse-argument dictionaries for use with argparse to setup arguments.

        :return: dictionary of flag to argparse arguments for use with argparse
        """
        available = list(
            map(lambda info: info.device, list_ports.comports(include_links=True))
        )
        default = "/dev/ttyACM0" if not available else available[-1]
        return {
            ("--uart-device",): {
                "dest": "device",
                "type": str,
                "default": default,
                "help": "UART device representing the FSW. Default: %(default)s",
            },
            ("--uart-baud",): {
                "dest": "baud",
                "type": int,
                "default": 9600,
                "help": "Baud rate of the serial device. Default: %(default)s",
            },
        }

    @classmethod
    def check_arguments(cls, args):
        """
        Code that should check arguments of this adapter. If there is a problem with this code, then a "ValueError"
        should be raised describing the problem with these arguments.

        :param args: arguments as dictionary
        """
        ports = map(lambda info: info.device, list_ports.comports(include_links=True))
        if not args["device"] in ports:
            raise ValueError(
                "Serial port '{}' not valid. Available ports: {}".format(
                    args["device"], ports
                )
            )
        # Note: baud rate may not *always* work. These are a superset
        try:
            baud = int(args["baud"])
        except ValueError:
            raise ValueError(
                "Serial baud rate '{}' not integer. Use one of: {}".format(
                    args["baud"], SerialAdapter.BAUDS
                )
            )
        if not int(baud) in SerialAdapter.BAUDS:
            raise ValueError(
                "Serial baud rate '{}' not supported. Use one of: {}".format(
                    baud, SerialAdapter.BAUDS
                )
            )
