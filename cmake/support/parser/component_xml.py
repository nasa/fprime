#! /usr/bin/env python

import os
import sys

import xml.etree.ElementTree as ET

def get_port_headers(input_file):
    tree = ET.parse(input_file)
    root = tree.getroot()

    # Find port headers
    # Generate cmake list
    hpp_str = ''
    for port in root.findall('import_port_type'):
        port_prefix = port.text[:-6]
        port_header = port_prefix + "Ac.hpp"
        hpp_str += port_header + ';'

    # Remove last cmake list seperator
    hpp_str = hpp_str[:-1]

    # Print without newline
    sys.stdout.write(hpp_str)
    sys.stdout.flush()

def get_port_targets(input_file, library_name):
    tree = ET.parse(input_file)
    root = tree.getroot()

    # Find port headers
    # Use set to remove duplicates
    target_set = set()
    for port in root.findall('import_port_type'):
        port_library = os.path.dirname(port.text)

        # Change seperators
        port_library = port_library.replace('/', '_')

        # Do not add self dependency
        if(port_library != library_name):
            target_set.add(port_library)

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
        print("Usage: component_xml.py <input_Ai.xml> <library name>")
        exit(1)

    get_port_targets(sys.argv[1], sys.argv[2])
