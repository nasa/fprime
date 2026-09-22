# Batch Processing Guide

## Overview

The ComLoggerDp decoder supports batch processing of entire directories containing data product files. This is useful when you have collected many data products and want to decode them all at once.

## Features

- **Automatic File Discovery**: Finds all data product files matching a specific container ID
- **Parallel Processing**: Processes files sequentially with progress indication
- **Flexible Output**: Choose output directory and format
- **Error Handling**: Continues processing even if individual files fail
- **Summary Report**: Provides statistics on successful/failed processing

## Basic Usage

### Decode All Files in a Directory

```bash
python decode_comlogger_dp.py --input-dir /path/to/dps \
  --dp-id 134217728 \
  --dict-path /path/to/dict
```

This will:
1. Find all files named `Dp_134217728_*.fdp` or `Dp_134217728_*.bin`
2. Decode each file
3. Save output files in the same directory

### Specify Output Directory

```bash
python decode_comlogger_dp.py --input-dir /path/to/dps \
  --dp-id 0x08000000 \
  --dict-path /path/to/dict \
  --output-dir /path/to/output
```

## Command Line Arguments for Batch Mode

### Required Arguments

```bash
--input-dir PATH        # Directory containing data product files
--dp-id ID              # Container ID to filter (decimal or hex)
                        # Examples: 134217728 or 0x08000000
```

### Optional Arguments

```bash
--dict-path PATH        # Dictionary path (auto-detected if omitted)
-d PATH                 # Short form

--output-dir PATH       # Output directory (default: same as input-dir)
                        # Creates subdirectory structure if needed

--format FORMAT         # Output format: json, text, or both (default: both)
-f FORMAT               # Short form

--sentry VALUE          # Override sentry value (e.g., 0xDEADBEEF)
--no-crc                # Skip CRC validation
--no-sentry             # Skip sentry validation
```

### Not Applicable in Batch Mode

These arguments don't work with `--input-dir`:

```bash
--output-file PATH      # Only for --input-file (single file mode)
--reconstruct           # Only for --input-file (reconstruction mode)
```

## Container ID (DP ID)

### What is the Container ID?

The container ID uniquely identifies which data product container the files belong to. In ComLoggerDp deployments, this is typically the base component ID shifted left.

### Finding Your Container ID

#### Method 1: From Filename

Data product filenames follow the pattern: `Dp_<container_id>_<timestamp>_<sequence>.fdp`

Example: `Dp_134217728_1789761500_00043237.fdp`
- Container ID: **134217728** (decimal)
- Hex equivalent: **0x08000000**

#### Method 2: From FPP Topology

Check your topology FPP file for the ComLoggerDp instance:

```fpp
instance comLogger: Svc.ComLoggerDp base id 0x8000
```

The container ID is the base ID shifted left by 20 bits:
- Base ID: `0x8000`
- Container ID: `0x08000000` (134217728 decimal)

#### Method 3: From Existing JSON

If you already have decoded JSON files:

```bash
python -c "import json; print(json.load(open('file.json'))['header']['Id']['value'])"
```

### ID Format

The `--dp-id` argument accepts both decimal and hexadecimal:

```bash
# Decimal
--dp-id 134217728

# Hexadecimal (with 0x prefix)
--dp-id 0x08000000
```

## File Naming Pattern

The decoder searches for files matching these patterns:
- `Dp_<container_id>_*.fdp`
- `Dp_<container_id>_*.bin`

Where `<container_id>` is the decimal representation of your DP ID.

## Output Files

For each input file, the decoder creates output files based on the format:

### Input File
`Dp_134217728_1789761500_00043237.fdp`

### Output Files (format: both)
- `Dp_134217728_1789761500_00043237.json` - Complete structured data
- `Dp_134217728_1789761500_00043237.txt` - Human-readable text

### Output Files (format: json)
- `Dp_134217728_1789761500_00043237.json` - JSON only

### Output Files (format: text)
- `Dp_134217728_1789761500_00043237.txt` - Text only

## Progress Indication

During batch processing, you'll see progress for each file:

```
Found 91 data product file(s) with container ID 134217728

Loading F Prime dictionaries...
Loading dictionary from: /path/to/RefTopologyDictionary.json

================================================================================
Processing file 1/91: Dp_134217728_1789761500_00043237.fdp
================================================================================
Decoding DP header...
Extracting ComBuffer records...
Found 10 ComBuffer records
Validating data CRC...
Data CRC validated successfully
Decoding ComBuffers...

================================================================================
Processing file 2/91: Dp_134217728_1789761506_00043312.fdp
================================================================================
...
```

## Batch Summary

At the end of processing, you'll get a summary:

```
================================================================================
BATCH PROCESSING SUMMARY
================================================================================
Total files: 91
Successful: 91
Failed: 0

Output directory: /path/to/output

Decoding completed successfully!
```

## Error Handling

If individual files fail to decode:

```
================================================================================
Processing file 45/91: Dp_134217728_1789761890_00043139.fdp
================================================================================
Error processing Dp_134217728_1789761890_00043139.fdp: CRC mismatch in Data

... continues with remaining files ...

================================================================================
BATCH PROCESSING SUMMARY
================================================================================
Total files: 91
Successful: 90
Failed: 1

Output directory: /path/to/output
```

The decoder continues processing remaining files even if some fail.

## Examples

### Example 1: Basic Batch Processing

```bash
# Process all files with container ID 134217728
python decode_comlogger_dp.py --input-dir /data/dps --dp-id 134217728
```

**Result**: All matching files decoded with both JSON and text output in `/data/dps/`

### Example 2: JSON Output Only

```bash
# Generate only JSON files
python decode_comlogger_dp.py --input-dir /data/dps \
  --dp-id 0x08000000 \
  --format json
```

**Result**: Only JSON files created in `/data/dps/`

### Example 3: Custom Output Directory

```bash
# Save decoded files to a different location
python decode_comlogger_dp.py --input-dir /data/raw_dps \
  --dp-id 134217728 \
  --output-dir /data/decoded_dps \
  --dict-path /path/to/dict
```

**Result**: Decoded files saved to `/data/decoded_dps/`

### Example 4: Text Output with Custom Sentry

```bash
# Decode with custom sentry value
python decode_comlogger_dp.py --input-dir /data/dps \
  --dp-id 134217728 \
  --sentry 0x12345678 \
  --format text
```

**Result**: Text files with custom sentry validation

### Example 5: Skip Validation for Corrupted Files

```bash
# Process files even if checksums fail
python decode_comlogger_dp.py --input-dir /data/dps \
  --dp-id 134217728 \
  --no-crc \
  --no-sentry
```

**Result**: All files decoded, validation errors ignored

## Performance

### Processing Speed

- **Small files (2-4 KB)**: ~0.5-1 second per file
- **Large files (>10 KB)**: ~1-2 seconds per file

### Example Benchmark

Processing 91 data product files (each ~3 KB):
- Total time: ~60 seconds
- Average: ~0.66 seconds per file
- Output size: ~360 KB total

## Tips

1. **Use Wildcards Carefully**: The `--dp-id` must match exactly. If no files are found, verify the container ID.

2. **Check Dictionary**: Make sure your dictionary matches the deployment that generated the data products.

3. **Disk Space**: Ensure sufficient disk space. Decoded files are typically 1.5-2x the size of input files.

4. **Large Batches**: For very large directories (1000+ files), consider processing in smaller batches or using `--format text` to reduce output size.

5. **Validation**: For production use, keep CRC and sentry validation enabled (default) to detect corrupted files.

## Troubleshooting

### No Files Found

```
Error: No data product files found matching container ID 134217728
```

**Solutions**:
- Verify the container ID is correct
- Check if files use `.bin` extension instead of `.fdp`
- List directory contents to confirm filename pattern

### Permission Errors

```
Error: Permission denied: /path/to/output
```

**Solutions**:
- Ensure write permissions for output directory
- Try a different output directory
- Use `sudo` if appropriate

### Memory Issues

For very large batches, if you encounter memory issues:
- Process files in smaller batches
- Use `--format text` instead of `both` to reduce memory usage
- Close other applications

## See Also

- [README.md](README.md) - Complete documentation
- [QUICK_START.md](QUICK_START.md) - Quick reference guide
- [TEST_RESULTS.md](TEST_RESULTS.md) - Validation and test results
