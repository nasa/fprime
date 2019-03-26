#! /usr/bin/env python

import os
import sys

import xml.etree.ElementTree as ET
from argparse import ArgumentParser

def get_component_headers(input_file):
    tree = ET.parse(input_file)
    root = tree.getroot()

    # Find component headers
    # Generate cmake list
    hpp_set = set()
    for component in root.findall('import_component_type'):
        component_prefix = component.text[:-6]
        component_header = component_prefix + 'Ac.hpp'
        hpp_set.add(component_header)

    hpp_str = ''
    for header in hpp_set:
        hpp_str += header + ';'

    # Remove last cmake list seperator
    hpp_str = hpp_str[:-1]

    # Print without newline
    sys.stdout.write(hpp_str)
    sys.stdout.flush()

def get_component_targets(input_file):
    tree = ET.parse(input_file)
    root = tree.getroot()

    # Find component headers
    # Use set to remove duplicates
    target_set = set()
    for component in root.findall('import_component_type'):
        component_library = os.path.dirname(component.text)
        target_set.add(component_library)

    target_str = ''
    for component in target_set:
        target_str += component + ';'

    # Remove last cmake list seperator
    target_str = target_str[:-1]

    # Change seperators
    target_str = target_str.replace('/', '_')

    # Print without newline
    sys.stdout.write(target_str)
    sys.stdout.flush()

def find_name(input_file):
    tree = ET.parse(input_file)
    root = tree.getroot()

    name = root.attrib['name']

    # Print without newline
    sys.stdout.write(name)
    sys.stdout.flush()

if __name__ == "__main__":

    parser = ArgumentParser(description='Process toplogy xml.')
    parser.add_argument(
        'toplogy_xml',
        metavar='xml',
        help='Toplogy XML to parse'
    )
    parser.add_argument(
        '--headers',
        action='store_true',
        help='Find component header dependencies and output to stdout in cmake list format'
    )
    parser.add_argument(
        '--targets',
        action='store_true',
        help='Find component target dependencies and output to stdout in cmake list format'
    )
    parser.add_argument(
        '--name',
        action='store_true',
        help='Find toplogy name and output to stdout'
    )

    args = parser.parse_args()

    if(args.headers):
        get_component_headers(args.toplogy_xml)
    if(args.name):
        find_name(args.toplogy_xml)
    if(args.targets):
        get_component_targets(args.toplogy_xml)
