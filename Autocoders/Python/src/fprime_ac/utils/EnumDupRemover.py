
def remove_duplicates(enum_list):
    temp_enum_list = []
    for enum_elem in enum_list:
        temp_enum_list.append(enum_elem)
    for enum_elem in temp_enum_list:
        should_remove = False
        for temp_enum in enum_list:
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
                        not children1[i].attrib["name"] == children2[i].attrib["name"]
                        and should_remove
                    ):
                        should_remove = False
                    i += 1
            if should_remove:
                break
        if should_remove:
            enum_list.remove(enum_elem)
