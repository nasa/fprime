import multiprocessing
import os
import subprocess
import sys
from pathlib import Path


class ExceptionWithStandardError(Exception):
    """Exception plus standard error"""

    def __init__(self, message, stderr):
        message = f"{message}\n{stderr}"
        super().__init__(message)


def run_process(directory, arguments):
    """Runs the fpp-depend process"""
    working_dir = directory / "fpp-cache"
    working_dir.mkdir(parents=True, exist_ok=True)
    with open(working_dir / "stdout.txt", "w") as capture_file:
        return subprocess.Popen(
            arguments,
            cwd=working_dir,
            stdin=subprocess.DEVNULL,
            stdout=capture_file,
            stderr=subprocess.PIPE,
            text=True,
        )


def raise_on_error(process, arguments):
    """Waits for process, raises on error"""
    code = process.wait()
    if code != 0:
        raise ExceptionWithStandardError(
            f"Failed to run '{' '.join(arguments)}'", process.stderr.read()
        )


def run_parallel_depend(fpp_depend, fpp_locs, file_input):
    """Runs the fpp-depend tool in massive parallel"""
    max_jobs = int(os.environ.get("PARALLEL_JOBS", multiprocessing.cpu_count()))
    with open(file_input, "r") as file_handle:
        lines = file_handle.readlines()

    # Loop over the lines of the file
    wait_list = []
    for line in lines:
        arguments = line.strip().split(";")  # Using CMake separator
        directory = arguments.pop(0)
        full_list = (
            [fpp_depend, fpp_locs]
            + arguments
            + [
                "-d",
                "direct.txt",
                "-m",
                "missing.txt",
                "-f",
                "framework.txt",
                "-g",
                "generated.txt",
                "-i",
                "include.txt",
            ]
        )
        wait_list.append((run_process(Path(directory), full_list), full_list))

        if len(wait_list) >= max_jobs:
            raise_on_error(*wait_list.pop(0))

    for proc, args in wait_list:
        raise_on_error(proc, args)


def main():
    try:
        _, fpp_depend, fpp_locs, file_input = sys.argv
        run_parallel_depend(fpp_depend, fpp_locs, file_input)
        sys.exit(0)
    except ValueError:
        print(
            "[ERROR] Bad arguments: fpp-depend path, locs file path, file input and integer PARALLEL_JOBS env var."
        )
    except Exception as exc:
        print(f"[ERROR] {exc}")
    sys.exit(-1)


if __name__ == "__main__":
    main()
