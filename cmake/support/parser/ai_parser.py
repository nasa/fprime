#!/usr/bin/env python
"""
Utils:

Utility functions used to support the quick-parsing of the Ai.xml files used in the CMake system. This helps detect
automatic dependencies, and registering into the build system.

@author mstarch
"""
import os
import sys
import xml.etree.ElementTree

# Dependencies required by basic types
BASIC_DEPENDENCIES = {
    "component":    ["Fw_Cfg", "Fw_Types", "Fw_Comp"],
    "port":         ["Fw_Cfg", "Fw_Types", "Fw_Port"],
    "interface":    ["Fw_Cfg", "Fw_Types", "Fw_Port"],
    "assembly":     ["Fw_Cfg", "Fw_Types"],
    "serializable": ["Fw_Cfg", "Fw_Types"],
    "commands":     ["Fw_Cfg", "Fw_Types", "Fw_Com", "Fw_Cmd"],
    "events":       ["Fw_Cfg", "Fw_Types", "Fw_Com", "Fw_Log"],
    "telemetry":    ["Fw_Cfg", "Fw_Types", "Fw_Com", "Fw_Tlm"],
    "parameters":   ["Fw_Cfg", "Fw_Types", "Fw_Prm"],
    "internal_interfaces": []
}
# Component type importations
COMPONENT_TYPE_DEPENDENCIES = {
    "passive": [],
    "queued": ["Os"],
    "active": ["Os"]
}


def main():
    """
    Read arguments and run!
    """
    if len(sys.argv) != 4:
        print("[ERROR] Failed to run, need 4 arguments. Usage:\n\t{0} <input_Ai.xml> <library name> <import base>"
              .format(sys.argv[0]), file=sys.stderr)
        sys.exit(1)
    elif not os.path.exists(sys.argv[1]):
        print("[ERROR] {0} does not exist. Usage:\n\t{1} <input_Ai.xml> <library name> <import base>"
              .format(sys.argv[1], sys.argv[0]), file=sys.stderr)
        sys.exit(2)
    elif not os.path.isdir(sys.argv[3]):
        print("[ERROR] {0} does not exist. Usage:\n\t{1} <input_Ai.xml> <library name> <import base>"
              .format(sys.argv[3], sys.argv[0]), file=sys.stderr)
        sys.exit(3)
    print_fprime_dependencies(*sys.argv[1:])

def print_fprime_dependencies(input_file, current_library, import_base):
    """
    Print F prime dependencies. These should be printed w/o a newlines.
    :param input_file: input XML file to parse
    :param current_library: current library to strip from dependencies
    :param import_base: base of imported dictionaries, used for finding and opening them
    """
    dependencies = read_xml_file(input_file, import_base)
    if current_library in dependencies:
        dependencies.remove(current_library)
    # Write out CMake style list
    sys.stdout.write(";".join(dependencies))
    sys.stdout.flush()


def read_fprime_import(import_type, root):
    """
    Read an FPrime dependency port/type. These are typically ports, serializables, and components.
    :param import_type: "import_port_type", "import_component_type", "import_serializable_type"
    :param root: root XML to parse
    :return: dependency mined from this item
    """
    deps = set()
    for imp in root.findall(import_type):
        imp_lib = os.path.dirname(imp.text).replace('/', '_')
        deps.add(imp_lib)
        # Components have extra attributes based on their type
        if "import_component_type" == import_type:
            kind = root.attrib.get("kind", None)
            if kind in COMPONENT_TYPE_DEPENDENCIES:
                deps.update(COMPONENT_TYPE_DEPENDENCIES[kind])
            for extra in ["commands", "events", "telemetry"]:
                if root.findall(extra):
                    deps.update(BASIC_DEPENDENCIES[extra])

    return deps


def read_xml_file(input_file, import_base):
    """
    Reads and Ai.xml file, detects the type, and then pulls in the dependencies. Recursively looks for dependencies in
    import_dictionaries by inferring dependencies for each type of dictionary.
    :param input_file: input XML file to parse
    :param import_base: base of imported dictionaries, used for finding and opening them
    :return: dependencies set
    """
    # Read the file into the XML libraries
    tree = xml.etree.ElementTree.parse(input_file)
    root = tree.getroot()
    ai_type = root.tag
    dependencies = set(BASIC_DEPENDENCIES[ai_type])
    # Import component/serialzable/port types
    for import_type in ["import_port_type", "import_component_type", "import_serializable_type", "include_header"]:
        dependencies.update(read_fprime_import(import_type, root))
    # Recurse imports for other importables
    for recurse_ai in root.findall("import_dictionary"):
        read_xml_file(os.path.join(import_base, recurse_ai.text), import_base)
    return dependencies


if __name__ == "__main__":
    main()
