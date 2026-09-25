#!/usr/bin/env python3
"""
TB_WORKSTATION_TIME converter

Converts Unix epoch time (seconds since 1970-01-01 00:00:00 UTC) to
ISO format in the specified timezone.

Usage:
    python TB_WORKSTATION_TIME_convert.py <seconds> <microseconds> <context> <timezone>

Arguments:
    seconds      : Unix epoch seconds
    microseconds : Microseconds portion
    context      : Time context (unused but required)
    timezone     : Target timezone (e.g., UTC, America/Los_Angeles)

Output:
    ISO timestamp string: YYYY-MM-DDTHH:MM:SS.mmmmmm
"""

import sys
from datetime import datetime

def main():
    if len(sys.argv) != 5:
        print(f"Error: Expected 4 arguments, got {len(sys.argv) - 1}", file=sys.stderr)
        print(f"Usage: {sys.argv[0]} <seconds> <microseconds> <context> <timezone>", file=sys.stderr)
        sys.exit(1)

    try:
        seconds = int(sys.argv[1])
        microseconds = int(sys.argv[2])
        # context = int(sys.argv[3])  # Not used in conversion
        timezone_str = sys.argv[4]

        # Create timestamp from Unix epoch
        timestamp = seconds + microseconds / 1000000.0

        # Convert to datetime
        if timezone_str.upper() == "UTC":
            # Simple UTC case
            dt = datetime.utcfromtimestamp(timestamp)
        else:
            # Try to use zoneinfo (Python 3.9+)
            try:
                from zoneinfo import ZoneInfo
                from datetime import timezone as tz
                dt_utc = datetime.fromtimestamp(timestamp, tz=tz.utc)
                dt = dt_utc.astimezone(ZoneInfo(timezone_str))
            except ImportError:
                # Fall back to pytz for older Python
                try:
                    import pytz
                    tz_obj = pytz.timezone(timezone_str)
                    dt_utc = datetime.utcfromtimestamp(timestamp)
                    dt_utc = pytz.utc.localize(dt_utc)
                    dt = dt_utc.astimezone(tz_obj)
                    # Remove tzinfo to match GDS format (no timezone suffix)
                    dt = dt.replace(tzinfo=None)
                except ImportError:
                    print(f"Error: Neither zoneinfo nor pytz available for timezone conversion", file=sys.stderr)
                    sys.exit(1)
                except Exception as e:
                    print(f"Error: Invalid timezone '{timezone_str}': {e}", file=sys.stderr)
                    sys.exit(1)

        # Format as ISO timestamp (matching GDS format)
        iso_time = dt.strftime("%Y-%m-%dT%H:%M:%S.%f")
        print(iso_time)

    except ValueError as e:
        print(f"Error: Invalid numeric argument: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
