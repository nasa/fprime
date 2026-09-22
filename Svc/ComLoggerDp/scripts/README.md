# ComLoggerDp Scripts

This directory contains utility scripts for working with ComLoggerDp data products.

## decode_comlogger_dp.py

A Python script that decodes ComLoggerDp data product binary files directly into human-readable events and telemetry channels, **without** requiring intermediate JSON dictionary generation.

### Features

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

### Requirements

```bash
# F Prime GDS must be installed
pip install fprime-gds
```

### Usage

#### Single File Processing

```bash
# Decode a single file with auto-detected dictionaries
python decode_comlogger_dp.py --input-file data_product.fdp

# Specify dictionary path
python decode_comlogger_dp.py --input-file data_product.fdp --dict-path /path/to/dict

# Specify output file
python decode_comlogger_dp.py --input-file data_product.fdp --output-file decoded.json

# JSON output only
python decode_comlogger_dp.py --input-file data_product.fdp --format json

# Text output only
python decode_comlogger_dp.py --input-file data_product.fdp --format text
```

#### Batch Directory Processing

```bash
# Decode all files with specific container ID in a directory
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728

# Use hex notation for container ID
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 0x08000000

# Specify output directory for batch processing
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728 \
  --output-dir /path/to/output

# Specify dictionary path
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728 \
  --dict-path /path/to/dict

# Process directory with JSON output only
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 134217728 --format json
```

#### Advanced Options

```bash
# Override sentry value (if different from default)
python decode_comlogger_dp.py --input-file data_product.fdp --sentry 0x12345678

# Skip CRC validation (for corrupted files)
python decode_comlogger_dp.py --input-file data_product.fdp --no-crc

# Skip sentry validation (for testing)
python decode_comlogger_dp.py --input-file data_product.fdp --no-sentry

# Specify custom output file (single file only)
python decode_comlogger_dp.py --input-file data_product.fdp \
  --output-file my_output.json --format json

# Batch process with custom output directory
python decode_comlogger_dp.py --input-dir /path/to/dps --dp-id 0x08000000 \
  --output-dir /custom/output --dict-path /path/to/dict
```

#### Collection Mode (GDS-Style Log Aggregation)

Aggregate all data products from a directory into GDS-compatible log files:

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

**Collection Mode Features:**

- Aggregates all events into single `events-dp.log` file
- Aggregates all telemetry into single `channels-dp.log` file
- Output format matches F Prime GDS log format exactly
- Creates timestamped directory: `comlogger-dp-YYYY_MM_DD-HH_MM_SS/`
- All entries sorted chronologically
- Includes README with statistics and viewing instructions

**Output Format:**

```text
Events: <ISO_timestamp>,(<fprime_time>),<event_name>,<id>,<severity>,<message>
Channels: <ISO_timestamp>,(<fprime_time>),<channel_name>,<id>,<value>
```

**Use Cases:**

- Post-mission analysis of recorded data
- Generating reports from data product archives
- Comparing recorded data with real-time GDS logs
- Batch analysis of events and telemetry trends

#### Reconstruction Mode (Data Recovery)

For corrupted or partial data product files, use `--reconstruct` to scan for sentinels and extract records:

```bash
# Reconstruct records from corrupted file
python decode_comlogger_dp.py --input-file corrupted.bin --reconstruct

# Reconstruct with custom sentry value
python decode_comlogger_dp.py --input-file corrupted.bin --reconstruct --sentry 0x12345678

# Reconstruct and save as JSON
python decode_comlogger_dp.py --input-file corrupted.bin --reconstruct \
  --format json --output-file recovered.json
```

**Reconstruction Mode Features:**
- Scans entire file for sentry values
- Ignores garbage/corrupted data before first sentinel
- Extracts data between sentinels as ComBuffer records
- Makes best-effort decode of each extracted record
- Continues processing even if individual records fail
- Reports detailed statistics on recovery success

**Use Cases:**
- Partial file downloads
- Corrupted data product files
- Files with missing headers or trailers
- Data recovered from damaged storage

#### Finding Container ID

The container ID can be found in several ways:

1. **From the filename**: Data product files are typically named `Dp_<container_id>_<timestamp>_<sequence>.fdp`
   - Example: `Dp_134217728_1789761500_00043237.fdp` has container ID 134217728 (0x08000000)

2. **From the FPP definition**: Check your project's ComLoggerDp instance in the topology FPP file
   ```fpp
   instance comLogger: Svc.ComLoggerDp base id 0x8000
   ```
   The container ID will be `0x08000000` (base ID shifted left by 20 bits)

3. **From existing JSON**: If you have decoded JSON files, check the `Id` field in the header:
   ```bash
   python -c "import json; print(json.load(open('file.json'))['header']['Id']['value'])"
   ```

### How It Works

The decoder processes ComLoggerDp data products in four phases:

#### Phase 1: Parse Data Product Structure
- Reads the standard F Prime DP header (Container ID, Time, Priority, DataSize)
- Validates header CRC32 checksum

#### Phase 2: Extract ComBuffer Records
- Iterates through all records in the data section
- For each record:
  - Reads record ID (should be 0 for ComBufferRecord)
  - Reads array size
  - Extracts and validates 4-byte sentry value
  - Extracts raw ComBuffer data
- Validates data section CRC32 checksum

#### Phase 3: Decode ComBuffers
- For each ComBuffer:
  - Extracts packet type (FwPacketDescriptorType - 2 bytes)
  - Identifies packet as Event, Telemetry, or other type
  
#### Phase 4: Decode Packets
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

### Output Formats

#### JSON Output
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

#### Text Output
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

### Binary Structure Reference

#### Data Product File Structure
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

#### ComBuffer Structure
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

### Troubleshooting

#### "ComLoggerDpSentry not found in config"
The script will use a default sentry value (0xDEADBEEF). If your deployment uses a different sentry value, specify it with `--sentry`:
```bash
python decode_comlogger_dp.py data_product.bin --sentry 0x12345678
```

#### "Channel/Event not found in dictionary"
Make sure the dictionary path is correct and matches the build that generated the data product:
```bash
python decode_comlogger_dp.py data_product.bin --dict-path /path/to/correct/dict
```

#### CRC validation errors
If the file is corrupted or was generated with checksums disabled, you can skip validation:
```bash
python decode_comlogger_dp.py data_product.bin --no-crc
```

#### Sentry validation errors
If records have the wrong sentry value, you can either:
1. Specify the correct sentry: `--sentry 0xVALUE`
2. Skip sentry validation: `--no-sentry`

### Integration with F Prime Workflows

This script complements the standard F Prime data product tools:

```bash
# Standard F Prime DP decoder (requires JSON dictionary generation)
python -m fprime_gds.executables.data_products decode data_product.bin

# ComLoggerDp decoder (direct binary processing, no JSON dictionary needed)
python decode_comlogger_dp.py data_product.bin
```

The ComLoggerDp decoder is specifically optimized for ComLogger data products and provides deeper insight into the original Com packets.

## Additional Documentation

- **[QUICK_START.md](QUICK_START.md)** - Quick reference guide with common commands
- **[BATCH_PROCESSING.md](BATCH_PROCESSING.md)** - Detailed guide for processing multiple files
- **[COLLECTION_MODE.md](COLLECTION_MODE.md)** - GDS-style log aggregation guide
- **[RECONSTRUCTION_MODE.md](RECONSTRUCTION_MODE.md)** - Data recovery from corrupted files

### See Also

- [ComLoggerDp Component Documentation](../docs/sdd.md)
- [F Prime Data Products User Guide](https://nasa.github.io/fprime/)
