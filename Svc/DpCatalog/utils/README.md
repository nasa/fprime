# DpCatalog Utilities

This directory contains utilities for working with the DpCatalog component.

## csv_to_dp_ops.py

Converts CSV files to binary DP operations format for use with the `PROCESS_DP_FILE` command.

### Usage

```bash
./csv_to_dp_ops.py [-h] [-v] csv_file output_file
```

**Arguments:**
- `csv_file` - Input CSV file path
- `output_file` - Output binary file path
- `-v, --verbose` - Print verbose output
- `-h, --help` - Show help message

### CSV Format

The input CSV file must have the following columns:

| Column    | Description                                    | Valid Values                          |
|-----------|------------------------------------------------|---------------------------------------|
| Operation | Operation type                                 | DELETE, REPRIORITIZE, RETRANSMIT      |
| ID        | Data product ID                                | 0 to 4294967295                       |
| tSec      | Generation time in seconds                     | 0 to 4294967295                       |
| tSub      | Generation time in subseconds                  | 0 to 4294967295                       |
| Priority  | Priority value                                 | 0 to 4294967295 (0xFFFFFFFF for RETRANSMIT means use file priority) |

**Note:** Integer fields support decimal (123), hexadecimal (0x7B), and octal (0o173) notation.

### Operation Types

- **DELETE** - Removes the specified data product from the catalog and deletes the file from the filesystem. The Priority field is ignored.

- **REPRIORITIZE** - Changes the priority of the specified data product in the catalog tree and state file. The Priority field specifies the new priority value.

- **RETRANSMIT** - Re-adds a transmitted data product to the catalog for retransmission. If the data product is already pending transmission, its priority is updated. The Priority field specifies the priority (0xFFFFFFFF = 4294967295 means use the priority stored in the file).

### Example CSV

See [example_operations.csv](example_operations.csv) for a sample input file.

```csv
Operation,ID,tSec,tSub,Priority
DELETE,123,1000,100,0
REPRIORITIZE,234,2000,200,5
RETRANSMIT,345,3000,300,10
RETRANSMIT,456,4000,400,4294967295
```

### Example Usage

```bash
# Convert CSV to binary format
./csv_to_dp_ops.py example_operations.csv operations.dat

# Convert with verbose output
./csv_to_dp_ops.py -v example_operations.csv operations.dat

# Upload to spacecraft and execute command
# (This is deployment-specific)
```

### Binary Format

Each record in the output file is exactly 17 bytes:

| Offset | Size | Field     | Description                                    |
|--------|------|-----------|------------------------------------------------|
| 0      | 1    | Operation | 1=DELETE, 2=REPRIORITIZE, 3=RETRANSMIT        |
| 1      | 4    | ID        | Data product ID (U32, big-endian)             |
| 5      | 4    | tSec      | Time seconds (U32, big-endian)                |
| 9      | 4    | tSub      | Time subseconds (U32, big-endian)             |
| 13     | 4    | Priority  | Priority value (U32, big-endian)              |

After all records, a single 4-byte CRC32 checksum (U32, big-endian) is appended. The CRC32 is calculated over all record data and provides file integrity validation.

### Error Handling

The script validates:
- Required CSV columns are present
- Operation names are valid (case-insensitive)
- All numeric fields are valid 32-bit unsigned integers
- File exists and is readable

If any validation fails, the script exits with an error message and non-zero exit code.

### Testing

To test the utility with the example file:

```bash
# Create binary file
./csv_to_dp_ops.py -v example_operations.csv test_ops.dat

# Verify file size (should be 72 bytes: 68 bytes data + 4 bytes CRC32)
ls -l test_ops.dat

# View binary content (hexdump)
hexdump -C test_ops.dat
```

Expected output for example file:
```
00000000  01 00 00 00 7b 00 00 03  e8 00 00 00 64 00 00 00  |....{.......d...|
00000010  00 02 00 00 00 ea 00 00  07 d0 00 00 00 c8 00 00  |................|
00000020  00 05 03 00 00 01 59 00  00 0b b8 00 00 01 2c 00  |......Y.......,.|
00000030  00 00 0a 03 00 00 01 c8  00 00 0f a0 00 00 01 90  |................|
00000040  ff ff ff ff                                       |....|
```

## Integration with DpCatalog

To use the generated binary file with DpCatalog:

1. Generate the binary operations file using this utility
2. Upload the file to the spacecraft filesystem
3. Execute the `PROCESS_DP_FILE` command with the file path as an argument
4. DpCatalog will process each operation sequentially and emit events for each action
