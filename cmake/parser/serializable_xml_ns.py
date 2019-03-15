#! /usr/bin/env python

import os
import sys

import xml.etree.ElementTree as ET

def get_serializable_qualified(input_file):
    tree = ET.parse(input_file)
    root = tree.getroot()
    spath = root.get("namespace").replace("::", "/").replace(":", "/") + "/" + root.get("name")

    # Print without newline
    sys.stdout.write(spath)
    sys.stdout.flush()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: serializable_xml_ns.py <input_Ai.xml>")
        exit(1)
    get_serializable_qualified(sys.argv[1])
