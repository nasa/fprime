""" File containing checksum implementations and function to calculate checksum implementation

fprime has historically supported several types of checksums. The primary is CRC32 and the constant testing-only
checksum, which was a constant.
"""
import zlib


def crc_calculation(data: bytes):
    """ Initial checksum implementation for FpFramerDeframer. """
    return zlib.crc32(data) & 0xffffffff


CHECKSUM_SELECTION = "crc32"
CHECKSUM_MAPPING = {
    "fixed": lambda data: 0xcafecafe,
    "crc32": crc_calculation,
    "default": crc_calculation
}


def calculate_checksum(data: bytes):
    """ Calculates the checksum of bytes """
    selected_checksum = CHECKSUM_SELECTION
    hash_fn = CHECKSUM_MAPPING.get(selected_checksum, CHECKSUM_MAPPING.get("default"))
    return hash_fn(data)
