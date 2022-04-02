""" Purge HTML directory of files with _ prefix

Script to purge a directory of files starting with _ as this does not host well with GH pages and
thus these files need to be rewritten. This file is designed for speed in these replacements.
"""
from __future__ import (division,  # To work with any system python
                        print_function)

import os
import re
import shutil
import sys
from multiprocessing import Pool


def converter(input_data):
    """ Conversion function for thread pooling """
    convert, changes  = input_data
    def corrector(line):
        """ Corrects all instances needing changing """
        for fix, needle in changes:
            line = line.replace(needle, fix)
        return line
    with open(convert, "r") as file_handle:
        try:
            read_lines = file_handle.readlines()
        except UnicodeDecodeError:
            if convert.endswith(".html") or convert.endswith(".js"):
                raise
            return
    corrected = [corrector(line) for line in read_lines]
    with open(convert, "w") as file_handle:
        file_handle.write("".join(corrected))

def absolution(all_files, changes):
    """ Corrects all input files with the list of changed pairs """
    total_len = len(all_files)
    # Start a thread pool
    data = zip(all_files, [changes] * total_len) 
    pool = Pool()
    for i, _ in enumerate(pool.imap_unordered(converter, list(data)), 1):
        print("[INFO] Updating {}/{}                ".format(i, total_len), end="\r")
        sys.stdout.flush()
    print()


def main():
    """ Main program entry """
    if len(sys.argv) < 2 or not os.path.isdir(sys.argv[1]):
        print("[ERROR] Must supply a directory to correct", file=sys.stderr)
        sys.exit(1)
    directory = sys.argv[1]
    all_files = os.listdir(directory)
    # Detect broken files
    defunct_files = [potential for potential in all_files if potential[0] == "_"]
    corrected = [old[1:] for old in defunct_files]
    # Move broken files
    for mover, movie in zip(defunct_files, corrected):
        shutil.move(os.path.join(directory, mover), os.path.join(directory, movie))
    # Rewrite updates for above files
    corrections_in = ['"{}'.format(correction) for correction in defunct_files] + \
                     ['/{}'.format(correction) for correction in defunct_files]
    corrections_out = ['"{}'.format(correction) for correction in corrected] + \
                      ['/{}'.format(correction) for correction in corrected]
    full_files = [os.path.join(path, file) for path, _, files in os.walk(directory) for file in files]
    mappings = list(zip(corrections_out, corrections_in))
    #for inin, outout in mappings:
    #    print("{} -> {}".format(inin, outout))
    absolution(full_files, mappings)


if __name__ == "__main__":
    main()
