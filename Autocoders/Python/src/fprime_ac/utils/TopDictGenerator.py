from fprime_ac.parsers import XmlArrayParser, XmlEnumParser, XmlSerializeParser
from fprime_ac.utils import DictTypeConverter
from fprime_ac.utils.buildroot import search_for_file
from lxml import etree


class TopDictGenerator:
    def __init__(self, parsed_xml_dict, log):
        self.__log = log
        self.__parsed_xml_dict = parsed_xml_dict

        # create a new XML tree for dictionary
        self.__enum_list = etree.Element("enums")
        self.__serializable_list = etree.Element("serializables")
        self.__array_list = etree.Element("arrays")
        self.__command_list = etree.Element("commands")
        self.__event_list = etree.Element("events")
        self.__telemetry_list = etree.Element("channels")
        self.__parameter_list = etree.Element("parameters")

    def set_current_comp(self, comp):
        self.__comp_type = comp.get_type()
        self.__comp_name = comp.get_name()
        self.__comp_id = int(comp.get_base_id(), 0)

    def check_for_enum_xml(self):
        enum_file_list = self.__parsed_xml_dict[self.__comp_type].get_enum_type_files()
        if enum_file_list != []:
            self.__check_enum_files(enum_file_list)

    def check_for_serial_xml(self):
        serializable_file_list = self.__parsed_xml_dict[
            self.__comp_type
        ].get_serializable_type_files()
        if serializable_file_list is not None:
            for serializable_file in serializable_file_list:
                serializable_file = search_for_file("Serializable", serializable_file)
                serializable_model = XmlSerializeParser.XmlSerializeParser(
                    serializable_file
                )

                # check for included enum XML in included serializable XML
                if len(serializable_model.get_include_enums()) != 0:
                    enum_file_list = serializable_model.get_include_enums()
                    self.__check_enum_files(enum_file_list)

                serializable_elem = etree.Element("serializable")
                serializable_type = (
                    serializable_model.get_namespace()
                    + "::"
                    + serializable_model.get_name()
                )
                serializable_elem.attrib["type"] = serializable_type
                members_elem = etree.Element("members")
                for (
                    member_name,
                    member_type,
                    member_array_size,
                    member_size,
                    member_format_specifier,
                    member_comment,
                    member_default,
                ) in serializable_model.get_members():
                    member_elem = etree.Element("member")
                    member_elem.attrib["name"] = member_name
                    member_elem.attrib["format_specifier"] = member_format_specifier
                    if member_comment is not None:
                        member_elem.attrib["description"] = member_comment
                    if member_default is not None:
                        member_elem.attrib["default"] = member_default
                    if isinstance(member_type, tuple):
                        type_name = "{}::{}::{}".format(
                            serializable_type,
                            member_name,
                            member_type[0][1],
                        )
                        enum_elem = self.__extract_enum_elem(type_name, member_type[1])
                        self.__enum_list.append(enum_elem)
                    else:
                        type_name = member_type
                        if member_type == "string":
                            member_elem.attrib["len"] = member_size
                    member_elem.attrib["type"] = type_name
                    members_elem.append(member_elem)
                serializable_elem.append(members_elem)

                dup = False
                for ser in self.__serializable_list:
                    if ser.attrib["type"] == serializable_elem.attrib["type"]:
                        dup = True
                if not dup:
                    self.__serializable_list.append(serializable_elem)

    def __check_enum_files(self, enum_file_list):
        for enum_file in enum_file_list:
            enum_file = search_for_file("Enum", enum_file)
            enum_model = XmlEnumParser.XmlEnumParser(enum_file)
            enum_elem = etree.Element("enum")
            enum_type = enum_model.get_namespace() + "::" + enum_model.get_name()
            enum_elem.attrib["type"] = enum_type
            enum_value = 0
            for (
                member_name,
                member_value,
                member_comment,
            ) in enum_model.get_items():
                enum_mem = etree.Element("item")
                enum_mem.attrib["name"] = member_name
                # keep track of incrementing enum value
                if member_value is not None:
                    enum_value = int(member_value)

                enum_mem.attrib["value"] = "%d" % enum_value
                enum_value = enum_value + 1
                if member_comment is not None:
                    enum_mem.attrib["description"] = member_comment
                enum_elem.append(enum_mem)
                self.__enum_list.append(enum_elem)

    def check_for_commands(self):
        commands = self.__parsed_xml_dict[self.__comp_type].get_commands()
        if commands is not None:
            for command in commands:
                self.__log("Processing Command %s" % command.get_mnemonic())
                command_elem = etree.Element("command")
                command_elem.attrib["component"] = self.__comp_name
                command_elem.attrib["mnemonic"] = command.get_mnemonic()
                command_elem.attrib["opcode"] = "%s" % (
                    hex(int(command.get_opcodes()[0], base=0) + self.__comp_id)
                )
                if command.get_comment() is not None:
                    command_elem.attrib["description"] = command.get_comment()

                args_elem = etree.Element("args")
                for arg in command.get_args():
                    arg_elem = etree.Element("arg")
                    arg_elem.attrib["name"] = arg.get_name()
                    if arg.get_comment() is not None:
                        arg_elem.attrib["description"] = arg.get_comment()

                    arg_type = arg.get_type()
                    if isinstance(arg_type, tuple):
                        type_name = "{}::{}::{}".format(
                            self.__comp_type,
                            arg.get_name(),
                            arg_type[0][1],
                        )
                        enum_elem = self.__extract_enum_elem(type_name, arg_type[1])
                        self.__enum_list.append(enum_elem)
                    else:
                        type_name = arg_type
                        if arg_type == "string":
                            arg_elem.attrib["len"] = arg.get_size()
                    arg_elem.attrib["type"] = type_name
                    args_elem.append(arg_elem)
                command_elem.append(args_elem)
                self.__command_list.append(command_elem)

    def check_for_channels(self):
        channels = self.__parsed_xml_dict[self.__comp_type].get_channels()
        if channels is not None:
            for chan in channels:
                self.__log("Processing Channel %s" % chan.get_name())
                channel_elem = etree.Element("channel")
                channel_elem.attrib["component"] = self.__comp_name
                channel_elem.attrib["name"] = chan.get_name()
                channel_elem.attrib["id"] = "%s" % (
                    hex(int(chan.get_ids()[0], base=0) + self.__comp_id)
                )
                if chan.get_format_string() is not None:
                    channel_elem.attrib["format_string"] = chan.get_format_string()
                if chan.get_comment() is not None:
                    channel_elem.attrib["description"] = chan.get_comment()

                channel_elem.attrib["id"] = "%s" % (
                    hex(int(chan.get_ids()[0], base=0) + self.__comp_id)
                )
                channel_type = chan.get_type()
                if isinstance(channel_type, tuple):
                    type_name = "{}::{}::{}".format(
                        self.__comp_type,
                        chan.get_name(),
                        channel_type[0][1],
                    )
                    enum_elem = self.__extract_enum_elem(type_name, channel_type[1])
                    self.__enum_list.append(enum_elem)
                else:
                    type_name = channel_type
                    if channel_type == "string":
                        channel_elem.attrib["len"] = chan.get_size()
                (lr, lo, ly, hy, ho, hr) = chan.get_limits()
                if lr is not None:
                    channel_elem.attrib["low_red"] = lr
                if lo is not None:
                    channel_elem.attrib["low_orange"] = lo
                if ly is not None:
                    channel_elem.attrib["low_yellow"] = ly
                if hy is not None:
                    channel_elem.attrib["high_yellow"] = hy
                if ho is not None:
                    channel_elem.attrib["high_orange"] = ho
                if hr is not None:
                    channel_elem.attrib["high_red"] = hr

                channel_elem.attrib["type"] = type_name
                self.__telemetry_list.append(channel_elem)

    def check_for_events(self):
        events = self.__parsed_xml_dict[self.__comp_type].get_events()
        if events is not None:
            for event in events:
                self.__log("Processing Event %s" % event.get_name())
                event_elem = etree.Element("event")
                event_elem.attrib["component"] = self.__comp_name
                event_elem.attrib["name"] = event.get_name()
                event_elem.attrib["id"] = "%s" % (
                    hex(int(event.get_ids()[0], base=0) + self.__comp_id)
                )
                event_elem.attrib["severity"] = event.get_severity()
                format_string = event.get_format_string()
                if event.get_comment() is not None:
                    event_elem.attrib["description"] = event.get_comment()

                args_elem = etree.Element("args")
                arg_num = 0
                for arg in event.get_args():
                    arg_elem = etree.Element("arg")
                    arg_elem.attrib["name"] = arg.get_name()
                    if arg.get_comment() is not None:
                        arg_elem.attrib["description"] = arg.get_comment()

                    arg_type = arg.get_type()
                    if isinstance(arg_type, tuple):
                        type_name = "{}::{}::{}".format(
                            self.__comp_type,
                            arg.get_name(),
                            arg_type[0][1],
                        )
                        enum_elem = self.__extract_enum_elem(type_name, arg_type[1])
                        self.__enum_list.append(enum_elem)
                        # replace enum format string %d with %s for ground system
                        format_string = (
                            DictTypeConverter.DictTypeConverter().format_replace(
                                format_string, arg_num, "d", "s"
                            )
                        )
                    else:
                        type_name = arg_type
                        if arg_type == "string":
                            arg_elem.attrib["len"] = arg.get_size()
                    arg_elem.attrib["type"] = type_name
                    args_elem.append(arg_elem)
                    arg_num += 1
                event_elem.attrib["format_string"] = format_string
                event_elem.append(args_elem)
                self.__event_list.append(event_elem)

    def __extract_enum_elem(self, type_name, member):
        enum_value = 0
        # Add enum entry
        enum_elem = etree.Element("enum")
        enum_elem.attrib["type"] = type_name
        # Add enum members
        for (membername, value, comment) in member:
            enum_mem = etree.Element("item")
            enum_mem.attrib["name"] = membername
            # keep track of incrementing enum value
            if value is not None:
                enum_value = int(value)

            enum_mem.attrib["value"] = "%d" % enum_value
            enum_value = enum_value + 1
            if comment is not None:
                enum_mem.attrib["description"] = comment
            enum_elem.append(enum_mem)
        return enum_elem

    def check_for_parameters(self):
        parameters = self.__parsed_xml_dict[self.__comp_type].get_parameters()
        if parameters is not None:
            for parameter in parameters:
                self.__log("Processing Parameter %s" % parameter.get_name())
                param_default = None
                command_elem_set = etree.Element("command")
                command_elem_set.attrib["component"] = self.__comp_name
                command_elem_set.attrib["mnemonic"] = parameter.get_name() + "_PRM_SET"
                command_elem_set.attrib["opcode"] = "%s" % (
                    hex(int(parameter.get_set_opcodes()[0], base=0) + self.__comp_id)
                )
                if parameter.get_comment() is not None:
                    command_elem_set.attrib["description"] = (
                        parameter.get_comment() + " parameter set"
                    )
                else:
                    command_elem_set.attrib["description"] = (
                        parameter.get_name() + " parameter set"
                    )

                args_elem = etree.Element("args")
                arg_elem = etree.Element("arg")
                arg_elem.attrib["name"] = "val"
                arg_type = parameter.get_type()
                if isinstance(arg_type, tuple):
                    enum_value = 0
                    type_name = "{}::{}::{}".format(
                        self.__comp_type,
                        parameter.get_name(),
                        arg_type[0][1],
                    )
                    # Add enum entry
                    enum_elem = etree.Element("enum")
                    enum_elem.attrib["type"] = type_name
                    # Add enum members
                    for (membername, value, comment) in arg_type[1]:
                        enum_mem = etree.Element("item")
                        enum_mem.attrib["name"] = membername
                        # keep track of incrementing enum value
                        if value is not None:
                            enum_value = int(value)

                        enum_mem.attrib["value"] = "%d" % enum_value
                        enum_value = enum_value + 1
                        if comment is not None:
                            enum_mem.attrib["description"] = comment
                        enum_elem.append(enum_mem)
                        # assign default to be first enum member
                        if param_default is None:
                            param_default = membername
                    self.__enum_list.append(enum_elem)
                else:
                    type_name = arg_type
                    if arg_type == "string":
                        arg_elem.attrib["len"] = parameter.get_size()
                    else:
                        param_default = "0"
                arg_elem.attrib["type"] = type_name
                args_elem.append(arg_elem)
                command_elem_set.append(args_elem)
                self.__command_list.append(command_elem_set)

                command_elem_save = etree.Element("command")
                command_elem_save.attrib["component"] = self.__comp_name
                command_elem_save.attrib["mnemonic"] = (
                    parameter.get_name() + "_PRM_SAVE"
                )
                command_elem_save.attrib["opcode"] = "%s" % (
                    hex(int(parameter.get_save_opcodes()[0], base=0) + self.__comp_id)
                )
                if parameter.get_comment() is not None:
                    command_elem_save.attrib["description"] = (
                        parameter.get_comment() + " parameter save"
                    )
                else:
                    command_elem_save.attrib["description"] = (
                        parameter.get_name() + " parameter save"
                    )

                self.__command_list.append(command_elem_save)

                param_elem = etree.Element("parameter")
                param_elem.attrib["component"] = self.__comp_name
                param_elem.attrib["name"] = parameter.get_name()
                param_elem.attrib["id"] = "%s" % (
                    hex(int(parameter.get_ids()[0], base=0) + self.__comp_id)
                )
                if parameter.get_default() is not None:
                    param_default = parameter.get_default()
                param_elem.attrib["default"] = param_default

                self.__parameter_list.append(param_elem)

    def check_for_arrays(self):
        array_file_list = self.__parsed_xml_dict[
            self.__comp_type
        ].get_array_type_files()
        if array_file_list is not None:
            for array_file in array_file_list:
                array_file = search_for_file("Array", array_file)
                array_model = XmlArrayParser.XmlArrayParser(array_file)

                array_elem = etree.Element("array")
                array_name = array_model.get_namespace() + "::" + array_model.get_name()
                array_elem.attrib["name"] = array_name

                array_type = array_model.get_type()
                array_elem.attrib["type"] = array_type

                array_type_id = array_model.get_type_id()
                array_elem.attrib["type_id"] = array_type_id

                array_size = array_model.get_size()
                array_elem.attrib["size"] = array_size

                array_format = array_model.get_format()
                array_elem.attrib["format"] = array_format

                members_elem = etree.Element("defaults")
                for d_val in array_model.get_default():
                    member_elem = etree.Element("default")
                    member_elem.attrib["value"] = d_val
                    members_elem.append(member_elem)

                array_elem.append(members_elem)

                dup = False
                for arr in self.__array_list:
                    if arr.attrib["name"] == array_elem.attrib["name"]:
                        dup = True
                if not dup:
                    self.__array_list.append(array_elem)

    def remove_duplicate_enums(self):
        temp_enum_list = []
        for enum_elem in self.__enum_list:
            temp_enum_list.append(enum_elem)
        for enum_elem in temp_enum_list:
            should_remove = False
            for temp_enum in self.__enum_list:
                # Skip over comparisons between same exact element
                if id(enum_elem) == id(temp_enum):
                    continue

                # Check all attributes
                if temp_enum.attrib["type"] == enum_elem.attrib["type"]:
                    should_remove = True
                if (
                    not len(temp_enum.getchildren()) == len(enum_elem.getchildren())
                    and should_remove
                ):
                    should_remove = False
                children1 = temp_enum.getchildren()
                children2 = enum_elem.getchildren()
                if children1 and children2:
                    i = 0
                    while i < len(children1) and i < len(children2):
                        if (
                            not children1[i].attrib["name"]
                            == children2[i].attrib["name"]
                            and should_remove
                        ):
                            should_remove = False
                        i += 1
                if should_remove:
                    break
            if should_remove:
                self.__enum_list.remove(enum_elem)

    def get_enum_list(self):
        return self.__enum_list

    def get_serializable_list(self):
        return self.__serializable_list

    def get_array_list(self):
        return self.__array_list

    def get_command_list(self):
        return self.__command_list

    def get_event_list(self):
        return self.__event_list

    def get_telemetry_list(self):
        return self.__telemetry_list

    def get_parameter_list(self):
        return self.__parameter_list
