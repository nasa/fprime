import os
import sys

import pytest
from lxml import etree

"""
To add tests, go down to the setup function.
"""


class schema_test:
    """
    schema_test is a base object for conducting tests on schemas.
    """

    def __init__(self, schema_name, schema_path):
        """
        Starts object.

        schema_name - Name to refer to schema by
        schema_path - Path to retrieve RelaxNG schema
        """
        self.__schema_name = schema_name
        self.__schema_path = schema_path

        self.__test_set_list = []  # Creates an empty list of test sets

        self.__validate_and_compile()

    def __validate_and_compile(self):
        """
        Validates and compiles the schemas specified on instantiation.
        """
        self.__validate_file(self.__schema_path, "RNG")

        with open(self.__schema_path) as relax_file_handler:

            # Parse schema file
            relax_parsed = etree.parse(relax_file_handler)

            # Compile schema file
            self.__compiled = etree.RelaxNG(relax_parsed)

    def __validate_file(self, file_name, extension):
        """
        Ensures file exists and has the proper extension.
        """
        if not os.path.exists(file_name):
            raise Exception("File does not exist - {}.".format(file_name))

        if not file_name.upper().endswith(f".{extension.upper()}"):
            raise Exception(
                "File does not end with proper extension {} - {}".format(
                    extension, file_name
                )
            )

        return True

    def __get_parsed_relaxng(self, file_path):
        """
        Returns root tag assuming file path is correct
        """

        with open(file_path) as handler:

            # Parse schema file
            parsed = etree.parse(handler)

        return parsed

    def add_test(self, test_name, xml_path, error_class, parsed_xml=None):
        """
        Add test case to object.

        test_name - Way of identifying the test
        xml_path - Path to xml test file
        error_class - What sort of error that is going to be thrown. If error_class is None, it is assumed that the test will pass without raising exceptions.
        parsed_xml - Add the etree of the XML if available.
        """
        test_set = (test_name, xml_path, error_class, parsed_xml)

        self.__test_set_list.append(test_set)

    def parse_and_add_directory(self, list_of_root_tags, directory):
        """
        Parses through directory and all subdirectories and adds tests to object test lists

        list_of_root_tags - list of root tags to check for
        directory - directory to look in
        """

        # Check if directory exists and list_of_root_tags isn't empty

        if len(list_of_root_tags) == 0:
            raise Exception(
                "{} : List of root tags empty in parse_and_add_directory!".format(
                    self.__schema_name
                )
            )

        if not os.path.isdir(directory):
            raise Exception(
                "{} : Directory {} does not exist in parse_and_add_directory!".format(
                    self.__schema_name, directory
                )
            )

        for subdir, dirs, files in os.walk(directory):
            for file in files:
                if file.upper().endswith(".XML"):
                    try:
                        new_path = os.path.join(subdir, file)
                        parsed = self.__get_parsed_relaxng(new_path)
                        root_tag = parsed.getroot().tag
                        if root_tag in list_of_root_tags:
                            self.add_test(
                                f"Path Added: {file}", new_path, None, parsed_xml=parsed
                            )
                    except:
                        pass

    def run_all_tests(self):
        """
        Runs all the tests consecutively.
        """
        for index in range(len(self.__test_set_list)):
            self.run_test(index)

    def get_test_amount(self):
        """
        Returns the amount of tests in the object.
        """

        return len(self.__test_set_list)

    def run_test(self, index):
        """
        Runs test of index
        """
        if index >= len(self.__test_set_list):
            raise Exception("Illegal index was accessed")

        test_set = self.__test_set_list[index]

        xml_parsed = test_set[3]

        if not xml_parsed:
            self.__validate_file(test_set[1], "XML")
            with open(test_set[1]) as xml_file_handler:
                xml_parsed = etree.parse(xml_file_handler)

        if test_set[2]:
            with pytest.raises(test_set[2]) as excinfo:
                self.__compiled.assertValid(xml_parsed)
                if excinfo:
                    print(
                        "Schema "
                        + self.__schema_name
                        + " failed validating the current file."
                    )
                    print("\n")
                    print(
                        test_set[0]
                        + " raised the wrong exception or passed, when fail was expected (Exception "
                        + str(test_set[2])
                        + "."
                    )
                    print(f"File path - {test_set[1]}")
                    print(excinfo)
                    print("\n")
                    sys.exit(1)

        else:
            try:
                self.__compiled.assertValid(xml_parsed)
            except:
                print(
                    "Schema "
                    + self.__schema_name
                    + " failed validating the current file."
                )
                print("\n")
                print(f"{test_set[0]} raised an exception but was supposed to pass.")
                print(f"File path - {test_set[1]}")

                print("\n")
                raise

    def print_header(self):
        """
        Prints a header string for a schema_test object.
        """
        print("\nTesting {} - {}\n".format(self.__schema_name, self.__schema_path))


def setup():
    """
    Sets up and returns test_list, which is a set of schema_test objects.
    """
    test_list = []

    # Create schema object
    topology_test = schema_test("Topology", "ISF/topology_schema.rng")
    component_test = schema_test("Component", "ISF/component_schema.rng")
    command_test = schema_test("Command", "ISF/command_schema.rng")
    parameter_test = schema_test("Parameter", "ISF/parameters_schema.rng")
    channel_test = schema_test("Channel", "ISF/channel_schema.rng")
    interface_test = schema_test("Interface", "ISF/interface_schema.rng")
    serializable_test = schema_test("Serializable", "ISF/serializable_schema.rng")
    event_test = schema_test("Event", "ISF/event_schema.rng")

    # Declare schema tests

    channel_test.add_test(
        "All working", "sample_XML_files/channel/allWorking.xml", None
    )
    channel_test.add_test(
        "High Orange string instead of number",
        "sample_XML_files/channel/colorString.xml",
        AssertionError,
    )
    channel_test.add_test(
        "Missing comments",
        "sample_XML_files/channel/missingComments.xml",
        AssertionError,
    )
    channel_test.add_test(
        "Missing data type",
        "sample_XML_files/channel/missingDataType.xml",
        AssertionError,
    )
    channel_test.add_test(
        "Missing enum", "sample_XML_files/channel/missingEnum.xml", AssertionError
    )

    command_test.add_test(
        "All working", "sample_XML_files/command/allWorking.xml", None
    )
    command_test.add_test(
        "Command size is negative",
        "sample_XML_files/command/negativeCommandSize.xml",
        AssertionError,
    )
    command_test.add_test(
        "Enum missing when type ENUM is specified",
        "sample_XML_files/command/missingEnum.xml",
        AssertionError,
    )
    command_test.add_test(
        "Kind neither sync nor async",
        "sample_XML_files/command/kindMixed.xml",
        AssertionError,
    )
    command_test.add_test(
        "String size not defined",
        "sample_XML_files/command/noStringSize.xml",
        AssertionError,
    )

    component_test.add_test(
        "Base all working", "sample_XML_files/component/baseAllWorking.xml", None
    )
    component_test.add_test(
        "Complex all working", "sample_XML_files/component/complexAllWorking.xml", None
    )
    component_test.add_test(
        "No ports", "sample_XML_files/component/noPorts.xml", AssertionError
    )
    component_test.add_test(
        "<Interface> tag instead of <internal_interface> tag",
        "sample_XML_files/component/interfaceOnly.xml",
        AssertionError,
    )

    event_test.add_test("All working", "sample_XML_files/event/allWorking.xml", None)

    event_test.add_test(
        "Event throttle negative",
        "sample_XML_files/event/negativeThrottle.xml",
        AssertionError,
    )
    event_test.add_test(
        "Format string missing",
        "sample_XML_files/event/missingFormatString.xml",
        AssertionError,
    )
    event_test.add_test(
        "Severity not valid",
        "sample_XML_files/event/unknownSeverity.xml",
        AssertionError,
    )

    interface_test.add_test(
        "All working", "sample_XML_files/interface/allWorking.xml", None
    )
    interface_test.add_test(
        "Multiple return tags",
        "sample_XML_files/interface/multipleReturns.xml",
        AssertionError,
    )
    interface_test.add_test(
        "No ENUM in return type ENUM",
        "sample_XML_files/interface/noEnumInReturn.xml",
        AssertionError,
    )
    interface_test.add_test(
        "No return tags", "sample_XML_files/interface/noReturns.xml", None
    )
    interface_test.add_test(
        "Priority attribute is a string",
        "sample_XML_files/interface/stringPriority.xml",
        AssertionError,
    )

    parameter_test.add_test(
        "All working", "sample_XML_files/parameter/allWorking.xml", None
    )
    parameter_test.add_test(
        "Float in native integer type default attribute",
        "sample_XML_files/parameter/floatInInt.xml",
        AssertionError,
    )
    parameter_test.add_test(
        "Negative in unsigned 8 bit default attribute",
        "sample_XML_files/parameter/negativeInUnsigned.xml",
        AssertionError,
    )
    parameter_test.add_test(
        "No size attribute for string type attribute",
        "sample_XML_files/parameter/noStringSize.xml",
        AssertionError,
    )
    parameter_test.add_test(
        "String in 32 bit float attribute",
        "sample_XML_files/parameter/stringInFloat.xml",
        AssertionError,
    )

    serializable_test.add_test(
        "All working", "sample_XML_files/serializable/allWorking.xml", None
    )
    serializable_test.add_test(
        "Multiple members tags",
        "sample_XML_files/serializable/multipleMembers.xml",
        AssertionError,
    )
    serializable_test.add_test(
        "No members tag", "sample_XML_files/serializable/noMembers.xml", AssertionError
    )
    serializable_test.add_test(
        "No name in root", "sample_XML_files/serializable/noName.xml", AssertionError
    )
    serializable_test.add_test(
        "No type in member", "sample_XML_files/serializable/noType.xml", AssertionError
    )

    topology_test.add_test(
        "All working", "sample_XML_files/topology/allWorking.xml", None
    )
    topology_test.add_test(
        "Negative connection number.",
        "sample_XML_files/topology/negativeConnectionNumber.xml",
        AssertionError,
    )
    topology_test.add_test(
        "No Imports", "sample_XML_files/topology/noImports.xml", None
    )
    topology_test.add_test(
        "No connections made",
        "sample_XML_files/topology/noConnections.xml",
        AssertionError,
    )
    topology_test.add_test(
        "No instances", "sample_XML_files/topology/noInstances.xml", AssertionError
    )
    topology_test.add_test(
        "No types", "sample_XML_files/topology/noTypes.xml", AssertionError
    )

    # Add more schema tests

    channel_test.parse_and_add_directory(["telemetry"], "../test")
    command_test.parse_and_add_directory(["commands"], "../test")
    component_test.parse_and_add_directory(["component"], "../test")
    event_test.parse_and_add_directory(["events"], "../test")
    interface_test.parse_and_add_directory(["interface", "port"], "../test")
    parameter_test.parse_and_add_directory(["parameters"], "../test")
    serializable_test.parse_and_add_directory(["serializable"], "../test")
    topology_test.parse_and_add_directory(["deployment", "assembly"], "../test")

    # Add schemas to test_list

    test_list.extend(
        (
            topology_test,
            component_test,
            command_test,
            parameter_test,
            channel_test,
            interface_test,
            serializable_test,
            event_test,
        )
    )

    return test_list


def get_test_list():
    test_list = setup()
    out = []
    for test_obj in test_list:
        tl = test_obj.get_test_amount()
        for ti in range(tl):
            out.append((test_obj.run_test, ti))
    return out


@pytest.fixture(scope="module", params=get_test_list())
def current_test(request):
    return request.param


def tests_all(current_test):
    func = current_test[0]
    index = current_test[1]
    func(index)


if __name__ == "__main__":
    test_list = setup()
    for test in test_list:
        test.print_header()
        test.run_all_tests()
