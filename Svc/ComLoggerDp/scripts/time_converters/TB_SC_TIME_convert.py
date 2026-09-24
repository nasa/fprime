#!/usr/bin/env python3
"""
TB_SC_TIME converter

Converts J2000 time (seconds since 2000-01-01 12:00:00 UTC) to
ISO format in the specified timezone.

J2000 epoch: 2000-01-01 12:00:00 UTC = Unix timestamp 946728000

Usage:
    python TB_SC_TIME_convert.py <seconds> <microseconds> <context> <timezone>

Arguments:
    seconds      : J2000 seconds (since 2000-01-01 12:00:00 UTC)
    microseconds : Microseconds portion
    context      : Time context (unused but required)
    timezone     : Target timezone (e.g., UTC, America/Los_Angeles)

Output:
    ISO timestamp string: YYYY-MM-DDTHH:MM:SS.mmmmmm
"""

import sys
from datetime import datetime

# J2000 epoch in Unix time: 2000-01-01 12:00:00 UTC
J2000_EPOCH_UNIX = 946728000

def main():
    if len(sys.argv) != 5:
        print(f"Error: Expected 4 arguments, got {len(sys.argv) - 1}", file=sys.stderr)
        print(f"Usage: {sys.argv[0]} <seconds> <microseconds> <context> <timezone>", file=sys.stderr)
        sys.exit(1)

    try:
        j2000_seconds = int(sys.argv[1])
        microseconds = int(sys.argv[2])
        # context = int(sys.argv[3])  # Not used in conversion
        timezone_str = sys.argv[4]

        # Convert J2000 seconds to Unix epoch seconds
        unix_seconds = j2000_seconds + J2000_EPOCH_UNIX
        timestamp = unix_seconds + microseconds / 1000000.0

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
