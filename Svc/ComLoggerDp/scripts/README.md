# ComLoggerDp Decoder Script

A Python script that decodes ComLoggerDp data product binary files directly into human-readable events and telemetry channels, **without** requiring intermediate JSON dictionary generation.

## Table of Contents

- [Quick Start](#quick-start)
- [Features](#features)
- [Installation](#installation)
- [Usage Modes](#usage-modes)
  - [Single File Processing](#single-file-processing)
  - [Batch Directory Processing](#batch-directory-processing)
  - [Collection Mode](#collection-mode-gds-style-log-aggregation)
  - [Reconstruction Mode](#reconstruction-mode-data-recovery)
- [Command Line Reference](#command-line-reference)
- [How It Works](#how-it-works)
- [Output Formats](#output-formats)
- [Examples](#examples)
- [Troubleshooting](#troubleshooting)

---

## Quick Start

### Installation

```bash
# Ensure fprime-gds is installed
pip install fprime-gds
```

### Basic Usage

```bash
# Decode a single file
python decode_comlogger_dp.py --input-file data_product.fdp

# Decode all files in a directory
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728

# Aggregate into GDS-style logs
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728 --collect
```

---

## Features

- **Direct binary decoding** - Processes `.bin`/`.fdp` files directly
- **Full packet reconstruction** - Extracts and decodes ComBuffer packets
- **Event decoding** - Converts event packets to readable messages with arguments
- **Telemetry decoding** - Extracts channel values with timestamps
- **Integrity validation** - Validates CRC checksums and sentry values
- **Multiple output formats** - JSON and human-readable text
- **Batch processing** - Process entire directories with container ID filtering
- **Collection mode** - Aggregate all data products into GDS-style log files
- **Reconstruction mode** - Recover data from corrupted or partial files
- **Error handling** - Graceful handling of corrupted or unknown packets

### What Gets Decoded

✅ **Events** - Full event messages with severity, timestamp, and arguments  
✅ **Telemetry Channels** - Channel values with timestamps  
✅ **Packet Metadata** - Packet types, sentry values, record indices  
✅ **Container Metadata** - Priority, container ID, timestamps

---

## Installation

```bash
# Install F Prime GDS
pip install fprime-gds
```

The script is located in the ComLoggerDp component scripts directory:
```bash
cd /path/to/fprime/Svc/ComLoggerDp/scripts
```

---

## Usage Modes

### Single File Processing

Decode a single data product file into JSON and/or text format.

#### Basic Usage

```bash
# Decode with auto-detected dictionaries
python decode_comlogger_dp.py --input-file data_product.fdp

# Specify dictionary path
python decode_comlogger_dp.py --input-file data_product.fdp --dict-path /path/to/dict

# Specify output file
python decode_comlogger_dp.py --input-file data_product.fdp --output-file decoded.json
```

#### Output Format Options

```bash
# JSON output only
python decode_comlogger_dp.py --input-file data_product.fdp --format json

# Text output only
python decode_comlogger_dp.py --input-file data_product.fdp --format text

# Both (default)
python decode_comlogger_dp.py --input-file data_product.fdp --format both
```

**Result**: Creates `data_product.json` and/or `data_product.txt` in the same directory.

---

### Batch Directory Processing

Process entire directories containing data product files.

#### Basic Usage

```bash
# Decode all files with specific container ID
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728

# Use hex notation for container ID
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 0x08000000

# Specify output directory
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728 \
  --output-dir /path/to/output

# With dictionary path
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728 \
  --dict-path /path/to/dict
```

#### Finding Your Container ID

The container ID uniquely identifies which data product container the files belong to.

**Method 1: From Filename**

Data product filenames follow the pattern: `Dp_<container_id>_<timestamp>_<sequence>.fdp`

Example: `Dp_134217728_1789761500_00043237.fdp`
- Container ID: **134217728** (decimal) or **0x08000000** (hex)

**Method 2: From FPP Topology**

Check your topology FPP file for the ComLoggerDp instance:
```fpp
instance comLogger: Svc.ComLoggerDp base id 0x8000
```

The container ID is the base ID shifted left by 20 bits:
- Base ID: `0x8000`
- Container ID: `0x08000000` (134217728 decimal)

**Method 3: From Existing JSON**

If you have decoded JSON files:
```bash
python -c "import json; print(json.load(open('file.json'))['header']['Id']['value'])"
```

#### Batch Processing Features

- **Automatic File Discovery**: Finds all files matching `Dp_<container_id>_*.fdp` or `Dp_<container_id>_*.bin`
- **Progress Indication**: Shows processing status for each file
- **Error Handling**: Continues processing even if individual files fail
- **Summary Report**: Provides statistics on successful/failed processing

#### Output Files

For each input file, the decoder creates output based on the format:

**Input**: `Dp_134217728_1789761500_00043237.fdp`

**Output (format: both)**:
- `Dp_134217728_1789761500_00043237.json` - Complete structured data
- `Dp_134217728_1789761500_00043237.txt` - Human-readable text

---

### Collection Mode (GDS-Style Log Aggregation)

Aggregate all data products from a directory into GDS-compatible log files. This creates a unified timeline of events and telemetry that matches the format produced by the F Prime Ground Data System (GDS).

#### When to Use Collection Mode

Use `--collect` when you want to:

- **Post-mission analysis** - Analyze recorded data after a test or mission
- **Timeline reconstruction** - Create a chronological view of all events and telemetry
- **GDS log comparison** - Compare recorded data with real-time GDS logs
- **Trending analysis** - Analyze telemetry trends over time
- **Event correlation** - Find relationships between events across multiple data products
- **Report generation** - Generate summaries from data product archives

#### Basic Usage

```bash
# Collect all DPs into GDS-style logs
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728 --collect

# Specify output location
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728 \
  --collect --collect-dir /path/to/logs

# With dictionary path
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728 \
  --collect --dict-path /path/to/dict
```

#### Output Structure

Collection mode creates a timestamped directory:

```
comlogger-dp-YYYY_MM_DD-HH_MM_SS/
├── events-dp.log      # All events, sorted chronologically
├── channels-dp.log    # All telemetry, sorted chronologically
└── README.txt         # Statistics and viewing instructions
```

#### Log Format

**Events Log Format** (`events-dp.log`):
```text
<ISO_timestamp>,(<fprime_time>),<event_name>,<event_id>,<severity>,<message>
```

Example:
```text
2026-09-18T19:58:15.779517,(2(0)-1789761495:779517),RecordedCom.comLogger.ComDpStarted,134217729,EventSeverity.ACTIVITY_HI,Started Com DP logging: 10 packets per container
```

**Channels Log Format** (`channels-dp.log`):
```text
<ISO_timestamp>,(<fprime_time>),<channel_name>,<channel_id>,<value>
```

Example:
```text
2026-09-18T19:58:16.043416,(2(0)-1789761496:43416),RecordedCom.comLogger.LoggingEnabled,134217728,True
```

#### Collection Mode Features

- **GDS Format Compatibility**: Output matches F Prime GDS logs exactly
- **Chronological Sorting**: All entries sorted by timestamp across all data products
- **Timestamped Directories**: Each collection creates a new directory to avoid overwriting
- **Progress Reporting**: Shows real-time processing status
- **Statistics**: Reports files processed, events collected, channels collected

#### Viewing Collected Logs

```bash
# View all events
cat events-dp.log

# Search for specific event
grep "ComDpStarted" events-dp.log

# Search for component events
grep "RecordedCom\." events-dp.log

# Search for channel
grep "systemResources.CPU" channels-dp.log

# Count events by severity
grep "EventSeverity.COMMAND" events-dp.log | wc -l

# Extract channel values
awk -F',' '{print $3, $5}' channels-dp.log
```

---

### Reconstruction Mode (Data Recovery)

For corrupted or partial data product files, use `--reconstruct` to scan for sentinels and extract records.

#### When to Use Reconstruction Mode

Use `--reconstruct` for:

- **Partial file downloads** - Incomplete data product files
- **Corrupted files** - Files with damaged headers or trailers
- **Missing headers** - Data without proper DP structure
- **Storage damage** - Data recovered from damaged storage

#### Basic Usage

```bash
# Reconstruct records from corrupted file
python decode_comlogger_dp.py --input-file corrupted.bin --reconstruct

# Reconstruct with custom sentry value
python decode_comlogger_dp.py --input-file corrupted.bin --reconstruct --sentry 0x12345678

# Reconstruct and save as JSON
python decode_comlogger_dp.py --input-file corrupted.bin --reconstruct \
  --format json --output-file recovered.json
```

#### Reconstruction Mode Features

- **Sentry scanning** - Scans entire file for sentry values (default: 0xDEADBEEF)
- **Garbage tolerance** - Ignores garbage/corrupted data before first sentinel
- **Best-effort decode** - Extracts data between sentinels as ComBuffer records
- **Error resilience** - Continues processing even if individual records fail
- **Detailed statistics** - Reports recovery success rate

#### How It Works

1. Scans entire file for 4-byte sentry values
2. Extracts data between consecutive sentries as ComBuffer records
3. Attempts to decode each extracted record
4. Reports statistics on recovery success

---

## Command Line Reference

### Input Arguments (Required - Choose One)

```bash
--input-file PATH       # Decode a single data product file
--input-dir PATH        # Decode all matching files in a directory
```

### Container ID (Required with --input-dir)

```bash
--dp-id ID              # Container ID filter (decimal or hex)
                        # Examples: --dp-id 134217728 or --dp-id 0x08000000
```

### Dictionary Arguments (Optional)

```bash
--dict-path PATH        # Path to F Prime dictionary directory
-d PATH                 # Short form (auto-detected if omitted)
```

### Output Location Arguments (Optional)

```bash
--output-file PATH      # Output file for single file mode (extension added)
--output-dir PATH       # Output directory for batch mode (default: input dir)
--collect-dir PATH      # Base directory for collection mode (default: current dir)
```

### Output Format Arguments (Optional)

```bash
--format FORMAT         # Output format: json, text, or both (default: both)
-f FORMAT               # Short form
```

### Validation Arguments (Optional)

```bash
--sentry VALUE          # Override sentry value (e.g., 0xDEADBEEF)
--no-crc                # Skip CRC validation
--no-sentry             # Skip sentry validation
```

### Mode Arguments (Optional)

```bash
--reconstruct           # Reconstruction mode (single file only)
--collect               # Collection mode (directory only)
```

---

## How It Works

The decoder processes ComLoggerDp data products in four phases:

### Phase 1: Parse Data Product Structure
- Reads the standard F Prime DP header (Container ID, Time, Priority, DataSize)
- Validates header CRC32 checksum

### Phase 2: Extract ComBuffer Records
- Iterates through all records in the data section
- For each record:
  - Reads record ID (should be 0 for ComBufferRecord)
  - Reads array size
  - Extracts and validates 4-byte sentry value
  - Extracts raw ComBuffer data
- Validates data section CRC32 checksum

### Phase 3: Decode ComBuffers
- For each ComBuffer:
  - Extracts packet type (FwPacketDescriptorType - 2 bytes)
  - Identifies packet as Event, Telemetry, or other type

### Phase 4: Decode Packets
- **Events (FW_PACKET_LOG)**:
  - Extracts event ID, timestamp, and arguments
  - Looks up event definition in dictionary
  - Formats message with arguments
  - Outputs severity, component name, and formatted message

- **Telemetry (FW_PACKET_TELEM)**:
  - Extracts channel ID, timestamp, and value
  - Looks up channel definition in dictionary
  - Deserializes value based on channel type
  - Outputs component name, channel name, and value

---

## Output Formats

### JSON Output

Complete structured output with all fields:

```json
{
  "header": {
    "Id": {"value": 0},
    "Priority": {"value": 5},
    "TimeTag": "...",
    "DataSize": {"value": 1234}
  },
  "records": [
    {
      "record_index": 0,
      "packet_type": "FW_PACKET_LOG",
      "decoded": {
        "event_name": "ComLoggerDp.ComDpStarted",
        "severity": "ACTIVITY_HI",
        "time": {...},
        "message": "Started Com DP logging: 100 packets per container",
        "args": [{"name": "packetsPerContainer", "value": 100}]
      }
    },
    {
      "record_index": 1,
      "packet_type": "FW_PACKET_TELEM",
      "decoded": {
        "channel_name": "ComLoggerDp.LoggingEnabled",
        "time": {...},
        "value": true
      }
    }
  ],
  "statistics": {
    "total_records": 2,
    "events": 1,
    "telemetry": 1,
    "errors": 0
  }
}
```

### Text Output

Human-readable format suitable for logs:

```
================================================================================
ComLoggerDp Data Product Decoder Output
================================================================================

HEADER:
--------------------------------------------------------------------------------
Container ID: 0
Priority: 5
Time: 2026-09-21T12:00:00
Data Size: 1234 bytes

DECODED PACKETS:
--------------------------------------------------------------------------------

[Record 0] FW_PACKET_LOG
  Event: ComLoggerDp.ComDpStarted
  Severity: ACTIVITY_HI
  Time: 2026-09-21T12:00:00
  Message: Started Com DP logging: 100 packets per container
  Args:
    packetsPerContainer: 100

[Record 1] FW_PACKET_TELEM
  Channel: ComLoggerDp.LoggingEnabled
  Time: 2026-09-21T12:00:01
  Value: True

================================================================================
STATISTICS:
--------------------------------------------------------------------------------
Total records: 2
Events: 1
Telemetry: 1
Errors: 0

Packet type breakdown:
  FW_PACKET_LOG: 1
  FW_PACKET_TELEM: 1
```

---

## Examples

### Example 1: Single File Decode

```bash
# Decode with both JSON and text output
python decode_comlogger_dp.py --input-file data_product.fdp

# View results
cat data_product.txt
jq . data_product.json
```

### Example 2: Batch Processing

```bash
# Process all files with container ID 134217728
python decode_comlogger_dp.py --input-dir /data/dps --dp-id 134217728

# Result: All matching files decoded in /data/dps/
```

### Example 3: Collection for Analysis

```bash
# Collect all events and telemetry
python decode_comlogger_dp.py --input-dir /data/dps --dp-id 134217728 --collect

# Analyze results
cd comlogger-dp-*/

# Count events
wc -l events-dp.log

# Search for specific events
grep "ComDpStarted" events-dp.log

# Extract CPU telemetry trend
grep "CPU_00" channels-dp.log | awk -F',' '{print $1, $5}' > cpu_trend.csv
```

### Example 4: Recover Corrupted File

```bash
# Scan for sentinels and extract data
python decode_comlogger_dp.py --input-file corrupted.bin --reconstruct

# View recovered data
cat corrupted.txt
```

### Example 5: Custom Output Location

```bash
# Save decoded files to different location
python decode_comlogger_dp.py --input-dir /data/raw_dps \
  --dp-id 134217728 \
  --output-dir /data/decoded_dps \
  --dict-path /path/to/dict
```

### Example 6: Skip Validation

```bash
# Process files even if checksums fail
python decode_comlogger_dp.py --input-dir /data/dps \
  --dp-id 134217728 \
  --no-crc \
  --no-sentry
```

---

## Troubleshooting

### "ComLoggerDpSentry not found in config"

The script will use a default sentry value (0xDEADBEEF). If your deployment uses a different sentry value, specify it:

```bash
python decode_comlogger_dp.py data_product.bin --sentry 0x12345678
```

### "Channel/Event not found in dictionary"

Make sure the dictionary path is correct and matches the build that generated the data product:

```bash
python decode_comlogger_dp.py data_product.bin --dict-path /path/to/correct/dict
```

### CRC validation errors

If the file is corrupted or was generated with checksums disabled:

```bash
python decode_comlogger_dp.py data_product.bin --no-crc
```

### Sentry validation errors

If records have the wrong sentry value, you can either:
1. Specify the correct sentry: `--sentry 0xVALUE`
2. Skip sentry validation: `--no-sentry`

### No files found in batch mode

```
Error: No data product files found matching container ID 134217728
```

**Solutions**:
- Verify the container ID is correct
- Check if files use `.bin` extension instead of `.fdp`
- List directory contents to confirm filename pattern

### "fprime-gds not found"

Install the fprime-gds package:

```bash
pip install fprime-gds
```

### Permission errors

```
Error: Permission denied: /path/to/output
```

**Solutions**:
- Ensure write permissions for output directory
- Try a different output directory
- Use `sudo` if appropriate

---

## Binary Structure Reference

### Data Product File Structure

```
┌─────────────────────────────────────────┐
│ DP Header (variable size)               │
│  - Container ID                         │
│  - Priority                             │
│  - Time Tag                             │
│  - Data Size                            │
│  - User Data                            │
│  - Header CRC32 (4 bytes)               │
├─────────────────────────────────────────┤
│ Data Records Section                    │
│  ┌─────────────────────────────────┐   │
│  │ Record 0                        │   │
│  │  - Record ID (FwDpIdType)       │   │
│  │  - Array Size (FwSizeStoreType) │   │
│  │  - Sentry (U32, 4 bytes)        │   │
│  │  - ComBuffer data               │   │
│  └─────────────────────────────────┘   │
│  ┌─────────────────────────────────┐   │
│  │ Record 1                        │   │
│  │  - Record ID                    │   │
│  │  - Array Size                   │   │
│  │  - Sentry (U32, 4 bytes)        │   │
│  │  - ComBuffer data               │   │
│  └─────────────────────────────────┘   │
│  ...                                    │
├─────────────────────────────────────────┤
│ Data CRC32 (4 bytes)                    │
└─────────────────────────────────────────┘
```

### ComBuffer Structure

```
┌─────────────────────────────────────────┐
│ Packet Type (FwPacketDescriptorType)    │
│  - U16, 2 bytes                         │
│  - Values from ComCfg::Apid enum        │
├─────────────────────────────────────────┤
│ Packet Data (variable size)             │
│                                         │
│  Event Packet (0x0002):                 │
│    - Event ID (FwEventIdType)           │
│    - Time Tag (Fw::Time)                │
│    - Arguments (type-specific)          │
│                                         │
│  Telemetry Packet (0x0001):             │
│    - Channel ID (FwChanIdType)          │
│    - Time Tag (Fw::Time)                │
│    - Value (type-specific)              │
└─────────────────────────────────────────┘
```

---

## Integration with F Prime Workflows

This script complements the standard F Prime data product tools:

```bash
# Standard F Prime DP decoder (requires JSON dictionary generation)
python -m fprime_gds.executables.data_products decode data_product.bin

# ComLoggerDp decoder (direct binary processing, no JSON dictionary needed)
python decode_comlogger_dp.py data_product.bin
```

The ComLoggerDp decoder is specifically optimized for ComLogger data products and provides deeper insight into the original Com packets.

---

## See Also

- [ComLoggerDp Component Documentation](../docs/sdd.md)
- [F Prime Data Products User Guide](https://nasa.github.io/fprime/)
