#!/usr/bin/env python3
"""
Convert CSV file to binary DP operations file format.

This script converts a CSV file containing data product operations into the
binary format expected by the DpCatalog PROCESS_DP_FILE command.

CSV Format:
    Operation,ID,tSec,tSub,Priority

Where:
    Operation: DELETE, REPRIORITIZE, or RETRANSMIT
    ID: Data product ID (32-bit unsigned integer)
    tSec: Generation time in seconds (32-bit unsigned integer)
    tSub: Generation time in subseconds (32-bit unsigned integer)
    Priority: Priority value (32-bit unsigned integer, 0xFFFFFFFF for RETRANSMIT means use file priority)

Binary Format:
    Each record is 17 bytes:
    Offset | Size | Field      | Description
    -------|------|------------|-------------
    0      | 1    | Operation  | 1=DELETE, 2=REPRIORITIZE, 3=RETRANSMIT
    1      | 4    | ID         | Data product ID (U32, big-endian)
    5      | 4    | tSec       | Time seconds (U32, big-endian)
    9      | 4    | tSub       | Time subseconds (U32, big-endian)
    13     | 4    | Priority   | Priority value (U32, big-endian)

    After all records, a single 4-byte CRC32 checksum (big-endian) covers all record data.

Example CSV:
    Operation,ID,tSec,tSub,Priority
    DELETE,123,1000,100,0
    REPRIORITIZE,234,2000,200,5
    RETRANSMIT,345,3000,300,10
    RETRANSMIT,456,4000,400,4294967295

Author: Generated for F' DpCatalog component
"""

import argparse
import csv
import struct
import sys
import zlib
from pathlib import Path
from typing import List, Tuple

# Operation name to code mapping
OPERATION_MAP = {"DELETE": 1, "REPRIORITIZE": 2, "RETRANSMIT": 3}

# Maximum value for U32
MAX_U32 = 0xFFFFFFFF


class DpOperationError(Exception):
    """Exception raised for errors in DP operations."""

    pass


def parse_operation(op_str: str) -> int:
    """
    Parse operation string and return operation code.

    Args:
        op_str: Operation string (case-insensitive)

    Returns:
        Operation code (1, 2, or 3)

    Raises:
        DpOperationError: If operation is invalid
    """
    op_upper = op_str.strip().upper()
    if op_upper not in OPERATION_MAP:
        raise DpOperationError(
            f"Invalid operation '{op_str}'. Must be one of: {', '.join(OPERATION_MAP.keys())}"
        )
    return OPERATION_MAP[op_upper]


def parse_u32(value_str: str, field_name: str) -> int:
    """
    Parse a string as a 32-bit unsigned integer.

    Args:
        value_str: String representation of the number
        field_name: Name of the field (for error messages)

    Returns:
        Parsed U32 value

    Raises:
        DpOperationError: If value is invalid
    """
    try:
        value = int(value_str, 0)  # Support decimal, hex (0x...), octal (0o...)
    except ValueError:
        raise DpOperationError(
            f"Invalid {field_name} '{value_str}': must be an integer"
        )

    if value < 0 or value > MAX_U32:
        raise DpOperationError(
            f"Invalid {field_name} '{value}': must be between 0 and {MAX_U32}"
        )

    return value


def parse_csv_row(row: dict, line_num: int) -> Tuple[int, int, int, int, int]:
    """
    Parse a CSV row into operation fields.

    Args:
        row: Dictionary from CSV DictReader
        line_num: Line number (for error messages)

    Returns:
        Tuple of (op_code, id, tSec, tSub, priority)

    Raises:
        DpOperationError: If row is invalid
    """
    try:
        op_code = parse_operation(row["Operation"])
        dp_id = parse_u32(row["ID"], "ID")
        t_sec = parse_u32(row["tSec"], "tSec")
        t_sub = parse_u32(row["tSub"], "tSub")
        priority = parse_u32(row["Priority"], "Priority")

        return (op_code, dp_id, t_sec, t_sub, priority)

    except KeyError as e:
        raise DpOperationError(f"Missing required column: {e}")
    except DpOperationError as e:
        raise DpOperationError(f"Line {line_num}: {e}")


def create_binary_record(
    op_code: int, dp_id: int, t_sec: int, t_sub: int, priority: int
) -> bytes:
    """
    Create a 17-byte binary record.

    Args:
        op_code: Operation code (1-3)
        dp_id: Data product ID
        t_sec: Time seconds
        t_sub: Time subseconds
        priority: Priority value

    Returns:
        17-byte binary record
    """
    # Format: B = unsigned char (1 byte), I = unsigned int (4 bytes, big-endian)
    # '>' prefix means big-endian
    return struct.pack(">BIIII", op_code, dp_id, t_sec, t_sub, priority)


def convert_csv_to_binary(
    csv_path: Path, output_path: Path, verbose: bool = False
) -> None:
    """
    Convert CSV file to binary operations file.

    Args:
        csv_path: Path to input CSV file
        output_path: Path to output binary file
        verbose: Print verbose output

    Raises:
        DpOperationError: If conversion fails
    """
    if not csv_path.exists():
        raise DpOperationError(f"Input file not found: {csv_path}")

    records = []

    # Read and parse CSV
    with open(csv_path, "r", newline="") as csv_file:
        reader = csv.DictReader(csv_file)

        # Validate header
        required_fields = {"Operation", "ID", "tSec", "tSub", "Priority"}
        if not required_fields.issubset(set(reader.fieldnames or [])):
            raise DpOperationError(
                f"CSV must have columns: {', '.join(sorted(required_fields))}"
            )

        # Parse each row
        for line_num, row in enumerate(
            reader, start=2
        ):  # Start at 2 (header is line 1)
            try:
                op_code, dp_id, t_sec, t_sub, priority = parse_csv_row(row, line_num)
                record = create_binary_record(op_code, dp_id, t_sec, t_sub, priority)
                records.append(record)

                if verbose:
                    op_name = [k for k, v in OPERATION_MAP.items() if v == op_code][0]
                    print(
                        f"Line {line_num}: {op_name} ID={dp_id} tSec={t_sec} tSub={t_sub} Priority={priority}"
                    )

            except DpOperationError as e:
                raise DpOperationError(f"Error parsing CSV: {e}")

    if not records:
        raise DpOperationError("No records found in CSV file")

    # Concatenate all records
    all_data = b"".join(records)

    # Calculate CRC32 over all record data
    # zlib.crc32 returns signed int on some platforms, mask to unsigned
    crc32 = zlib.crc32(all_data) & 0xFFFFFFFF

    # Write binary file with records + CRC32
    with open(output_path, "wb") as bin_file:
        bin_file.write(all_data)
        bin_file.write(struct.pack(">I", crc32))

    if verbose:
        print(f"\nSuccessfully converted {len(records)} record(s) to {output_path}")
        print(
            f"Output file size: {len(all_data) + 4} bytes ({len(records)} records + 4-byte CRC32)"
        )
        print(f"CRC32: 0x{crc32:08X}")


def main() -> int:
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="Convert CSV file to binary DP operations format",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Example CSV file:
    Operation,ID,tSec,tSub,Priority
    DELETE,123,1000,100,0
    REPRIORITIZE,234,2000,200,5
    RETRANSMIT,345,3000,300,10

Valid operations:
    DELETE        - Remove DP from catalog and delete file
    REPRIORITIZE  - Change DP priority in catalog
    RETRANSMIT    - Re-add transmitted DP to catalog for retransmission
                   (Priority 4294967295 = use priority from file)
        """,
    )

    parser.add_argument("csv_file", type=Path, help="Input CSV file path")

    parser.add_argument("output_file", type=Path, help="Output binary file path")

    parser.add_argument(
        "-v", "--verbose", action="store_true", help="Print verbose output"
    )

    args = parser.parse_args()

    try:
        convert_csv_to_binary(args.csv_file, args.output_file, args.verbose)
        return 0

    except DpOperationError as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

    except Exception as e:
        print(f"Unexpected error: {e}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
