"""
jsonable.py:

Folder with helper methods that allow for seamless conversion of F prime types to JSON types. This allows us to produce
data in JSON format with ease, assuming that the correct encoder is registered.

Note: JSON types must use only the following data types

 1. booleans
 2. numbers
 3. strings
 4. lists
 5. anonymous objects (dictionaries)

@author mstarch
"""


def fprime_to_jsonable(obj):
    """
    Takes an F prime object and converts it to a jsonable type.

    :param obj: object to convert
    :return: object in jsonable format (can call json.dump(obj))
    """
    # Otherwise try and scrape all "get_" getters in a smart way
    anonymous = {}
    getters = [attr for attr in dir(obj) if attr.startswith("get_")]
    for getter in getters:
        # Call the get_ functions, and call all non-static methods
        try:
            func = getattr(obj, getter)
            item = func()
            # If there is a property named "args" it needs to be handled specifically unless an incoming command
            if (
                getter == "get_args"
                and not "fprime_gds.common.data_types.cmd_data.CmdData"
                in str(type(obj))
            ):
                args = []
                for arg_spec in item:
                    arg_dict = {
                        "name": arg_spec[0],
                        "description": arg_spec[1],
                        "value": arg_spec[2].val,
                        "type": str(arg_spec[2]),
                    }
                    if arg_dict["type"] == "Enum":
                        arg_dict["possible"] = arg_spec[2].keys()
                    args.append(arg_dict)
                # Fill in our special handling
                item = args
            anonymous[getter.replace("get_", "")] = item
        except TypeError:
            continue
    return anonymous
