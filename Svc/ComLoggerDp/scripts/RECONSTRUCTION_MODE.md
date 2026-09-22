# Reconstruction Mode - Data Recovery Guide

## Overview

Reconstruction mode is a special recovery feature that can extract ComBuffer records from corrupted or partial data product files. It scans the entire file looking for sentry values and attempts to decode the data between sentinels, making it possible to recover data from damaged files.

## When to Use Reconstruction Mode

Use `--reconstruct` when you have:

- **Corrupted data product files** - Files with damaged headers or trailers
- **Partial downloads** - Incomplete file transfers  
- **Truncated files** - Files cut off during writing
- **Damaged storage** - Data recovered from failing drives
- **Extracted binary data** - Raw data dumps without proper DP structure
- **Concatenated fragments** - Multiple partial files combined

## Basic Usage

```bash
# Reconstruct from corrupted file
python decode_comlogger_dp.py --input-file corrupted.bin --reconstruct

# With custom sentry value
python decode_comlogger_dp.py --input-file corrupted.bin \
  --reconstruct --sentry 0x12345678

# JSON output
python decode_comlogger_dp.py --input-file corrupted.bin \
  --reconstruct --format json

# With output file
python decode_comlogger_dp.py --input-file corrupted.bin \
  --reconstruct --output-file recovered.json --format json
```

## Command Line Arguments

### Required Arguments

```bash
--input-file PATH       # Corrupted or partial data product file
--reconstruct           # Enable reconstruction mode
```

### Optional Arguments

```bash
--dict-path PATH        # Dictionary path (auto-detected if omitted)
-d PATH                 # Short form

--output-file PATH      # Output file path (extension added based on format)
                        # Default: <input_file_basename>.json/.txt

--format FORMAT         # Output format: json, text, or both (default: both)
-f FORMAT               # Short form

--sentry VALUE          # Override sentry value (e.g., 0xDEADBEEF)
                        # Must match the sentry in your corrupted file

--no-sentry             # Skip sentry validation during decode
                        # Still searches for sentinels for extraction
```

### Not Applicable in Reconstruction Mode

These arguments don't work with `--reconstruct`:

```bash
--input-dir PATH        # Reconstruction is single-file only
--dp-id ID              # Not needed (no container ID validation)
--output-dir PATH       # Use --output-file instead
--no-crc                # CRC validation not performed in reconstruction
--collect               # Collection mode is directory-based
```

## How It Works

### 1. File Scanning

The decoder reads the entire file and searches for sentry values (4-byte magic numbers):

```
File: [GARBAGE...] [SENTRY] [DATA] [SENTRY] [DATA] [GARBAGE...]
                      ↓                ↓
                    Found            Found
```

### 2. Data Extraction

For each sentry found, it extracts data until:
- The next sentry is found (normal case)
- End of file (last record)
- Maximum record size reached (4096 bytes)

### 3. Decoding Attempt

Each extracted chunk is decoded as a ComBuffer:
- Parse packet type (2 bytes)
- Extract packet data
- Decode based on type (event, telemetry, etc.)
- Report success or error

### 4. Statistics

Provides detailed information:
- How many sentinels found
- How many records extracted
- How many successfully decoded
- Breakdown by packet type

## Output Format

### Text Output Example

```
================================================================================
ComLoggerDp Reconstruction Mode Output
================================================================================

FILE INFORMATION:
--------------------------------------------------------------------------------
Path: /path/to/corrupted.bin
Size: 1455 bytes

RECONSTRUCTION:
--------------------------------------------------------------------------------
Sentinels found: 6
Records extracted: 6
Records decoded: 5
Bytes scanned: 1455

RECOVERED RECORDS:
--------------------------------------------------------------------------------

[Sentinel 0] Position: 210, Size: 31 bytes
  Type: FW_PACKET_LOG
  Event: CdhCore.cmdDisp.OpCodeDispatched
  Severity: EventSeverity.COMMAND
  Time: {'seconds': 1789761495, 'microseconds': 779435}
  Message: Opcode 0x8000000 dispatched to port 12

[Sentinel 1] Position: 245, Size: 27 bytes
  Type: FW_PACKET_LOG
  Event: RecordedCom.comLogger.ComDpStarted
  Severity: EventSeverity.ACTIVITY_HI
  Message: Started Com DP logging: 10 packets per container

[Sentinel 2] Position: 512, Size: 0 bytes
  ERROR: Failed to decode: ComBuffer too short

...
```

### JSON Output Structure

```json
{
  "file_info": {
    "path": "/path/to/corrupted.bin",
    "size": 1455
  },
  "reconstruction": {
    "sentinels_found": 6,
    "records_extracted": 6,
    "records_decoded": 5,
    "bytes_scanned": 1455
  },
  "records": [
    {
      "sentinel_index": 0,
      "file_position": 210,
      "sentry": "0xdeadbeef",
      "data_size": 31,
      "packet_type": "FW_PACKET_LOG",
      "decoded": {
        "event_name": "CdhCore.cmdDisp.OpCodeDispatched",
        "severity": "EventSeverity.COMMAND",
        "message": "Opcode 0x8000000 dispatched to port 12"
      }
    }
  ],
  "statistics": {
    "packet_types": {
      "FW_PACKET_LOG": 3,
      "FW_PACKET_TELEM": 2
    },
    "events": 3,
    "telemetry": 2,
    "errors": 1
  }
}
```

## Key Features

### Ignores Garbage Data

Any data before the first sentinel is ignored:

```
[CORRUPT_HEADER] [GARBAGE...] [SENTRY] [VALID_DATA]
                               ↑
                    Starts decoding here
```

### Continues on Errors

If one record fails to decode, processing continues:

```
Record 0: ✅ Success
Record 1: ✅ Success  
Record 2: ❌ Error - continues
Record 3: ✅ Success
```

### Position Tracking

Each record reports its position in the file, useful for:
- Understanding file structure
- Debugging corruption
- Manual hex editing if needed

### Best-Effort Decoding

Attempts to decode every extracted chunk, even if:
- Packet type is unknown
- Data is incomplete
- Dictionary doesn't have definitions

## Differences from Normal Mode

| Feature | Normal Mode | Reconstruction Mode |
|---------|-------------|---------------------|
| Requires valid DP header | ✅ Yes | ❌ No |
| Validates CRC | ✅ Yes (default) | ❌ N/A |
| Requires container structure | ✅ Yes | ❌ No |
| Handles garbage data | ❌ No | ✅ Yes |
| File position reporting | ❌ No | ✅ Yes |
| Batch processing | ✅ Yes | ❌ No (single file only) |

## Examples

### Example 1: Partial Download

You have a partial data product file from an incomplete download:

```bash
python decode_comlogger_dp.py --input-file partial_download.bin \
  --reconstruct --format text
```

**Result**: Extracts all complete records up to the truncation point.

### Example 2: Corrupted Header

File has a corrupted header but data is intact:

```bash
python decode_comlogger_dp.py --input-file bad_header.bin \
  --reconstruct
```

**Result**: Skips corrupted header, finds first sentry, extracts all records.

### Example 3: Custom Sentry

Your deployment uses a different sentry value:

```bash
python decode_comlogger_dp.py --input-file data.bin \
  --reconstruct --sentry 0x12345678 \
  --format json --output-file recovered.json
```

**Result**: Scans for custom sentry, saves results as JSON.

### Example 4: Raw Binary Dump

You extracted a memory dump that contains ComBuffer data:

```bash
python decode_comlogger_dp.py --input-file memory_dump.bin \
  --reconstruct --dict-path /path/to/dict
```

**Result**: Finds any ComBuffer records in the dump.

### Example 5: Concatenated Fragments

Multiple partial files concatenated together:

```bash
# Combine fragments
cat fragment1.bin fragment2.bin fragment3.bin > combined.bin

# Reconstruct
python decode_comlogger_dp.py --input-file combined.bin --reconstruct
```

**Result**: Finds all sentinels across all fragments.

## Limitations

### Single File Only

Reconstruction mode works with `--input-file` only, not `--input-dir`:

```bash
# ✅ Valid
python decode_comlogger_dp.py --input-file corrupt.bin --reconstruct

# ❌ Invalid
python decode_comlogger_dp.py --input-dir /corrupt_files --reconstruct
```

### No Container Metadata

Since the DP header is not required/validated, you won't get:
- Container ID
- Priority
- Container timestamp
- User data

Each record still has its individual timestamp.

### Maximum Record Size

To avoid reading garbage as data, extraction stops at:
- Next sentry (if found)
- 4096 bytes (maximum reasonable ComBuffer size)
- End of file

Very large records may be truncated.

### Sentry Value Required

You must know or specify the correct sentry value:
- Default: `0xDEADBEEF`
- Custom: Use `--sentry` argument
- Wrong sentry = no records found

## Troubleshooting

### No Sentinels Found

```
Found 0 sentinel(s)
No sentinels found - cannot reconstruct records
```

**Solutions:**
- Verify sentry value with `--sentry`
- Check if file is completely corrupted
- Verify file actually contains ComLogger data
- Try hex editor to search for sentry manually

### All Records Fail to Decode

```
Sentinels found: 10
Records extracted: 10
Records decoded: 0
```

**Solutions:**
- Verify dictionary path matches the deployment
- Check if data is from different F Prime version
- Examine raw hex data to verify format
- Check if ComBuffer format is non-standard

### Unexpected Sentinel Positions

```
[Sentinel 0] Position: 50, Data size: 2 bytes
[Sentinel 1] Position: 53, Data size: 1 bytes
```

**Solutions:**
- Sentinels may appear in legitimate data
- Very small records likely false positives
- Use hex editor to verify sentinel context
- Consider if file contains multiple types of data

## Technical Details

### Sentry Format

The sentry is a 4-byte unsigned integer serialized big-endian:

```c
U32 sentry = 0xDEADBEEF;
// On disk: 0xDE 0xAD 0xBE 0xEF
```

### Record Structure

Data between sentinels should be:

```
[SENTRY (4 bytes)] [ComBuffer Data (variable)]
                   └─ Packet Type (2 bytes)
                   └─ Packet Data (variable)
```

### Search Algorithm

1. Read entire file into memory
2. Search for sentry bytes sequentially
3. Record all positions
4. For each position:
   - Extract data until next sentry or limit
   - Attempt to decode as ComBuffer
   - Report success or error

## See Also

- [README.md](README.md) - Complete decoder documentation
- [QUICK_START.md](QUICK_START.md) - Quick reference guide
- [BATCH_PROCESSING.md](BATCH_PROCESSING.md) - Batch processing guide
