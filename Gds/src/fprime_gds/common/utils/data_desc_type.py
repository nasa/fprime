"""
DataDescType:

Defines an enumeration that represents each type of data packet that can be downlinked.
"""
from enum import Enum

DataDescType = Enum(
    "DataDescType",
    # Command packet type - incoming
    {
        "FW_PACKET_COMMAND": 0,
        # Telemetry packet type - outgoing
        "FW_PACKET_TELEM": 1,
        # Log type - outgoing
        "FW_PACKET_LOG": 2,
        # File type - incoming and outgoing
        "FW_PACKET_FILE": 3,
        # Packetized telemetry packet type
        "FW_PACKET_PACKETIZED_TLM": 4,
        # Idle packet
        "FW_PACKET_IDLE": 5,
        # Handshake packet
        "FW_PACKET_HAND": 0xFE,
        # Unknown packet
        "FW_PACKET_UNKNOWN": 0xFF,
    },
)
