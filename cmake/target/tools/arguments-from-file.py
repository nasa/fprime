import sys
import subprocess

if len(sys.argv) < 2:
    print("[ERROR] Supply output file then tool")
    sys.exit(1)
with open(sys.argv[1], "r") as file_handle:
    args = sys.argv[2:] + [line.strip() for line in file_handle.readlines()]
# Discard tool stdout: argv[1] is a dependency cache and must not be overwritten
sys.exit(subprocess.run(args, stdout=subprocess.DEVNULL).returncode)
