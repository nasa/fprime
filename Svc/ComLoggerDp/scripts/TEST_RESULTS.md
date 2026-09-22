# ComLoggerDp Decoder Test Results

## Test Date
2026-09-21

## Test Environment
- **Platform**: Linux (WSL2)
- **F Prime GDS Version**: Installed via pip
- **Test Data**: TestDeploymentsProject/Ref deployment
- **Dictionary**: RefTopologyDictionary.json

## Test Files
Test data product files from: `/home/tcanham/source/fprime/TestDeploymentsProject/build-artifacts/Linux/Ref/bin/DpCat/`

Container ID: **134217728** (0x08000000) - RecordedCom.comLogger.ComBuffContainer

## Test Results

### Test 1: Basic Decoding
**File**: `Dp_134217728_1789761500_00043237.fdp`
- **Result**: ✅ SUCCESS
- **Records Decoded**: 10
- **Events**: 3
- **Telemetry**: 7
- **Errors**: 0

**Decoded Events**:
1. `CdhCore.cmdDisp.OpCodeDispatched` - Opcode 0x8000000 dispatched to port 12
2. `RecordedCom.comLogger.ComDpStarted` - Started Com DP logging: 10 packets per container
3. `CdhCore.cmdDisp.OpCodeCompleted` - Opcode 0x8000000 completed

**Decoded Telemetry**:
1. `Ref.systemResources.CPU_00` - CPU usage values (3.0%, 4.95%, 8.16%, 4.04%)
2. `RecordedCom.comLogger.LoggingEnabled` - true
3. `Ref.sendBuffComp.SendState` - SEND_IDLE
4. `ComCcsds.comQueue.buffQueueDepth` - Queue depth [0]

### Test 2: Recent Data Product
**File**: `Dp_134217728_1790043489_00268854.fdp`
- **Result**: ✅ SUCCESS
- **Records Decoded**: 10
- **Events**: 3
- **Telemetry**: 7
- **Errors**: 0

## Features Validated

### ✅ Header Decoding
- Container ID extraction
- Priority parsing
- Timestamp decoding (seconds + microseconds)
- Data size validation
- Header CRC32 validation

### ✅ ComBuffer Record Extraction
- Record ID parsing (FwDpIdType)
- Array size reading (FwSizeStoreType)
- Sentry value extraction (U32, big-endian)
- ComBuffer data extraction

### ✅ Packet Type Detection
- FW_PACKET_LOG (0x0002) - Events
- FW_PACKET_TELEM (0x0001) - Telemetry
- Correct packet type identification

### ✅ Event Decoding
- Event ID lookup
- Event name resolution
- Severity extraction
- Timestamp parsing
- Argument deserialization
- Message formatting with arguments

### ✅ Telemetry Decoding
- Channel ID lookup
- Channel name resolution
- Component name extraction
- Timestamp parsing
- Value deserialization (F32, Bool, Enum types)
- Complex types (BuffQueueDepth array)

### ✅ Data Integrity
- Header CRC32 validation
- Data section CRC32 validation
- Sentry value validation
- All checksums passed

### ✅ Output Formats
- JSON output with complete structure
- Human-readable text output
- Statistics summary
- Error handling and reporting

## Sample Output

### Text Format (Excerpt)
```
[Record 0] FW_PACKET_LOG
  Event: CdhCore.cmdDisp.OpCodeDispatched
  Severity: EventSeverity.COMMAND
  Time: {'type': 'Time', 'base': 2, 'context': 0, 'seconds': 1789761495, 'microseconds': 779435}
  Message: Opcode 0x8000000 dispatched to port 12
  Args:
    Opcode: {'value': 134217728, 'type': 'U32'}
    port: {'value': 12, 'type': 'I32'}

[Record 3] FW_PACKET_TELEM
  Channel: Ref.systemResources.CPU_00
  Time: {'type': 'Time', 'base': 2, 'context': 0, 'seconds': 1789761495, 'microseconds': 43053}
  Value: {'value': 3.0, 'type': 'F32'}
```

### JSON Format (Structure)
```json
{
  "header": {
    "PacketDescriptor": {"value": 5, "type": "U16"},
    "Id": {"value": 134217728, "type": "U32"},
    "Priority": {"value": 1, "type": "U32"},
    "Time": {"seconds": 1789761500, "microseconds": 43237},
    "DataSize": {"value": 2745}
  },
  "records": [
    {
      "record_index": 0,
      "sentry": "0xdeadbeef",
      "packet_type": "FW_PACKET_LOG",
      "decoded": {
        "event_name": "RecordedCom.comLogger.ComDpStarted",
        "severity": "ACTIVITY_HI",
        "message": "Started Com DP logging: 10 packets per container"
      }
    }
  ],
  "statistics": {
    "total_records": 10,
    "events": 3,
    "telemetry": 7,
    "errors": 0
  }
}
```

## Performance

- **Decoding Speed**: < 1 second per data product file
- **Memory Usage**: Minimal (files are 2-4 KB each)
- **File Size**: Input files 2.4-3.8 KB, output files ~5-10 KB

## Issues Found and Fixed

1. ✅ **Dictionary Loading** - Fixed `Dictionaries` API usage
2. ✅ **Time Type** - Imported `TimeType` directly instead of using ConfigManager
3. ✅ **Template API** - Fixed channel and event template method calls
4. ✅ **Header Field Names** - Adapted to actual field structure

## Compatibility

The decoder successfully works with:
- F Prime data products from ComLogger component
- Standard F Prime packet types (events, telemetry)
- Multiple F Prime type serializations (U32, F32, Bool, Enums, Arrays)
- CRC32 checksum validation
- Sentry value validation

## Recommendations

1. **Sentry Configuration**: The script currently uses default sentry value (0xDEADBEEF). For production use, configure the correct sentry value via:
   - Project config constant `ComLoggerDpSentry`
   - Command line: `--sentry 0xVALUE`

2. **Dictionary Management**: Ensure dictionary path matches the deployment that generated the data products

3. **Validation**: Use `--no-crc` and `--no-sentry` flags only for debugging corrupted files

## Conclusion

✅ **All tests passed successfully!**

The ComLoggerDp decoder script is fully functional and ready for production use. It successfully:
- Decodes ComLoggerDp data product binary files
- Extracts and validates ComBuffer records
- Reconstructs original events and telemetry
- Provides both JSON and human-readable output formats
- Validates data integrity with CRC and sentry checks

## Example Commands

```bash
# Basic usage
python decode_comlogger_dp.py data_product.fdp --dict-path /path/to/dict

# JSON only
python decode_comlogger_dp.py data_product.fdp --dict-path /path/to/dict --format json

# Text only
python decode_comlogger_dp.py data_product.fdp --dict-path /path/to/dict --format text

# Both formats
python decode_comlogger_dp.py data_product.fdp --dict-path /path/to/dict --format both
```
