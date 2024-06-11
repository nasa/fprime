import os.path
import pickle
from optparse import OptionParser

from lxml import etree

"""
This script is meant to serve as "sanity" checker for the MagicDraw generated topology XML files
"""


def setup_opt_parse():
    usage = "usage: %prog [options] [topology_filename]"
    parser = OptionParser(usage)

    parser.add_option(
        "-d",
        "--xml_diff",
        dest="xml_diff_flag",
        help="Performs a diff on the specified XML files.",
        action="store_true",
        default=False,
    )

    parser.add_option(
        "-c",
        "--command_index_print",
        dest="command_index_print_flag",
        help="Prints a table of the command indexes.",
        action="store_true",
        default=False,
    )

    return parser


def validate_xml(xml_list):
    """
    Iterates through XML list, shows an error if an XML file is not an XML file or does not exist.

    A list of valid XML files is returned (Not valid files are thrown out)
    """

    out_list = []
    for xml_path in xml_list:
        if os.path.isfile(xml_path):
            out_list.append(xml_path)
        else:
            print(
                "WARNING: XML {} is not a valid file. Rejecting file.".format(xml_path)
            )

    return out_list


def recursive_xml_parse(tree_obj):
    """
    returns a list of items
    [tagName , [(argKey:argVal)] , [" " or [tagName , [] , []] ] ]
    """
    out_obj = [tree_obj.tag, [], []]

    for att in tree_obj.attrib:
        out_obj[1].append((att, tree_obj.attrib[att]))

    internal_text = tree_obj.text
    if internal_text is not None:
        internal_text = internal_text.strip()
        if internal_text != "":
            out_obj[2].append(internal_text)

    for internal_item in tree_obj:
        out_obj[2].append(recursive_xml_parse(internal_item))

    return out_obj


def tag_object_to_string(tag_obj):
    out = "<{}{}>".format(
        tag_obj[0], "".join(" " + x[0] + "=" + '"' + x[1] + '"' for x in tag_obj[1])
    )

    final_line_break = ""
    for internal_item in tag_obj[2]:
        if type(internal_item) == str:
            out += internal_item
        else:
            out += "\n\t" + tag_object_to_string(internal_item).replace("\n", "\n\t")
            final_line_break = "\n"

    out += final_line_break + "</" + tag_obj[0] + ">"
    return out


def diff_files(xml_list):
    """
    Finds the difference between topology XML files, ignoring  ordering and names in "connection" tags

    Iterate through root tag elements
    Create a dictionary with file_dict[tag] = [list of tag objects]
    """
    if len(xml_list) < 2:
        print("Less than two XML files were specified. Exiting.")
        return

    master_tag_dict = {}
    for xml_path in xml_list:
        # Create etree object
        fd = open(xml_path, "r")
        xml_parser = etree.XMLParser(remove_comments=True)
        element_tree = etree.parse(fd, parser=xml_parser)
        fd.close()

        # Internal Parsing
        xml_dict = recursive_xml_parse(element_tree.getroot())

        for tag_obj in xml_dict[2]:
            if tag_obj[0] == "connection":
                tag_obj[1] = []

            pickled_obj = pickle.dumps(tag_obj)

            if pickled_obj not in master_tag_dict:
                # del master_tag_dict[pickled_obj]
                master_tag_dict[pickled_obj] = []

            master_tag_dict[pickled_obj].append(xml_path)

    # Separate by XML path
    tag_to_object = {}  # tag_to_object[xml_path] = [obj]
    for pickled_obj in master_tag_dict:
        if len(master_tag_dict[pickled_obj]) == 1:
            if master_tag_dict[pickled_obj][0] not in tag_to_object:
                tag_to_object[master_tag_dict[pickled_obj][0]] = []
            tag_to_object[master_tag_dict[pickled_obj][0]].append(pickled_obj)

    for xml_path in tag_to_object:
        print(xml_path + "\n")
        # sort pickled obj lists
        tag_to_object[xml_path].sort()
        for pickled_obj in tag_to_object[xml_path]:
            tag_obj = pickle.loads(pickled_obj)
            print(tag_object_to_string(tag_obj))
            print("\n")
        print("\n")


def command_index_print(xml_list):
    for xml_path in xml_list:
        # Create etree object
        fd = open(xml_path, "r")
        xml_parser = etree.XMLParser(remove_comments=True)
        element_tree = etree.parse(fd, parser=xml_parser)
        fd.close()

        # Internal Parsing
        xml_dict = recursive_xml_parse(element_tree.getroot())

        connection_information = []  # [{target/source: {attribKey:attribVal}}]

        # Gather connection data
        for tag_obj in xml_dict[2]:
            if tag_obj[0] == "connection":
                in_dict = {"SOURCE": None, "TARGET": None}
                for conn_obj in tag_obj[2]:
                    if type(conn_obj) != str:
                        attrib_dict = {}
                        for attrib in conn_obj[1]:
                            attrib_dict[attrib[0].upper()] = attrib[1].upper()
                        in_dict[conn_obj[0].upper()] = attrib_dict
                connection_information.append(in_dict)

        component_to_cmd_info = (
            {}
        )  # component_to_cmd_info[componentName] = {cmdIndex:index , cmdRegIndex:index}

        # Create dict mapping components to their mult values on the cmdDisp
        for conn_info in connection_information:
            source_dict = conn_info["SOURCE"]
            target_dict = conn_info["TARGET"]

            source_comp_name = source_dict["COMPONENT"]
            target_comp_name = target_dict["COMPONENT"]

            if source_comp_name not in component_to_cmd_info:
                component_to_cmd_info[source_comp_name] = {
                    "cmdIndex": None,
                    "cmdRegIndex": None,
                }
            if target_comp_name not in component_to_cmd_info:
                component_to_cmd_info[target_comp_name] = {
                    "cmdIndex": None,
                    "cmdRegIndex": None,
                }

            if source_dict["TYPE"] == "CMD":
                component_to_cmd_info[target_comp_name]["cmdIndex"] = source_dict["NUM"]

            if target_dict["TYPE"] == "CMDREG":
                component_to_cmd_info[source_comp_name]["cmdRegIndex"] = target_dict[
                    "NUM"
                ]

        # sort by num
        sorted_list = []
        for comp_info in component_to_cmd_info:
            sorted_list.append([comp_info, component_to_cmd_info[comp_info]])

        sorted_list.sort(
            key=lambda x: int(float(x[1]["cmdIndex"]))
            if x[1]["cmdIndex"] is not None
            else -1
        )

        # Print table
        print(xml_path + "\n")
        header_list = [
            "\t\tComponent Name\t\t",
            "Command Index",
            "Command Registration Index",
        ]
        len_list = [len(x) for x in header_list]

        print("| " + " | ".join(x for x in header_list) + " |")
        for comp_info in sorted_list:
            print_list = [
                comp_info[0],
                comp_info[1]["cmdIndex"],
                comp_info[1]["cmdRegIndex"],
            ]
            i = 0
            row_string = ""
            while i != len(header_list):
                if i != 0:
                    row_string += " | "
                format_string = "{0:^" + str(len_list[i]) + "}"
                row_string += format_string.format(print_list[i])
                i += 1
            print("| " + row_string + " |")

        print("\n\n")


def main():
    parser = setup_opt_parse()
    (opt, args) = parser.parse_args()

    xml_list = validate_xml(args)

    if opt.command_index_print_flag:
        command_index_print(xml_list)

    if opt.xml_diff_flag:
        diff_files(xml_list)


if __name__ == "__main__":
    main()
