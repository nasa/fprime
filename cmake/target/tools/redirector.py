import sys
import subprocess

if len(sys.argv) < 2:
    print("[ERROR] Supply output file then tool")
    sys.exit(1)
with open(sys.argv[1], "w") as file_handle:
    sys.exit(subprocess.run(sys.argv[2:], stdout=file_handle).returncode)
