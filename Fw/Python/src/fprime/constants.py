"""
Provided constants for use within the fprime system.
"""
# pylint: disable=W0105
"""
In order to transmit data in a serialized format, string objects need to be encoded. Otherwise,
it is unclear how the characters are translated into raw bytes on the wire. This value should be
consistent with the encoding used on the flight software that is being communicated with.

Traditional C/C++ strings typically use "ascii" encoding. Hence being used here.  However, should
F prime be updated to use some other encoding, this value may be changed.
"""
DATA_ENCODING = "utf-8"
