#!/usr/bin/env python3
"""
TB_PROC_TIME converter

Returns raw time value with context. No conversion is performed since
processor cycle time is not tied to external time.

Usage:
    python TB_PROC_TIME_convert.py <seconds> <microseconds> <context> <timezone>

Arguments:
    seconds      : Raw processor cycle seconds
    microseconds : Microseconds portion
    context      : Time context
    timezone     : Target timezone (ignored for raw values)

Output:
    Raw value string: <seconds>.<microseconds> (ctx=<context>)
"""

import sys

def main():
    if len(sys.argv) != 5:
        print(f"Error: Expected 4 arguments, got {len(sys.argv) - 1}", file=sys.stderr)
        print(f"Usage: {sys.argv[0]} <seconds> <microseconds> <context> <timezone>", file=sys.stderr)
        sys.exit(1)

    try:
        seconds = int(sys.argv[1])
        microseconds = int(sys.argv[2])
        context = int(sys.argv[3])
        # timezone is ignored for raw values

        # Format as raw value with context
        raw_value = f"{seconds}.{microseconds:06d} (ctx={context})"
        print(raw_value)

    except ValueError as e:
        print(f"Error: Invalid numeric argument: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
