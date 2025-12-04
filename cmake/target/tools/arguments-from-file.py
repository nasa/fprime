import sys
import subprocess

if len(sys.argv) < 2:
    print("[ERROR] Supply output file then tool")
    sys.exit(1)
with open(sys.argv[1], "r") as file_handle:
    args = sys.argv[2:] + [line.strip() for line in file_handle.readlines()]
    sys.exit(subprocess.run(args, stdout=file_handle).returncode)
