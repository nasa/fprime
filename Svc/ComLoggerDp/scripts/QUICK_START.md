# Quick Start Guide

## Installation

```bash
# Ensure fprime-gds is installed
pip install fprime-gds
```

## Basic Usage

### Single File
```bash
# Decode a single ComLoggerDp data product file
python decode_comlogger_dp.py --input-file data_product.fdp

# Result: Creates data_product.json and data_product.txt
```

### Batch Processing (Directory)
```bash
# Decode all files with container ID 134217728 in a directory
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728

# Use hex notation
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 0x08000000

# Result: Processes all matching files and creates output in the same directory
```

### Collection Mode (GDS-Style Logs)
```bash
# Aggregate all data products into GDS-compatible log files
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728 --collect

# Result: Creates comlogger-dp-YYYY_MM_DD-HH_MM_SS/ with:
#   - events-dp.log (all events, sorted by time)
#   - channels-dp.log (all telemetry, sorted by time)
#   - README.txt (statistics and viewing instructions)
```

## Common Commands

```bash
# Single file - JSON output only
python decode_comlogger_dp.py --input-file data_product.fdp --format json

# Single file - Text output only
python decode_comlogger_dp.py --input-file data_product.fdp --format text

# Single file - Specify dictionary location
python decode_comlogger_dp.py --input-file data_product.fdp --dict-path /path/to/dict

# Single file - Custom output file
python decode_comlogger_dp.py --input-file data_product.fdp --output-file decoded.json

# Batch - Process directory with specific container ID
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 0x08000000 \
  --dict-path /path/to/dict

# Batch - Specify output directory
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728 \
  --output-dir /output/dir

# Collection - Aggregate into GDS-style logs
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728 --collect

# Collection - Specify output location
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728 \
  --collect --collect-dir /output/logs

# Override sentry value (if your deployment uses a custom value)
python decode_comlogger_dp.py --input-file data_product.fdp --sentry 0x12345678

# Skip validation (for corrupted files or testing)
python decode_comlogger_dp.py --input-file data_product.fdp --no-crc --no-sentry
```

## What Gets Decoded

The script extracts and decodes:

✅ **Events** - Full event messages with severity, timestamp, and arguments  
✅ **Telemetry Channels** - Channel values with timestamps  
✅ **Packet Metadata** - Packet types, sentry values, record indices  
✅ **Container Metadata** - Priority, container ID, timestamps

## Output Modes

### Individual Files (Default)

- JSON and/or text files per data product
- Detailed packet-level information
- Best for inspecting individual files

### Collection Mode (--collect)

- Single `events-dp.log` with all events
- Single `channels-dp.log` with all telemetry
- GDS-compatible format for analysis
- Best for post-mission analysis and trending

## Recovery Mode (--reconstruct)

For corrupted or partial files:

```bash
# Scan for sentinels and extract recoverable data
python decode_comlogger_dp.py --input-file corrupted.bin --reconstruct
```

**What it does:**
- Scans entire file for sentry values
- Skips garbage data before first sentinel
- Extracts and decodes data between sentinels
- Reports success/failure for each record
- Continues processing even with errors

## Output Formats

### JSON Output
- Complete structured data
- All metadata preserved
- Machine-readable
- Good for further processing

### Text Output
- Human-readable format
- Event messages formatted
- Easy to read in terminal
- Good for logs and debugging

## Troubleshooting

| Problem | Solution |
|---------|----------|
| "Dictionary not found" | Use `--dict-path /path/to/dict` |
| CRC validation error | Use `--no-crc` to skip validation |
| Sentry mismatch | Use `--sentry 0xVALUE` with correct value |
| "fprime-gds not found" | Run `pip install fprime-gds` |

## Example Workflow

```bash
# 1. Capture data products on spacecraft
# ComLoggerDp component records packets to data products

# 2. Download data product files to ground station

# 3. Decode with this script
cd /path/to/fprime/Svc/ComLoggerDp/scripts
python decode_comlogger_dp.py /path/to/downloaded.bin \
    --dict-path /path/to/project/dict \
    --format both

# 4. View results
cat /path/to/downloaded.txt          # Human-readable
jq . /path/to/downloaded.json        # JSON (requires jq)

# 5. Search for specific events
grep "ComDpStarted" /path/to/downloaded.txt

# 6. Extract telemetry values
jq '.records[] | select(.packet_type == "FW_PACKET_TELEM")' \
    /path/to/downloaded.json
```

## For More Details

See [README.md](README.md) for complete documentation including:
- Detailed binary structure reference
- Complete API documentation
- Advanced usage examples
- Integration with F Prime workflows
