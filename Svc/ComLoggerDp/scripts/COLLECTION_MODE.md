# Collection Mode - GDS-Style Log Aggregation

## Overview

Collection mode aggregates all ComLoggerDp data products from a directory into GDS-compatible log files. This creates a unified timeline of events and telemetry that matches the format produced by the F Prime Ground Data System (GDS), making it ideal for post-mission analysis, trending, and comparison with real-time logs.

## When to Use Collection Mode

Use `--collect` when you want to:

- **Post-mission analysis** - Analyze recorded data after a test or mission
- **Timeline reconstruction** - Create a chronological view of all events and telemetry
- **GDS log comparison** - Compare recorded data with real-time GDS logs
- **Trending analysis** - Analyze telemetry trends over time
- **Event correlation** - Find relationships between events across multiple data products
- **Report generation** - Generate summaries from data product archives
- **Data export** - Export data in a format compatible with other tools

## Basic Usage

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

## Output Structure

### Directory Creation

Collection mode creates a timestamped directory to avoid overwriting previous collections:

```
comlogger-dp-YYYY_MM_DD-HH_MM_SS/
├── events-dp.log      # All events, sorted chronologically
├── channels-dp.log    # All telemetry, sorted chronologically
└── README.txt         # Statistics and viewing instructions
```

**Directory naming:** `comlogger-dp-` prefix distinguishes these from actual `fprime-gds-` runtime logs.

### Events Log Format

File: `events-dp.log`

```text
<ISO_timestamp>,(<fprime_time>),<event_name>,<event_id>,<severity>,<message>
```

**Example:**
```text
2026-09-18T19:58:15.779435,(2(0)-1789761495:779435),CdhCore.cmdDisp.OpCodeDispatched,16777217,EventSeverity.COMMAND,Opcode 0x8000000 dispatched to port 12
2026-09-18T19:58:15.779517,(2(0)-1789761495:779517),RecordedCom.comLogger.ComDpStarted,134217729,EventSeverity.ACTIVITY_HI,Started Com DP logging: 10 packets per container
```

**Fields:**
- **ISO timestamp** - UTC time in ISO 8601 format
- **F Prime time** - (base(context)-seconds:microseconds)
- **Event name** - Component.port.event or Component.event
- **Event ID** - Numeric event identifier
- **Severity** - Event severity level (COMMAND, ACTIVITY_HI, ACTIVITY_LO, etc.)
- **Message** - Formatted event message with arguments

### Channels Log Format

File: `channels-dp.log`

```text
<ISO_timestamp>,(<fprime_time>),<channel_name>,<channel_id>,<value>
```

**Example:**
```text
2026-09-18T19:58:15.043053,(2(0)-1789761495:43053),Ref.systemResources.CPU_00,268578821,3.0
2026-09-18T19:58:16.043303,(2(0)-1789761496:43303),Ref.systemResources.CPU_00,268578821,4.95049524307251
2026-09-18T19:58:16.043416,(2(0)-1789761496:43416),RecordedCom.comLogger.LoggingEnabled,134217728,True
```

**Fields:**
- **ISO timestamp** - UTC time in ISO 8601 format
- **F Prime time** - (base(context)-seconds:microseconds)
- **Channel name** - Component.channel
- **Channel ID** - Numeric channel identifier
- **Value** - Channel value (numbers, booleans, enums, arrays)

## How It Works

### 1. File Discovery

Scans the input directory for all data product files matching the container ID:

```
Input directory: /path/to/dps
Container ID: 134217728 (0x08000000)

Files found:
  Dp_134217728_1789761500_00043237.fdp ✓
  Dp_134217728_1789761506_00043312.fdp ✓
  Dp_999999999_1789761512_00043230.fdp ✗ (different container ID)
```

### 2. Data Extraction

Each data product file is decoded:
- Extract all ComBuffer records
- Decode events (FW_PACKET_LOG)
- Decode telemetry channels (FW_PACKET_TELEM, FW_PACKET_PACKETIZED_TLM)
- Collect timestamps, IDs, and decoded content

### 3. Aggregation

All decoded data is aggregated:
- Events from all files → single list
- Channels from all files → single list
- Sort both lists chronologically by timestamp

### 4. Output Generation

Create timestamped directory and write:
- `events-dp.log` - All events in GDS format
- `channels-dp.log` - All channels in GDS format
- `README.txt` - Statistics and usage instructions

### 5. Statistics

Reports:
- Files processed / failed
- Total events collected
- Total channels collected
- Output file paths

## Key Features

### GDS Format Compatibility

Output format matches F Prime GDS logs exactly:
- Same CSV structure
- Same timestamp formats
- Same field ordering
- Compatible with GDS log viewers and tools

### Chronological Sorting

All entries sorted by timestamp:
- Events ordered by event time
- Channels ordered by telemetry time
- Creates unified timeline across all data products

### Timestamped Directories

Each collection run creates a new directory:
- Format: `comlogger-dp-YYYY_MM_DD-HH_MM_SS/`
- Prevents overwriting previous collections
- Timestamp shows when collection was performed

### Progress Reporting

Shows real-time progress:
```
=== COLLECTION MODE ===
Aggregating data products into GDS-style logs...
Output directory: /path/to/comlogger-dp-2026_09_21-20_18_25

Processing: Dp_134217728_1789761500_00043237.fdp
Processing: Dp_134217728_1789761506_00043312.fdp
...

Processed 91 files, 0 failed
Collected 102 events, 806 channels
```

### README Generation

Each collection includes a README with:
- Source information (directory, container ID)
- Statistics (file count, event/channel counts)
- Log format reference
- Viewing instructions
- Example grep/awk commands

## Use Cases

### Post-Mission Analysis

After a test or mission, analyze all recorded data:

```bash
python decode_comlogger_dp.py --input-dir ./mission_data \
  --dp-id 134217728 --collect
```

Then analyze:
```bash
cd comlogger-dp-2026_09_21-20_18_25/

# View all events
cat events-dp.log

# Search for specific events
grep "FileWritten" events-dp.log

# Count events by severity
grep "EventSeverity.COMMAND" events-dp.log | wc -l
```

### GDS Log Comparison

Compare recorded data with real-time GDS logs:

```bash
# Collect ComLoggerDp data
python decode_comlogger_dp.py --input-dir ./recorded_data \
  --dp-id 134217728 --collect

# Compare with GDS logs
diff comlogger-dp-*/events-dp.log fprime-gds-*/event.log
```

### Telemetry Trending

Analyze telemetry trends over time:

```bash
# Collect all telemetry
python decode_comlogger_dp.py --input-dir ./telemetry_dps \
  --dp-id 134217728 --collect

# Extract specific channel
grep "CPU_00" comlogger-dp-*/channels-dp.log | \
  awk -F',' '{print $1, $5}' > cpu_trend.csv
```

### Report Generation

Generate event summaries for reports:

```bash
# Collect data
python decode_comlogger_dp.py --input-dir ./test_run \
  --dp-id 134217728 --collect

cd comlogger-dp-*/

# Count events by component
awk -F',' '{print $3}' events-dp.log | \
  cut -d'.' -f1 | sort | uniq -c

# List all unique channels
awk -F',' '{print $3}' channels-dp.log | sort -u
```

## Viewing Collected Logs

### Basic Viewing

```bash
# View all events
cat events-dp.log

# View all channels
cat channels-dp.log

# Page through events
less events-dp.log

# Tail last events
tail -n 20 events-dp.log
```

### Searching and Filtering

```bash
# Search for specific event
grep "ComDpStarted" events-dp.log

# Search for component events
grep "RecordedCom\." events-dp.log

# Search for severity
grep "EventSeverity.COMMAND" events-dp.log

# Search for channel
grep "systemResources.CPU" channels-dp.log

# Case-insensitive search
grep -i "error" events-dp.log
```

### Time Range Filtering

```bash
# Events in specific minute
grep "2026-09-18T19:58:" events-dp.log

# Events in time range using awk
awk -F',' '$1 >= "2026-09-18T19:58:00" && $1 <= "2026-09-18T19:59:00"' \
  events-dp.log

# Channels in specific hour
grep "2026-09-18T19:" channels-dp.log
```

### Counting and Statistics

```bash
# Total events
wc -l events-dp.log

# Total channels
wc -l channels-dp.log

# Events by severity
grep "EventSeverity.COMMAND" events-dp.log | wc -l
grep "EventSeverity.ACTIVITY_HI" events-dp.log | wc -l

# Unique event types
awk -F',' '{print $3}' events-dp.log | sort -u

# Unique channels
awk -F',' '{print $3}' channels-dp.log | sort -u
```

### Extracting Specific Data

```bash
# Extract just event messages
awk -F',' '{print $6}' events-dp.log

# Extract channel values
awk -F',' '{print $3, $5}' channels-dp.log

# Extract timestamps and events
awk -F',' '{print $1, $3, $6}' events-dp.log

# Export to CSV (already CSV format)
cp events-dp.log events.csv
cp channels-dp.log channels.csv
```

## Differences from Normal Mode

| Feature | Normal Mode | Collection Mode |
|---------|-------------|-----------------|
| Output files per DP | ✅ Yes (JSON/TXT) | ❌ No |
| Aggregated logs | ❌ No | ✅ Yes (events/channels) |
| GDS log format | ❌ No | ✅ Yes |
| Detailed packet info | ✅ Yes | ❌ No (events/channels only) |
| Container metadata | ✅ Yes | ❌ No (only timestamps) |
| Chronological sorting | ❌ Per-file | ✅ Global across all files |
| Output directory | Same as input | Timestamped subdirectory |
| README generation | ✅ Yes | ✅ Yes |

## Arguments and Options

### Required Arguments

```bash
--input-dir PATH     # Directory containing data products
--dp-id ID           # Container ID to filter (decimal or hex)
--collect            # Enable collection mode
```

### Optional Arguments

```bash
--collect-dir PATH   # Base directory for output (default: current directory)
--dict-path PATH     # Dictionary path (default: auto-detect)
--sentry VALUE       # Sentry value (default: 0xDEADBEEF)
--no-crc             # Skip CRC validation
--no-sentry          # Skip sentry validation
```

### Ignored Arguments

These are ignored in collection mode (with warnings):
- `--format` - Collection mode always produces log files
- `--output-dir` - Use `--collect-dir` instead
- `--output-file` - Not applicable for batch processing

## Limitations

### Directory Mode Only

Collection mode requires `--input-dir`:

```bash
# ✅ Valid
python decode_comlogger_dp.py --input-dir /dps --dp-id 134217728 --collect

# ❌ Invalid - collection mode requires directory
python decode_comlogger_dp.py --input-file single.fdp --collect
```

### Container ID Required

Must specify which container ID to collect:

```bash
# ✅ Valid
python decode_comlogger_dp.py --input-dir /dps --dp-id 134217728 --collect

# ❌ Invalid - missing --dp-id
python decode_comlogger_dp.py --input-dir /dps --collect
```

### Single Container ID

Only processes files matching one container ID per run. For multiple container IDs, run collection mode multiple times:

```bash
# Collect container A
python decode_comlogger_dp.py --input-dir /dps --dp-id 134217728 --collect

# Collect container B
python decode_comlogger_dp.py --input-dir /dps --dp-id 999999999 --collect
```

### No Packet-Level Detail

Collection mode only outputs events and channels, not:
- Packet metadata (packet types, sizes)
- Sentry values and positions
- Record indices
- Container priority
- CRC values

Use normal mode for packet-level details.

## Troubleshooting

### No Files Processed

```
Processed 0 files, 0 failed
Collected 0 events, 0 channels
```

**Solutions:**
- Verify container ID matches your data products
- Check filename format: `Dp_<container_id>_*.fdp`
- Try hex notation: `--dp-id 0x08000000` instead of `134217728`
- Verify input directory path is correct

### Some Files Failed

```
Processed 85 files, 6 failed
```

**Solutions:**
- Check individual warnings for failure reasons
- Common issues: corrupted files, missing dictionary entries
- Consider using `--no-crc` if CRC validation fails
- Failed files are skipped, collection continues

### Empty Log Files

Log files created but empty (0 bytes):

**Solutions:**
- Check if data products actually contain events/telemetry
- Verify dictionary path is correct
- Look for packet decoding errors in console output
- Ensure ComBuffer records are not all corrupt

### Timestamp Format Issues

If timestamps look wrong:

**Solutions:**
- Verify time base in F Prime time tag (usually 2)
- Check if system clock was correct during data collection
- F Prime timestamps are UTC by default
- Compare with known good GDS logs for reference

## Technical Details

### Time Handling

Timestamps are converted from F Prime format to ISO 8601:

```python
# F Prime time (in data product)
seconds = 1789761495
microseconds = 779435

# Converted to ISO 8601
iso_time = "2026-09-18T19:58:15.779435"

# F Prime time tag format
fprime_time = "(2(0)-1789761495:779435)"
# Format: (base(context)-seconds:microseconds)
```

### Sorting Algorithm

Events and channels are sorted by timestamp tuple:

```python
# Sort key: (seconds, microseconds)
key = (1789761495, 779435)

# Ensures chronological order even across files
```

### Memory Considerations

Collection mode loads all decoded data into memory before writing:
- Large collections (1000+ files) may use significant memory
- Typical data product: ~100 KB → ~10 MB decoded
- 1000 files × 10 MB = ~10 GB memory usage
- For very large collections, process in batches

## See Also

- [README.md](README.md) - Complete decoder documentation
- [QUICK_START.md](QUICK_START.md) - Quick reference guide
- [BATCH_PROCESSING.md](BATCH_PROCESSING.md) - Batch processing guide
- [RECONSTRUCTION_MODE.md](RECONSTRUCTION_MODE.md) - Data recovery guide
