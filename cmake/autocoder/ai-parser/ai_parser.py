#!/usr/bin/env python3
"""
Utils:

Utility functions used to support the quick-parsing of the Ai.xml files used in the CMake system. This helps detect
automatic dependencies, and registering into the build system.

@author mstarch
"""
from __future__ import print_function

import os
import sys
import xml.etree.ElementTree

# Dependencies required by basic types
BASIC_DEPENDENCIES = {
    "component": ["Fw_Cfg", "Fw_Types"],
    "port": ["Fw_Cfg", "Fw_Types", "Fw_Port"],
    "interface": ["Fw_Cfg", "Fw_Types", "Fw_Port"],
    "assembly": ["Fw_Cfg", "Fw_Types"],
    "serializable": ["Fw_Cfg", "Fw_Types"],
    "commands": ["Fw_Cfg", "Fw_Types", "Fw_Time", "Fw_Com", "Fw_Cmd"],
    "events": ["Fw_Cfg", "Fw_Types", "Fw_Time", "Fw_Com", "Fw_Log"],
    "telemetry": ["Fw_Cfg", "Fw_Types", "Fw_Time", "Fw_Com", "Fw_Tlm"],
    "parameters": ["Fw_Cfg", "Fw_Types", "Fw_Prm", "Fw_Cmd"],
    "internal_interfaces": [],
    "enum": [],
    "packets": ["Fw_Types", "Svc_TlmPacketizer"],
    "array": [],
}
# Component type importations
KIND_DEPENDENCIES = {
    "passive": ["Fw_Comp"],
    "queued": ["Fw_CompQueued"],  # Depends on the queued portion of Fw_Comp
    "active": ["Fw_CompQueued"],
    "guarded_input": ["Os"],
}

XML_TAG_REMAP = {
    "interface": "Port",
    "assembly": "TopologyApp",
}

# Dependency Ordering:
#
# On Linux/GCC the order of the link libraries actually matters. This means dependencies ordered
# here in this parser must have global ordering information. Thus, output will be in the following
# order in order to make Linux/GCC happy.
#
DEPENDENCY_ORDER = [
    "Fw_Cfg",
    "Fw_Types",
    "Fw_Port",
    "Fw_Time",
    "Fw_Com",
    "Fw_Cmd",
    "Fw_Log",
    "Fw_Tlm",
    "Fw_Prm",
    "Fw_Comp",
    "Os",
    "Fw_CompQueued",
]
# Oh, also dependencies need to be listed in reverse order.
DEPENDENCY_ORDER.reverse()


def main():
    """
    Read arguments and run!
    """
    if len(sys.argv) != 4:
        print(
            "[ERROR] Failed to run, need 4 arguments. Usage:\n\t{0} <input_Ai.xml> <library name> <import base>".format(
                sys.argv[0]
            ),
            file=sys.stderr,
        )
        sys.exit(1)
    elif not os.path.exists(sys.argv[1]):
        print(
            "[ERROR] {0} does not exist. Usage:\n\t{1} <input_Ai.xml> <library name> <import base>".format(
                sys.argv[1], sys.argv[0]
            ),
            file=sys.stderr,
        )
        sys.exit(2)
    elif not os.path.isdir(sys.argv[3]):
        print(
            "[ERROR] {0} does not exist. Usage:\n\t{1} <input_Ai.xml> <library name> <import base>".format(
                sys.argv[3], sys.argv[0]
            ),
            file=sys.stderr,
        )
        sys.exit(3)
    # Read the xml tree
    tree = xml.etree.ElementTree.parse(sys.argv[1])
    root = tree.getroot()
    print(XML_TAG_REMAP.get(root.tag, root.tag.capitalize()))
    print_fprime_dependencies(root, *sys.argv[2:])
    print("")
    print_file_dependencies(root, sys.argv[3])
    print("")


def print_file_dependencies(root, import_base):
    """
    Print F prime dependencies. These should be printed w/o a newlines.

    :param root: input XML file to parse parsed, root of tree
    :param current_library: current library to strip from dependencies
    :param import_base: base of imported dictionaries, used for finding and opening them
    """
    dicts = map(
        lambda x: os.path.join(import_base, x.text), root.findall("import_dictionary")
    )
    sys.stdout.write(";".join(dicts))
    sys.stdout.flush()


def print_fprime_dependencies(root, current_library, import_base):
    """
    Print F prime dependencies. These should be printed w/o a newlines.

    :param root: input XML file to parse parsed, root of tree
    :param current_library: current library to strip from dependencies
    :param import_base: base of imported dictionaries, used for finding and opening them
    """
    dependencies = read_xml_file(root, import_base)
    if current_library in dependencies:
        dependencies.remove(current_library)
    # Rebuilding is broken by non-deterministic set behavior, so this makes it a reverse-sorted (deterministic) list
    dependencies = list(dependencies)
    dependencies.sort()
    dependencies.reverse()
    # Go in order, for GCC
    gcc_order = []
    for dep in DEPENDENCY_ORDER:
        if dep in dependencies:
            gcc_order.append(dep)
    # Find module dependencies, and anything else left over goes last
    for dep in dependencies:
        if dep not in gcc_order:
            gcc_order.append(dep)
    # Write out CMake style list
    sys.stdout.write(";".join(gcc_order))
    sys.stdout.flush()


def read_fprime_import(import_type, root):
    """
    Read an FPrime dependency port/type. These are typically ports, serializables, and components.

    :param import_type: "import_port_type", "import_component_type", "import_serializable_type", "import_enum_type", "import_topology"
    :param root: root XML to parse
    :return: dependency mined from this item
    """
    deps = set()
    for imp in root.findall(import_type):
        imp_lib = os.path.dirname(imp.text).replace("/", "_")
        deps.add(imp_lib)
    return deps


def read_xml_file(root, import_base):
    """
    Reads an Ai.xml file, detects the type, and then pulls in the dependencies. Recursively looks for dependencies in
    import_dictionaries by inferring dependencies for each type of dictionary.

    :param root: input XML file to parsed, root of tree
    :param import_base: base of imported dictionaries, used for finding and opening them
    :return: dependencies set
    """
    # Read the file into the XML libraries
    ai_type = root.tag
    dependencies = set(BASIC_DEPENDENCIES[ai_type])
    kind = root.attrib.get("kind", None)
    if kind in KIND_DEPENDENCIES:
        dependencies.update(KIND_DEPENDENCIES[kind])
    # Import component/serializable/port types
    for import_type in [
        "import_port_type",
        "import_component_type",
        "import_serializable_type",
        "include_header",
        "import_enum_type",
        "import_array_type",
        "import_topology",
    ]:
        dependencies.update(read_fprime_import(import_type, root))
    # Other items based on what tags are declared here specifically
    for extra in BASIC_DEPENDENCIES.keys():
        for cell in root.findall(".//{0}".format(extra)):
            dependencies.update(BASIC_DEPENDENCIES[extra])
            kind = cell.attrib.get("kind", None)
            if kind in KIND_DEPENDENCIES:
                dependencies.update(KIND_DEPENDENCIES[kind])
    # Recurse imports for other importables
    for recurse_ai in root.findall("import_dictionary"):
        tmptree = xml.etree.ElementTree.parse(
            os.path.join(import_base, recurse_ai.text)
        )
        newroot = tmptree.getroot()
        dependencies.update(read_xml_file(newroot, import_base))
    return dependencies


if __name__ == "__main__":
    main()
