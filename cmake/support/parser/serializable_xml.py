#! /usr/bin/env python

import os
import sys

import xml.etree.ElementTree as ET

def get_serializable_headers(input_file):
    tree = ET.parse(input_file)
    root = tree.getroot()

    # Find serializable headers
    # Generate cmake list
    hpp_str = ''
    for ser in root.findall('import_serializable_type'):
        ser_xml = ser.text
        ser_header = ser_xml[:-6] + "Ac.hpp"
        hpp_str += ser_header + ';'

    # Remove last cmake list seperator
    hpp_str = hpp_str[:-1]

    # Print without newline
    sys.stdout.write(hpp_str)
    sys.stdout.flush()

def get_serializable_targets(input_file, library_name):
    tree = ET.parse(input_file)
    root = tree.getroot()

    # Find serializable headers
    # Use set to remove duplicates
    target_set = set()
    for ser in root.findall('import_serializable_type'):
        ser_library = os.path.dirname(ser.text)

        # Change seperators
        ser_library = ser_library.replace('/', '_')

        # Do not add self dependency
        if(ser_library != library_name):
            target_set.add(ser_library)

    target_str = ''
    # Convert to cmake list
    for target in target_set:
        # Add cmake list seperator
        target_str += str(target) + ';'

    # Remove last cmake list seperator
    target_str = target_str[:-1]

    # Print without newline
    sys.stdout.write(target_str)
    sys.stdout.flush()

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: serializable_xml.py <input_Ai.xml> <module_name>")
        exit(1)

    get_serializable_targets(sys.argv[1], sys.argv[2])
