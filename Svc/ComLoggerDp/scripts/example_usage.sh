#!/bin/bash
#
# Example usage script for decode_comlogger_dp.py
#
# This script demonstrates common usage patterns for the ComLoggerDp decoder.
#

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DECODER="${SCRIPT_DIR}/decode_comlogger_dp.py"

echo "ComLoggerDp Decoder - Example Usage"
echo "===================================="
echo ""

# Check if a data product file is provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <data_product.bin> [dict_path]"
    echo ""
    echo "This script demonstrates various ways to use the ComLoggerDp decoder."
    echo ""
    echo "Examples:"
    echo "  $0 /path/to/data_product.bin"
    echo "  $0 /path/to/data_product.bin /path/to/dict"
    echo ""
    echo "Available decoder options:"
    echo "  --format {json,text,both}  : Output format (default: both)"
    echo "  --dict-path PATH           : Path to F Prime dictionaries"
    echo "  --sentry 0xVALUE           : Override sentry value"
    echo "  --no-crc                   : Skip CRC validation"
    echo "  --no-sentry                : Skip sentry validation"
    echo ""
    exit 1
fi

DP_FILE="$1"
DICT_PATH="${2:-}"

# Check if file exists
if [ ! -f "$DP_FILE" ]; then
    echo "Error: Data product file not found: $DP_FILE"
    exit 1
fi

echo "Data Product File: $DP_FILE"
echo ""

# Example 1: Basic decode with auto-detected dictionary
echo "Example 1: Basic decode (JSON + Text output)"
echo "----------------------------------------------"
if [ -z "$DICT_PATH" ]; then
    python3 "$DECODER" "$DP_FILE" --format both
else
    python3 "$DECODER" "$DP_FILE" --format both --dict-path "$DICT_PATH"
fi
echo ""

# Example 2: JSON output only
echo "Example 2: JSON output only"
echo "---------------------------"
OUTPUT_JSON="${DP_FILE%.bin}_decoded.json"
if [ -z "$DICT_PATH" ]; then
    python3 "$DECODER" "$DP_FILE" --format json --output "$OUTPUT_JSON"
else
    python3 "$DECODER" "$DP_FILE" --format json --output "$OUTPUT_JSON" --dict-path "$DICT_PATH"
fi
echo ""

# Example 3: Human-readable text output only
echo "Example 3: Text output only"
echo "---------------------------"
OUTPUT_TXT="${DP_FILE%.bin}_decoded.txt"
if [ -z "$DICT_PATH" ]; then
    python3 "$DECODER" "$DP_FILE" --format text --output "$OUTPUT_TXT"
else
    python3 "$DECODER" "$DP_FILE" --format text --output "$OUTPUT_TXT" --dict-path "$DICT_PATH"
fi
echo ""

# Show output file locations
echo "=================================="
echo "Output files generated:"
echo "  JSON: ${DP_FILE%.bin}.json"
echo "  Text: ${DP_FILE%.bin}.txt"
echo "  Custom JSON: $OUTPUT_JSON"
echo "  Custom Text: $OUTPUT_TXT"
echo ""
echo "To view the decoded output:"
echo "  cat ${DP_FILE%.bin}.txt"
echo "  jq . ${DP_FILE%.bin}.json"
echo ""
