#!/usr/bin/env python3
"""
ComLoggerDp Data Product Decoder

This script decodes F Prime ComLoggerDp data product binary files directly,
extracting the original ComBuffer packets and decoding them into events and
telemetry channels without requiring intermediate JSON dictionary files.

Usage:
    python decode_comlogger_dp.py <dp_binary_file> [options]

Requirements:
    - fprime-gds package
    - Project dictionary files (loaded via fprime_gds)

Author: F Prime / Claude Code
Date: 2026-09-21
"""

import argparse
import json
import sys
from pathlib import Path
from typing import Dict, List, Any, Optional, Tuple
from io import BytesIO
import struct
from datetime import datetime

# F Prime GDS imports
try:
    from fprime_gds.common.dp.common import (
        get_dp_header_type,
        calculate_crc32,
        ChecksumConfig,
    )
    from fprime_gds.common.utils.config_manager import ConfigManager
    from fprime_gds.common.models.dictionaries import Dictionaries
    from fprime_gds.common.models.serialize.type_base import ValueType
    from fprime_gds.common.models.serialize.time_type import TimeType
except ImportError as e:
    print(
        f"Error: fprime-gds package not found. Please install it first.",
        file=sys.stderr,
    )
    print(f"  pip install fprime-gds", file=sys.stderr)
    sys.exit(1)


# ==============================================================================
# Exceptions
# ==============================================================================


class ComLoggerDpError(Exception):
    """Base exception for ComLoggerDp decoding errors."""

    pass


class CRCError(ComLoggerDpError):
    """CRC validation failure."""

    def __init__(self, section: str, expected: int, calculated: int):
        self.section = section
        self.expected = expected
        self.calculated = calculated
        super().__init__(
            f"CRC mismatch in {section}: expected {expected:#010x}, got {calculated:#010x}"
        )


class SentryError(ComLoggerDpError):
    """Sentry validation failure."""

    def __init__(self, expected: int, actual: int, record_index: int):
        self.expected = expected
        self.actual = actual
        self.record_index = record_index
        super().__init__(
            f"Sentry mismatch in record {record_index}: expected {expected:#010x}, got {actual:#010x}"
        )


# ==============================================================================
# ComLoggerDp Decoder
# ==============================================================================


class ComLoggerDpDecoder:
    """Decoder for ComLoggerDp data product binary files.

    This decoder:
    1. Parses the DP header and validates CRC
    2. Extracts ComBuffer records with sentry validation
    3. Decodes each ComBuffer into telemetry/event packets
    4. Outputs decoded data in JSON or human-readable format
    """

    def __init__(
        self,
        dp_binary_path: str,
        dictionaries: Dictionaries,
        sentry_value: Optional[int] = None,
        validate_crc: bool = True,
        validate_sentry: bool = True,
    ):
        """Initialize the decoder.

        Args:
            dp_binary_path: Path to the ComLoggerDp data product binary file
            dictionaries: F Prime dictionaries object (events, channels, etc.)
            sentry_value: Expected sentry value (if None, will try to load from config)
            validate_crc: Whether to validate CRC checksums
            validate_sentry: Whether to validate sentry values
        """
        self.dp_binary_path = dp_binary_path
        self.dictionaries = dictionaries
        self.validate_crc = validate_crc
        self.validate_sentry = validate_sentry

        # Get type information from ConfigManager
        self.config_mgr = ConfigManager()

        # Get sentry value from config or use provided value
        if sentry_value is not None:
            self.sentry_value = sentry_value
        else:
            # Try to load from project config
            try:
                self.sentry_value = self.config_mgr.get_constant("ComLoggerDpSentry")
                print(f"Loaded sentry value from config: {self.sentry_value:#010x}")
            except:
                # Default sentry if not found in config
                self.sentry_value = 0xDEADBEEF
                print(
                    f"Warning: ComLoggerDpSentry not found in config, using default: {self.sentry_value:#010x}"
                )

        # Packet type enumeration from ComCfg
        self.PACKET_TYPES = {
            0x0000: "FW_PACKET_COMMAND",
            0x0001: "FW_PACKET_TELEM",
            0x0002: "FW_PACKET_LOG",
            0x0003: "FW_PACKET_FILE",
            0x0004: "FW_PACKET_PACKETIZED_TLM",
            0x0005: "FW_PACKET_DP",
            0x0006: "FW_PACKET_IDLE",
            0x0007: "FW_PACKET_PARAM",
            0x00FE: "FW_PACKET_HAND",
            0x00FF: "FW_PACKET_UNKNOWN",
            0x07FF: "SPP_IDLE_PACKET",
        }

    def decode_header(self, file_handle) -> Dict[str, Any]:
        """Decode the data product header.

        Args:
            file_handle: Binary file handle positioned at start of DP file

        Returns:
            Dictionary containing header fields

        Raises:
            CRCError: If header CRC validation fails
        """
        header = get_dp_header_type()()
        header_size = header.getMaxSize()
        header_bin_data = file_handle.read(header_size)

        if len(header_bin_data) < header_size:
            raise ComLoggerDpError(
                f"Incomplete header: expected {header_size} bytes, got {len(header_bin_data)}"
            )

        header.deserialize(header_bin_data, 0)
        header_json = header.to_jsonable()

        # Validate CRC if enabled
        if self.validate_crc:
            computed_hash = calculate_crc32(
                header_bin_data[: header_size - ChecksumConfig.CHECKSUM_LEN]
            )
            stored_hash = header_json["Checksum"]["value"]
            if stored_hash != computed_hash:
                raise CRCError("Header", stored_hash, computed_hash)

        return header_json

    def extract_combuffer_records(
        self, file_handle, data_size: int
    ) -> List[Dict[str, Any]]:
        """Extract all ComBuffer records from the data section.

        Args:
            file_handle: Binary file handle positioned at start of data section
            data_size: Total size of data section in bytes

        Returns:
            List of dictionaries containing:
                - record_index: Sequential record number
                - record_id: DP record ID (should be 0 for ComBufferRecord)
                - sentry: Sentry value found
                - combuffer_data: Raw ComBuffer bytes

        Raises:
            SentryError: If sentry validation fails
        """
        records = []
        position_at_start = file_handle.tell()
        record_index = 0

        # Get type sizes
        record_id_type = self.config_mgr.get_type("FwDpIdType")()
        record_id_size = record_id_type.getSize()

        size_type = self.config_mgr.get_type("FwSizeStoreType")()
        size_type_size = size_type.getSize()

        sentry_size = 4  # U32

        while (file_handle.tell() - position_at_start) < data_size:
            # Read record ID
            record_id_bin = file_handle.read(record_id_size)
            if len(record_id_bin) == 0:
                break  # End of data
            if len(record_id_bin) < record_id_size:
                raise ComLoggerDpError(f"Incomplete record ID at record {record_index}")

            record_id_obj = self.config_mgr.get_type("FwDpIdType")()
            record_id_obj.deserialize(record_id_bin, 0)
            record_id = record_id_obj.val

            # Read array size (total bytes including sentry + ComBuffer data)
            array_size_bin = file_handle.read(size_type_size)
            if len(array_size_bin) < size_type_size:
                raise ComLoggerDpError(
                    f"Incomplete array size at record {record_index}"
                )

            array_size_obj = self.config_mgr.get_type("FwSizeStoreType")()
            array_size_obj.deserialize(array_size_bin, 0)
            array_size = array_size_obj.val

            # Read the record data (sentry + ComBuffer)
            record_data = file_handle.read(array_size)
            if len(record_data) < array_size:
                raise ComLoggerDpError(
                    f"Incomplete record data at record {record_index}: expected {array_size}, got {len(record_data)}"
                )

            # Extract sentry value (first 4 bytes, U32 big-endian)
            sentry_bytes = record_data[:sentry_size]
            sentry = struct.unpack(">I", sentry_bytes)[0]  # Big-endian U32

            # Validate sentry if enabled
            if self.validate_sentry and sentry != self.sentry_value:
                raise SentryError(self.sentry_value, sentry, record_index)

            # Extract ComBuffer data (remaining bytes)
            combuffer_data = record_data[sentry_size:]

            records.append(
                {
                    "record_index": record_index,
                    "record_id": record_id,
                    "sentry": sentry,
                    "combuffer_size": len(combuffer_data),
                    "combuffer_data": combuffer_data,
                }
            )

            record_index += 1

        return records

    def decode_combuffer(self, combuffer_data: bytes) -> Tuple[int, str, bytes]:
        """Decode a ComBuffer to extract packet type and data.

        Args:
            combuffer_data: Raw ComBuffer bytes

        Returns:
            Tuple of (packet_type_value, packet_type_name, packet_data)
        """
        if len(combuffer_data) < 2:
            raise ComLoggerDpError(f"ComBuffer too short: {len(combuffer_data)} bytes")

        # First 2 bytes are FwPacketDescriptorType (U16, big-endian)
        packet_type = struct.unpack(">H", combuffer_data[:2])[0]
        packet_type_name = self.PACKET_TYPES.get(
            packet_type, f"UNKNOWN_{packet_type:#06x}"
        )
        packet_data = combuffer_data[2:]

        return packet_type, packet_type_name, packet_data

    def decode_telemetry_packet(self, packet_data: bytes) -> Dict[str, Any]:
        """Decode a telemetry channel packet.

        Args:
            packet_data: Packet data bytes (after packet type)

        Returns:
            Dictionary containing decoded telemetry information
        """
        try:
            buffer = BytesIO(packet_data)

            # Read channel ID
            id_type = self.config_mgr.get_type("FwChanIdType")()
            id_bytes = buffer.read(id_type.getSize())
            id_type.deserialize(id_bytes, 0)
            channel_id = id_type.val

            # Read time tag
            time_type = TimeType()
            time_bytes = buffer.read(time_type.getMaxSize())
            time_type.deserialize(time_bytes, 0)
            time_dict = time_type.to_jsonable()

            # Look up channel in dictionary
            channel_template = self.dictionaries.channel_id.get(channel_id)

            if channel_template is None:
                return {
                    "channel_id": channel_id,
                    "channel_name": f"UNKNOWN_CHANNEL_{channel_id}",
                    "time": time_dict,
                    "value": f"<raw: {buffer.read().hex()}>",
                    "error": "Channel not found in dictionary",
                }

            # Deserialize channel value
            channel_type_class = channel_template.get_type_obj()
            channel_type = channel_type_class()
            value_bytes = buffer.read(channel_type.getMaxSize())
            channel_type.deserialize(value_bytes, 0)

            return {
                "channel_id": channel_id,
                "channel_name": channel_template.get_full_name(),
                "channel_comp": channel_template.get_comp_name(),
                "time": time_dict,
                "value": channel_type.to_jsonable(),
            }

        except Exception as e:
            return {
                "error": f"Failed to decode telemetry packet: {str(e)}",
                "raw_data": packet_data.hex(),
            }

    def decode_event_packet(self, packet_data: bytes) -> Dict[str, Any]:
        """Decode an event log packet.

        Args:
            packet_data: Packet data bytes (after packet type)

        Returns:
            Dictionary containing decoded event information
        """
        try:
            buffer = BytesIO(packet_data)

            # Read event ID
            id_type = self.config_mgr.get_type("FwEventIdType")()
            id_bytes = buffer.read(id_type.getSize())
            id_type.deserialize(id_bytes, 0)
            event_id = id_type.val

            # Read time tag
            time_type = TimeType()
            time_bytes = buffer.read(time_type.getMaxSize())
            time_type.deserialize(time_bytes, 0)
            time_dict = time_type.to_jsonable()

            # Look up event in dictionary
            event_template = self.dictionaries.event_id.get(event_id)

            if event_template is None:
                return {
                    "event_id": event_id,
                    "event_name": f"UNKNOWN_EVENT_{event_id}",
                    "severity": "UNKNOWN",
                    "time": time_dict,
                    "args": [],
                    "message": f"<raw: {buffer.read().hex()}>",
                    "error": "Event not found in dictionary",
                }

            # Deserialize event arguments
            args = []
            arg_tuples = event_template.get_args()
            for arg_tuple in arg_tuples:
                # arg_tuple is (name, description, type_class)
                arg_name = arg_tuple[0]
                arg_type_class = arg_tuple[2]
                arg_type = arg_type_class()
                arg_bytes = buffer.read(arg_type.getMaxSize())
                arg_type.deserialize(arg_bytes, 0)
                args.append({"name": arg_name, "value": arg_type.to_jsonable()})

            # Format the message
            try:
                format_str = event_template.get_format_str()
                arg_values = [
                    (
                        arg["value"]["value"]
                        if isinstance(arg["value"], dict) and "value" in arg["value"]
                        else arg["value"]
                    )
                    for arg in args
                ]
                message = format_str.format(*arg_values) if arg_values else format_str
            except:
                message = format_str

            return {
                "event_id": event_id,
                "event_name": event_template.get_full_name(),
                "event_comp": event_template.get_comp_name(),
                "severity": event_template.get_severity(),
                "time": time_dict,
                "args": args,
                "message": message,
                "format_string": event_template.get_format_str(),
            }

        except Exception as e:
            return {
                "error": f"Failed to decode event packet: {str(e)}",
                "raw_data": packet_data.hex(),
            }

    @staticmethod
    def format_time_gds(time_dict: Dict[str, Any]) -> Tuple[str, str]:
        """Format time in GDS log format.

        Args:
            time_dict: Time dictionary with seconds and microseconds

        Returns:
            Tuple of (ISO timestamp, F Prime time tag)
        """
        seconds = time_dict.get("seconds", 0)
        microseconds = time_dict.get("microseconds", 0)
        context = time_dict.get("context", 0)
        base = time_dict.get("base", 2)

        # Convert to datetime
        dt = datetime.utcfromtimestamp(seconds + microseconds / 1000000.0)
        iso_time = dt.strftime("%Y-%m-%dT%H:%M:%S.%f")

        # F Prime time tag format: (base(context)-seconds:microseconds)
        fprime_time = f"({base}({context})-{seconds}:{microseconds})"

        return iso_time, fprime_time

    @staticmethod
    def format_value_gds(value: Any) -> str:
        """Format a value for GDS log output.

        Args:
            value: The value to format (dict, str, number, bool)

        Returns:
            Formatted string value
        """
        if isinstance(value, dict):
            if "value" in value:
                val = value["value"]
                # Handle enums
                if (
                    isinstance(val, str)
                    and not val.replace(".", "").replace("_", "").isalnum()
                ):
                    return val
                # Handle numeric values
                if isinstance(val, (int, float)):
                    return str(val)
                if isinstance(val, bool):
                    return str(val)
                return str(val)
            # Handle complex types like arrays
            if "values" in value:
                return str(value["values"])
            return str(value)
        return str(value)

    def decode_packet(
        self, packet_type: int, packet_type_name: str, packet_data: bytes
    ) -> Dict[str, Any]:
        """Decode a packet based on its type.

        Args:
            packet_type: Numeric packet type value
            packet_type_name: String name of packet type
            packet_data: Packet data bytes

        Returns:
            Dictionary containing decoded packet information
        """
        if packet_type == 0x0001:  # FW_PACKET_TELEM
            return self.decode_telemetry_packet(packet_data)
        elif packet_type == 0x0002:  # FW_PACKET_LOG
            return self.decode_event_packet(packet_data)
        elif packet_type == 0x0004:  # FW_PACKET_PACKETIZED_TLM
            return self.decode_telemetry_packet(packet_data)
        else:
            return {
                "warning": f"Unsupported packet type: {packet_type_name}",
                "raw_data": (
                    packet_data.hex()[:100] + "..."
                    if len(packet_data) > 50
                    else packet_data.hex()
                ),
            }

    def reconstruct_from_sentinels(self) -> Dict[str, Any]:
        """Reconstruct records from a corrupted file by scanning for sentinels.

        This method searches for sentry values in the binary file and attempts
        to extract and decode ComBuffer data between sentinels. It's useful for
        recovering data from corrupted or partial data product files.

        Returns:
            Dictionary containing recovered records and statistics

        Raises:
            FileNotFoundError: If binary file doesn't exist
        """
        results = {
            "file_info": {"path": self.dp_binary_path, "size": 0},
            "reconstruction": {
                "sentinels_found": 0,
                "records_extracted": 0,
                "records_decoded": 0,
                "bytes_scanned": 0,
            },
            "records": [],
            "statistics": {
                "packet_types": {},
                "events": 0,
                "telemetry": 0,
                "errors": 0,
            },
        }

        print(f"Starting reconstruction mode on {self.dp_binary_path}")
        print(f"Scanning for sentry value: {self.sentry_value:#010x}\n")

        # Sentry is big-endian U32
        sentry_bytes = struct.pack(">I", self.sentry_value)
        sentry_size = 4

        with open(self.dp_binary_path, "rb") as f:
            # Read entire file
            file_data = f.read()
            file_size = len(file_data)
            results["file_info"]["size"] = file_size
            results["reconstruction"]["bytes_scanned"] = file_size

            print(f"File size: {file_size} bytes")

            # Find all sentry positions
            sentry_positions = []
            pos = 0
            while pos < len(file_data):
                pos = file_data.find(sentry_bytes, pos)
                if pos == -1:
                    break
                sentry_positions.append(pos)
                pos += 1  # Move past this sentry to find next

            results["reconstruction"]["sentinels_found"] = len(sentry_positions)
            print(f"Found {len(sentry_positions)} sentinel(s)")

            if len(sentry_positions) == 0:
                print("No sentinels found - cannot reconstruct records")
                return results

            print("\nExtracting records between sentinels...\n")

            # Extract and decode data between sentinels
            for idx in range(len(sentry_positions)):
                sentry_pos = sentry_positions[idx]

                # Determine end of this record
                if idx + 1 < len(sentry_positions):
                    # Next sentinel marks the end of this record
                    next_sentry = sentry_positions[idx + 1]
                    record_data = file_data[sentry_pos + sentry_size : next_sentry]
                else:
                    # Last sentinel - take rest of file (or reasonable chunk)
                    # Use a max chunk size to avoid reading garbage
                    max_record_size = 4096  # Maximum reasonable ComBuffer size
                    end_pos = min(
                        sentry_pos + sentry_size + max_record_size, len(file_data)
                    )
                    record_data = file_data[sentry_pos + sentry_size : end_pos]

                if len(record_data) == 0:
                    continue

                results["reconstruction"]["records_extracted"] += 1

                print(
                    f"[Sentinel {idx}] Position: {sentry_pos}, Data size: {len(record_data)} bytes"
                )

                try:
                    # Try to decode the ComBuffer
                    packet_type, packet_type_name, packet_data = self.decode_combuffer(
                        record_data
                    )

                    # Track packet type statistics
                    results["statistics"]["packet_types"][packet_type_name] = (
                        results["statistics"]["packet_types"].get(packet_type_name, 0)
                        + 1
                    )

                    # Decode packet based on type
                    decoded_packet = self.decode_packet(
                        packet_type, packet_type_name, packet_data
                    )

                    # Track statistics
                    if packet_type == 0x0002:  # Events
                        results["statistics"]["events"] += 1
                    elif packet_type in [0x0001, 0x0004]:  # Telemetry
                        results["statistics"]["telemetry"] += 1

                    if "error" in decoded_packet:
                        results["statistics"]["errors"] += 1
                        print(f"  └─ Decoded: {packet_type_name} (decode error)")
                    else:
                        results["reconstruction"]["records_decoded"] += 1
                        if packet_type == 0x0002:  # Event
                            event_name = decoded_packet.get("event_name", "UNKNOWN")
                            print(f"  └─ Decoded: {packet_type_name} - {event_name}")
                        elif packet_type in [0x0001, 0x0004]:  # Telemetry
                            channel_name = decoded_packet.get("channel_name", "UNKNOWN")
                            print(f"  └─ Decoded: {packet_type_name} - {channel_name}")
                        else:
                            print(f"  └─ Decoded: {packet_type_name}")

                    # Add to results
                    results["records"].append(
                        {
                            "sentinel_index": idx,
                            "file_position": sentry_pos,
                            "sentry": f"{self.sentry_value:#010x}",
                            "data_size": len(record_data),
                            "packet_type": packet_type_name,
                            "packet_type_value": packet_type,
                            "decoded": decoded_packet,
                        }
                    )

                except Exception as e:
                    results["statistics"]["errors"] += 1
                    print(f"  └─ Error: {str(e)}")
                    results["records"].append(
                        {
                            "sentinel_index": idx,
                            "file_position": sentry_pos,
                            "sentry": f"{self.sentry_value:#010x}",
                            "data_size": len(record_data),
                            "error": f"Failed to decode: {str(e)}",
                            "raw_data": (
                                record_data[:100].hex() + "..."
                                if len(record_data) > 50
                                else record_data.hex()
                            ),
                        }
                    )

        return results

    def decode(self) -> Dict[str, Any]:
        """Decode the entire ComLoggerDp data product file.

        Returns:
            Dictionary containing header, records, and decoded packets

        Raises:
            FileNotFoundError: If binary file doesn't exist
            CRCError: If CRC validation fails
            ComLoggerDpError: For other decoding errors
        """
        results = {
            "header": None,
            "records": [],
            "statistics": {
                "total_records": 0,
                "packet_types": {},
                "events": 0,
                "telemetry": 0,
                "errors": 0,
            },
        }

        with open(self.dp_binary_path, "rb") as f:
            # Decode header
            print("Decoding DP header...")
            header_json = self.decode_header(f)
            results["header"] = header_json

            # Extract ComBuffer records
            print("Extracting ComBuffer records...")
            data_size = header_json["DataSize"]["value"]
            position_at_start = f.tell()
            combuffer_records = self.extract_combuffer_records(f, data_size)
            results["statistics"]["total_records"] = len(combuffer_records)
            print(f"Found {len(combuffer_records)} ComBuffer records")

            # Validate data CRC
            if self.validate_crc:
                print("Validating data CRC...")
                assert f.tell() == position_at_start + data_size
                dp_crc_bin = f.read(ChecksumConfig.CHECKSUM_LEN)
                dp_crc = ChecksumConfig.CHECKSUM_TOKEN_TYPE()
                dp_crc.deserialize(dp_crc_bin, 0)

                f.seek(position_at_start)
                data_to_crc = f.read(data_size)
                computed_crc = calculate_crc32(data_to_crc)

                if computed_crc != dp_crc.val:
                    raise CRCError("Data", dp_crc.val, computed_crc)
                print("Data CRC validated successfully")

            # Decode each ComBuffer
            print("Decoding ComBuffers...")
            for record in combuffer_records:
                try:
                    # Decode ComBuffer to get packet type and data
                    packet_type, packet_type_name, packet_data = self.decode_combuffer(
                        record["combuffer_data"]
                    )

                    # Track packet type statistics
                    results["statistics"]["packet_types"][packet_type_name] = (
                        results["statistics"]["packet_types"].get(packet_type_name, 0)
                        + 1
                    )

                    # Decode packet based on type
                    decoded_packet = self.decode_packet(
                        packet_type, packet_type_name, packet_data
                    )

                    # Track statistics
                    if packet_type == 0x0002:  # Events
                        results["statistics"]["events"] += 1
                    elif packet_type in [0x0001, 0x0004]:  # Telemetry
                        results["statistics"]["telemetry"] += 1

                    if "error" in decoded_packet:
                        results["statistics"]["errors"] += 1

                    # Add to results
                    results["records"].append(
                        {
                            "record_index": record["record_index"],
                            "record_id": record["record_id"],
                            "sentry": f"{record['sentry']:#010x}",
                            "packet_type": packet_type_name,
                            "packet_type_value": packet_type,
                            "decoded": decoded_packet,
                        }
                    )

                except Exception as e:
                    results["statistics"]["errors"] += 1
                    results["records"].append(
                        {
                            "record_index": record["record_index"],
                            "error": f"Failed to decode ComBuffer: {str(e)}",
                            "raw_data": record["combuffer_data"].hex()[:100],
                        }
                    )

        return results

    def process_to_json(self, output_path: Optional[str] = None) -> str:
        """Decode the DP file and write JSON output.

        Args:
            output_path: Output JSON file path (default: input_file.json)

        Returns:
            Path to output JSON file
        """
        if output_path is None:
            output_path = str(Path(self.dp_binary_path).with_suffix(".json"))

        print(f"\nDecoding {self.dp_binary_path}...")
        data = self.decode()

        print(f"\nWriting results to {output_path}...")
        with open(output_path, "w") as f:
            json.dump(data, f, indent=2, default=str)

        print("\n=== Decoding Summary ===")
        print(f"Total records: {data['statistics']['total_records']}")
        print(f"Events: {data['statistics']['events']}")
        print(f"Telemetry: {data['statistics']['telemetry']}")
        print(f"Errors: {data['statistics']['errors']}")
        print(f"\nPacket type breakdown:")
        for ptype, count in data["statistics"]["packet_types"].items():
            print(f"  {ptype}: {count}")

        return output_path

    def process_to_text(self, output_path: Optional[str] = None) -> str:
        """Decode the DP file and write human-readable text output.

        Args:
            output_path: Output text file path (default: input_file.txt)

        Returns:
            Path to output text file
        """
        if output_path is None:
            output_path = str(Path(self.dp_binary_path).with_suffix(".txt"))

        print(f"\nDecoding {self.dp_binary_path}...")
        data = self.decode()

        print(f"\nWriting results to {output_path}...")
        with open(output_path, "w") as f:
            f.write("=" * 80 + "\n")
            f.write("ComLoggerDp Data Product Decoder Output\n")
            f.write("=" * 80 + "\n\n")

            # Header information
            f.write("HEADER:\n")
            f.write("-" * 80 + "\n")
            header = data["header"]

            # Helper to extract value from header field
            def get_value(field):
                if isinstance(field, dict):
                    if "value" in field:
                        return field["value"]
                    elif "seconds" in field and "microseconds" in field:
                        # Time field
                        return f"{field['seconds']}.{field['microseconds']:06d}"
                    elif "values" in field:
                        # Array field - show first few values
                        vals = field["values"]
                        if all(v == 0 for v in vals):
                            return "[all zeros]"
                        return (
                            f"[{', '.join(map(str, vals[:8]))}...]"
                            if len(vals) > 8
                            else f"[{', '.join(map(str, vals))}]"
                        )
                return str(field)

            f.write(f"Container ID: {get_value(header.get('Id', 'N/A'))}\n")
            f.write(f"Priority: {get_value(header.get('Priority', 'N/A'))}\n")
            f.write(f"Time: {get_value(header.get('Time', 'N/A'))}\n")
            f.write(f"Data Size: {get_value(header.get('DataSize', 'N/A'))} bytes\n")
            f.write(f"User Data: {get_value(header.get('UserData', 'N/A'))}\n\n")

            # Records
            f.write("DECODED PACKETS:\n")
            f.write("-" * 80 + "\n\n")

            for record in data["records"]:
                f.write(f"[Record {record['record_index']}] ")

                if "error" in record:
                    f.write(f"ERROR: {record['error']}\n\n")
                    continue

                f.write(f"{record['packet_type']}\n")
                decoded = record["decoded"]

                if "error" in decoded:
                    f.write(f"  ERROR: {decoded['error']}\n\n")
                    continue

                # Format based on packet type
                if record["packet_type"] == "FW_PACKET_LOG":
                    # Event
                    f.write(f"  Event: {decoded.get('event_name', 'UNKNOWN')}\n")
                    f.write(f"  Severity: {decoded.get('severity', 'UNKNOWN')}\n")
                    f.write(f"  Time: {decoded.get('time', 'N/A')}\n")
                    f.write(f"  Message: {decoded.get('message', '')}\n")
                    if decoded.get("args"):
                        f.write(f"  Args:\n")
                        for arg in decoded["args"]:
                            f.write(f"    {arg['name']}: {arg['value']}\n")

                elif record["packet_type"] in [
                    "FW_PACKET_TELEM",
                    "FW_PACKET_PACKETIZED_TLM",
                ]:
                    # Telemetry
                    f.write(f"  Channel: {decoded.get('channel_name', 'UNKNOWN')}\n")
                    f.write(f"  Time: {decoded.get('time', 'N/A')}\n")
                    f.write(f"  Value: {decoded.get('value', 'N/A')}\n")

                else:
                    f.write(f"  {json.dumps(decoded, indent=4)}\n")

                f.write("\n")

            # Statistics
            f.write("=" * 80 + "\n")
            f.write("STATISTICS:\n")
            f.write("-" * 80 + "\n")
            stats = data["statistics"]
            f.write(f"Total records: {stats['total_records']}\n")
            f.write(f"Events: {stats['events']}\n")
            f.write(f"Telemetry: {stats['telemetry']}\n")
            f.write(f"Errors: {stats['errors']}\n\n")
            f.write("Packet type breakdown:\n")
            for ptype, count in stats["packet_types"].items():
                f.write(f"  {ptype}: {count}\n")

        return output_path

    def process_reconstruction_to_json(self, output_path: Optional[str] = None) -> str:
        """Reconstruct records from corrupted file and write JSON output.

        Args:
            output_path: Output JSON file path (default: input_file_reconstructed.json)

        Returns:
            Path to output JSON file
        """
        if output_path is None:
            input_file = Path(self.dp_binary_path)
            output_path = str(
                input_file.parent / f"{input_file.stem}_reconstructed.json"
            )

        print(f"\n{'='*80}")
        data = self.reconstruct_from_sentinels()

        print(f"\n{'='*80}")
        print(f"Writing results to {output_path}...")
        with open(output_path, "w") as f:
            json.dump(data, f, indent=2, default=str)

        print("\n=== Reconstruction Summary ===")
        print(f"File size: {data['file_info']['size']} bytes")
        print(f"Sentinels found: {data['reconstruction']['sentinels_found']}")
        print(f"Records extracted: {data['reconstruction']['records_extracted']}")
        print(f"Records decoded: {data['reconstruction']['records_decoded']}")
        print(f"Events: {data['statistics']['events']}")
        print(f"Telemetry: {data['statistics']['telemetry']}")
        print(f"Errors: {data['statistics']['errors']}")
        print(f"\nPacket type breakdown:")
        for ptype, count in data["statistics"]["packet_types"].items():
            print(f"  {ptype}: {count}")

        return output_path

    def process_reconstruction_to_text(self, output_path: Optional[str] = None) -> str:
        """Reconstruct records from corrupted file and write text output.

        Args:
            output_path: Output text file path (default: input_file_reconstructed.txt)

        Returns:
            Path to output text file
        """
        if output_path is None:
            input_file = Path(self.dp_binary_path)
            output_path = str(
                input_file.parent / f"{input_file.stem}_reconstructed.txt"
            )

        print(f"\n{'='*80}")
        data = self.reconstruct_from_sentinels()

        print(f"\n{'='*80}")
        print(f"Writing results to {output_path}...")
        with open(output_path, "w") as f:
            f.write("=" * 80 + "\n")
            f.write("ComLoggerDp Reconstruction Mode Output\n")
            f.write("=" * 80 + "\n\n")

            # File information
            f.write("FILE INFORMATION:\n")
            f.write("-" * 80 + "\n")
            f.write(f"Path: {data['file_info']['path']}\n")
            f.write(f"Size: {data['file_info']['size']} bytes\n\n")

            # Reconstruction statistics
            f.write("RECONSTRUCTION:\n")
            f.write("-" * 80 + "\n")
            f.write(f"Sentinels found: {data['reconstruction']['sentinels_found']}\n")
            f.write(
                f"Records extracted: {data['reconstruction']['records_extracted']}\n"
            )
            f.write(f"Records decoded: {data['reconstruction']['records_decoded']}\n")
            f.write(f"Bytes scanned: {data['reconstruction']['bytes_scanned']}\n\n")

            # Recovered records
            f.write("RECOVERED RECORDS:\n")
            f.write("-" * 80 + "\n\n")

            for record in data["records"]:
                f.write(
                    f"[Sentinel {record['sentinel_index']}] Position: {record['file_position']}, "
                )
                f.write(f"Size: {record.get('data_size', 0)} bytes\n")

                if "error" in record:
                    f.write(f"  ERROR: {record['error']}\n\n")
                    continue

                f.write(f"  Type: {record.get('packet_type', 'UNKNOWN')}\n")
                decoded = record["decoded"]

                if "error" in decoded:
                    f.write(f"  ERROR: {decoded['error']}\n\n")
                    continue

                # Format based on packet type
                if record.get("packet_type") == "FW_PACKET_LOG":
                    # Event
                    f.write(f"  Event: {decoded.get('event_name', 'UNKNOWN')}\n")
                    f.write(f"  Severity: {decoded.get('severity', 'UNKNOWN')}\n")
                    f.write(f"  Time: {decoded.get('time', 'N/A')}\n")
                    f.write(f"  Message: {decoded.get('message', '')}\n")
                    if decoded.get("args"):
                        f.write(f"  Args:\n")
                        for arg in decoded["args"]:
                            f.write(f"    {arg['name']}: {arg['value']}\n")

                elif record.get("packet_type") in [
                    "FW_PACKET_TELEM",
                    "FW_PACKET_PACKETIZED_TLM",
                ]:
                    # Telemetry
                    f.write(f"  Channel: {decoded.get('channel_name', 'UNKNOWN')}\n")
                    f.write(f"  Time: {decoded.get('time', 'N/A')}\n")
                    f.write(f"  Value: {decoded.get('value', 'N/A')}\n")

                else:
                    f.write(f"  {json.dumps(decoded, indent=4)}\n")

                f.write("\n")

            # Statistics
            f.write("=" * 80 + "\n")
            f.write("STATISTICS:\n")
            f.write("-" * 80 + "\n")
            stats = data["statistics"]
            f.write(f"Events: {stats['events']}\n")
            f.write(f"Telemetry: {stats['telemetry']}\n")
            f.write(f"Errors: {stats['errors']}\n\n")
            f.write("Packet type breakdown:\n")
            for ptype, count in stats["packet_types"].items():
                f.write(f"  {ptype}: {count}\n")

        return output_path


# ==============================================================================
# Main Entry Point
# ==============================================================================




# ==============================================================================
# Main Entry Point - Helper Functions
# ==============================================================================

def parse_args():
    """Parse and return command-line arguments.

    Returns:
        argparse.Namespace: Parsed command-line arguments
    """
    parser = argparse.ArgumentParser(
        description="Decode ComLoggerDp data product binary files",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Decode a single DP file
  python decode_comlogger_dp.py --input-file data_product.fdp

  # Single file with custom output
  python decode_comlogger_dp.py --input-file data_product.fdp --output-file decoded.json

  # Batch process directory
  python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728

  # Batch with custom output directory
  python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 0x08000000 --output-dir /decoded

  # Specify dictionary path
  python decode_comlogger_dp.py --input-file data_product.fdp --dict-path /path/to/dict

  # Different output formats
  python decode_comlogger_dp.py --input-file data_product.fdp --format json
  python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728 --format text

  # Override sentry value
  python decode_comlogger_dp.py --input-file data_product.fdp --sentry 0x12345678

  # Skip validation checks
  python decode_comlogger_dp.py --input-file data_product.fdp --no-crc --no-sentry
        """,
    )

    # Input options
    input_group = parser.add_mutually_exclusive_group(required=True)
    input_group.add_argument(
        "--input-file", help="Path to a single ComLoggerDp data product binary file"
    )
    input_group.add_argument(
        "--input-dir",
        help="Path to directory containing ComLoggerDp data product files",
    )

    # Configuration options
    parser.add_argument(
        "--dict-path",
        "-d",
        help="Path to F Prime dictionary directory (auto-detected if not specified)",
    )
    parser.add_argument(
        "--dp-id",
        type=lambda x: int(x, 0),  # Allows 0x prefix
        help="Container ID to filter (e.g., 134217728 or 0x08000000). Required when using --input-dir.",
    )

    # Output options
    parser.add_argument(
        "--output-file",
        help="Output file path for single file processing (extension added based on format)",
    )
    parser.add_argument(
        "--output-dir",
        help="Output directory for batch processing (default: same as input directory)",
    )
    parser.add_argument(
        "--format",
        "-f",
        choices=["json", "text", "both"],
        default="both",
        help="Output format (default: both)",
    )

    # Validation options
    parser.add_argument(
        "--sentry",
        type=lambda x: int(x, 0),  # Allows 0x prefix
        help="Override sentry value (e.g., 0xDEADBEEF)",
    )
    parser.add_argument("--no-crc", action="store_true", help="Skip CRC validation")
    parser.add_argument(
        "--no-sentry", action="store_true", help="Skip sentry validation"
    )

    # Recovery mode
    parser.add_argument(
        "--reconstruct",
        action="store_true",
        help="Reconstruction mode: scan for sentinels and extract partial records from corrupted files. Only valid with --input-file.",
    )

    # Collection mode
    parser.add_argument(
        "--collect",
        action="store_true",
        help="Collection mode: aggregate all data products into GDS-style event.log and channel.log files. Only valid with --input-dir.",
    )
    parser.add_argument(
        "--collect-dir",
        help="Base directory for collected logs (default: current directory). Timestamped subdirectory will be created.",
    )

    return parser.parse_args()


def validate_mode(args) -> Tuple[Path, bool, str]:
    """Validate arguments and determine operating mode.

    Args:
        args: Parsed command-line arguments

    Returns:
        Tuple of (input_path, is_directory, mode_description)

    Raises:
        SystemExit: If validation fails
    """
    # Determine input path and mode
    if args.input_file:
        input_path = Path(args.input_file)
        is_directory = False
        mode_description = "single file"
    else:  # args.input_dir
        input_path = Path(args.input_dir)
        is_directory = True
        mode_description = "directory"

    # Check if input path exists
    if not input_path.exists():
        print(f"Error: Input path not found: {input_path}", file=sys.stderr)
        sys.exit(1)

    # Validate arguments based on mode
    if is_directory:
        # Directory mode validations
        if not args.dp_id:
            print(f"Error: --dp-id is required when using --input-dir", file=sys.stderr)
            sys.exit(1)
        if args.output_file:
            print(
                f"Error: --output-file cannot be used with --input-dir. Use --output-dir instead.",
                file=sys.stderr,
            )
            sys.exit(1)
        if args.reconstruct:
            print(
                f"Error: --reconstruct can only be used with --input-file (single file mode)",
                file=sys.stderr,
            )
            sys.exit(1)
        if args.collect:
            # Collect mode ignores --format and --output-dir
            if args.format != "both":
                print(
                    f"Warning: --format is ignored in --collect mode", file=sys.stderr
                )
            if args.output_dir:
                print(
                    f"Warning: --output-dir is ignored in --collect mode. Use --collect-dir instead.",
                    file=sys.stderr,
                )
    else:
        # Single file mode validations
        if args.dp_id:
            print(
                f"Warning: --dp-id is ignored when using --input-file", file=sys.stderr
            )
        if args.output_dir:
            print(
                f"Warning: --output-dir is ignored when using --input-file. Use --output-file instead.",
                file=sys.stderr,
            )
        if args.collect:
            print(
                f"Error: --collect can only be used with --input-dir (directory mode)",
                file=sys.stderr,
            )
            sys.exit(1)

    return input_path, is_directory, mode_description


def find_input_files(
    input_path: Path, is_directory: bool, dp_id: Optional[int]
) -> Tuple[List[Path], Optional[Path]]:
    """Discover input files to process.

    Args:
        input_path: Base input path (file or directory)
        is_directory: Whether input_path is a directory
        dp_id: Container ID to filter (required for directory mode)

    Returns:
        Tuple of (files_to_process, output_dir)

    Raises:
        SystemExit: If no files found
    """
    if is_directory:
        # Find all files matching the pattern Dp_<container_id>_*.fdp
        dp_id_str = str(dp_id)
        pattern = f"Dp_{dp_id_str}_*.fdp"
        files_to_process = list(input_path.glob(pattern))

        # Also try with .bin extension
        pattern_bin = f"Dp_{dp_id_str}_*.bin"
        files_to_process.extend(list(input_path.glob(pattern_bin)))

        if not files_to_process:
            print(
                f"Error: No data product files found matching container ID {dp_id} in {input_path}",
                file=sys.stderr,
            )
            print(f"  Searched for: {pattern} and {pattern_bin}", file=sys.stderr)
            sys.exit(1)

        # Sort files by name
        files_to_process.sort()
        print(
            f"Found {len(files_to_process)} data product file(s) with container ID {dp_id}"
        )

        # Determine output directory - will be used by caller
        output_dir = input_path
    else:
        files_to_process = [input_path]
        output_dir = None
        print(f"Processing single file: {input_path}")

    return files_to_process, output_dir


def load_dictionaries(dict_path: Optional[str], input_path: Path):
    """Load F Prime dictionaries with auto-detection.

    Args:
        dict_path: Optional explicit dictionary path
        input_path: Input path for auto-detection

    Returns:
        Dictionaries object
    """
    from fprime_gds.common.models.dictionaries import Dictionaries

    print("\nLoading F Prime dictionaries...")

    # Find dictionary JSON file
    dict_json_path = None
    if dict_path:
        dict_path_obj = Path(dict_path)
        if dict_path_obj.is_file() and dict_path_obj.suffix == ".json":
            dict_json_path = dict_path_obj
        elif dict_path_obj.is_dir():
            # Look for JSON dictionary in directory
            json_files = list(dict_path_obj.glob("*Dictionary.json"))
            if json_files:
                dict_json_path = json_files[0]
                print(f"Found dictionary: {dict_json_path.name}")
            else:
                print(f"Warning: No *Dictionary.json file found in {dict_path_obj}")
    else:
        # Try to auto-detect dictionary path
        possible_dirs = [
            input_path.parent / "dict",
            input_path.parent.parent / "dict",
            Path.cwd() / "dict",
        ]
        for dir_path in possible_dirs:
            if dir_path.exists() and dir_path.is_dir():
                json_files = list(dir_path.glob("*Dictionary.json"))
                if json_files:
                    dict_json_path = json_files[0]
                    print(f"Auto-detected dictionary: {dict_json_path}")
                    break

    # Load dictionaries
    if dict_json_path:
        print(f"Loading dictionary from: {dict_json_path}")
        dictionaries = Dictionaries.load_dictionaries_into_config(str(dict_json_path))
    else:
        print("Warning: Could not find dictionary JSON file. Some decoding may fail.")
        print("Specify dictionary with --dict-path /path/to/dictionary.json")
        dictionaries = Dictionaries()

    return dictionaries


def run_collection_mode(args, files_to_process: List[Path], dictionaries):
    """Run collection mode: aggregate all DPs into GDS-style logs.

    Args:
        args: Parsed command-line arguments
        files_to_process: List of DP files to process
        dictionaries: F Prime dictionaries

    Note:
        Exits the program after completing collection mode
    """
    # Import ComLoggerDpDecoder here to avoid circular dependency in the real script

    print("\n=== COLLECTION MODE ===")
    print("Aggregating data products into GDS-style logs...")

    # Create timestamped output directory
    now = datetime.now()
    timestamp = now.strftime("%Y_%m_%d-%H_%M_%S")
    collect_base = Path(args.collect_dir) if args.collect_dir else Path.cwd()
    collect_dir = collect_base / f"comlogger-dp-{timestamp}"
    collect_dir.mkdir(parents=True, exist_ok=True)

    print(f"Output directory: {collect_dir}")

    # Collect all events and channels from all files
    all_events = []
    all_channels = []
    files_processed = 0
    files_failed = 0

    for file_path in files_to_process:
        try:
            # This would use ComLoggerDpDecoder from the same file
            decoder = ComLoggerDpDecoder(
                str(file_path),
                dictionaries,
                sentry_value=args.sentry,
                validate_crc=not args.no_crc,
                validate_sentry=not args.no_sentry,
            )

            # Decode the file
            data = decoder.decode()

            # Extract events and channels
            for record in data["records"]:
                if "decoded" in record and "error" not in record["decoded"]:
                    decoded = record["decoded"]
                    packet_type = record.get("packet_type")

                    if packet_type == "FW_PACKET_LOG":
                        # Event
                        all_events.append(decoded)
                    elif packet_type in [
                        "FW_PACKET_TELEM",
                        "FW_PACKET_PACKETIZED_TLM",
                    ]:
                        # Channel
                        all_channels.append(decoded)

            files_processed += 1
        except Exception as e:
            print(f"Warning: Failed to process {file_path.name}: {e}")
            files_failed += 1

    print(f"\nProcessed {files_processed} files, {files_failed} failed")
    print(f"Collected {len(all_events)} events, {len(all_channels)} channels")

    # Sort by timestamp
    def get_time_key(item):
        time_dict = item.get("time", {})
        seconds = time_dict.get("seconds", 0)
        microseconds = time_dict.get("microseconds", 0)
        return (seconds, microseconds)

    all_events.sort(key=get_time_key)
    all_channels.sort(key=get_time_key)

    # Write event.log
    event_log_path = collect_dir / "events-dp.log"
    with open(event_log_path, "w") as f:
        for event in all_events:
            iso_time, fprime_time = ComLoggerDpDecoder.format_time_gds(
                event.get("time", {})
            )
            event_name = event.get("event_name", "UNKNOWN")
            event_id = event.get("event_id", 0)
            severity = event.get("severity", "UNKNOWN")
            message = event.get("message", "")

            f.write(
                f"{iso_time},{fprime_time},{event_name},{event_id},{severity},{message}\n"
            )

    # Write channel.log
    channel_log_path = collect_dir / "channels-dp.log"
    with open(channel_log_path, "w") as f:
        for channel in all_channels:
            iso_time, fprime_time = ComLoggerDpDecoder.format_time_gds(
                channel.get("time", {})
            )
            channel_name = channel.get("channel_name", "UNKNOWN")
            channel_id = channel.get("channel_id", 0)
            value = ComLoggerDpDecoder.format_value_gds(channel.get("value", ""))

            f.write(f"{iso_time},{fprime_time},{channel_name},{channel_id},{value}\n")

    print(f"\n=== OUTPUT ===")
    print(f"Events log:   {event_log_path}")
    print(f"Channels log: {channel_log_path}")
    print(f"\nCollection completed successfully!")

    sys.exit(0)


def run_decode_mode(
    args,
    files_to_process: List[Path],
    output_dir: Optional[Path],
    is_directory: bool,
    dictionaries,
):
    """Run decode mode: process files and output JSON/text.

    Args:
        args: Parsed command-line arguments
        files_to_process: List of files to decode
        output_dir: Output directory for batch processing
        is_directory: Whether processing directory or single file
        dictionaries: F Prime dictionaries

    Returns:
        Tuple of (successful_count, failed_count)
    """
    # Resolve output directory for batch mode
    if is_directory and output_dir is None:
        output_dir = files_to_process[0].parent
    if output_dir:
        output_dir.mkdir(parents=True, exist_ok=True)

    # Use explicit output_dir from args if provided
    if args.output_dir and is_directory:
        output_dir = Path(args.output_dir)
        output_dir.mkdir(parents=True, exist_ok=True)

    total_files = len(files_to_process)
    successful = 0
    failed = 0

    for idx, file_path in enumerate(files_to_process, 1):
        if total_files > 1:
            print(f"\n{'='*80}")
            print(f"Processing file {idx}/{total_files}: {file_path.name}")
            print("=" * 80)

        try:
            # Create decoder for this file
            decoder = ComLoggerDpDecoder(
                str(file_path),
                dictionaries,
                sentry_value=args.sentry,
                validate_crc=not args.no_crc,
                validate_sentry=not args.no_sentry,
            )

            # Determine output paths
            if is_directory:
                # For directory processing, put outputs in output_dir
                base_name = file_path.stem  # filename without extension
                output_json = (
                    str(output_dir / f"{base_name}.json")
                    if args.format in ["json", "both"]
                    else None
                )
                output_text = (
                    str(output_dir / f"{base_name}.txt")
                    if args.format in ["text", "both"]
                    else None
                )
            else:
                # For single file, use --output-file if specified
                output_json = (
                    args.output_file
                    if args.output_file and args.format == "json"
                    else None
                )
                output_text = (
                    args.output_file
                    if args.output_file and args.format == "text"
                    else None
                )

            # Process based on mode
            if args.reconstruct:
                # Reconstruction mode
                if args.format in ["json", "both"]:
                    json_path = decoder.process_reconstruction_to_json(output_json)
                    if total_files == 1:
                        print(f"\nJSON output written to: {json_path}")

                if args.format in ["text", "both"]:
                    text_path = decoder.process_reconstruction_to_text(output_text)
                    if total_files == 1:
                        print(f"Text output written to: {text_path}")
            else:
                # Normal mode
                if args.format in ["json", "both"]:
                    json_path = decoder.process_to_json(output_json)
                    if total_files == 1:
                        print(f"\nJSON output written to: {json_path}")

                if args.format in ["text", "both"]:
                    text_path = decoder.process_to_text(output_text)
                    if total_files == 1:
                        print(f"Text output written to: {text_path}")

            successful += 1

        except Exception as e:
            failed += 1
            print(f"Error processing {file_path.name}: {e}", file=sys.stderr)
            if total_files == 1:
                # Re-raise for single file to show full traceback
                raise

    # Print summary for batch processing
    if total_files > 1:
        print(f"\n{'='*80}")
        print("BATCH PROCESSING SUMMARY")
        print("=" * 80)
        print(f"Total files: {total_files}")
        print(f"Successful: {successful}")
        print(f"Failed: {failed}")
        if successful > 0:
            print(f"\nOutput directory: {output_dir}")

    if successful > 0:
        print("\nDecoding completed successfully!")

    return successful, failed


def main():
    """Main entry point for ComLoggerDp decoder."""
    try:
        # 1. Parse arguments
        args = parse_args()

        # 2. Validate mode and arguments
        input_path, is_directory, mode_description = validate_mode(args)

        # 3. Find input files
        files_to_process, output_dir = find_input_files(
            input_path, is_directory, args.dp_id
        )

        # 4. Load dictionaries
        dictionaries = load_dictionaries(args.dict_path, input_path)

        # 5. Run collection mode if requested (exits after completion)
        if is_directory and args.collect:
            run_collection_mode(args, files_to_process, dictionaries)

        # 6. Run decode mode (normal or reconstruction)
        successful, failed = run_decode_mode(
            args, files_to_process, output_dir, is_directory, dictionaries
        )

        # Exit with appropriate status
        if failed > 0 and successful == 0:
            sys.exit(1)

    except ComLoggerDpError as e:
        print(f"\nError: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"\nUnexpected error: {e}", file=sys.stderr)
        import traceback

        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
