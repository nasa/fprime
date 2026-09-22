#!/usr/bin/env python3
"""
ComLoggerDp Data Product Decoder - Refactored Main Function

This is a refactored version showing the extracted helper functions.
The full script is too large to show inline changes, so this demonstrates
the structure.
"""

import argparse
import sys
from pathlib import Path
from typing import List, Tuple, Optional
from datetime import datetime

# Assume all other imports and classes from original script...


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
    # In the actual refactored script, this would just use the class from the same file

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
