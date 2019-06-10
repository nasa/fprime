####
# json.py:
#
# Encodes GDS objects as JSON.
####
import collections
import enum

import flask.json

import fprime_gds.common.templates.ch_template
import fprime_gds.common.templates.cmd_template
import fprime_gds.common.templates.event_template

import fprime.common.models.serialize.type_base
import fprime.common.models.serialize.serializable_type

class GDSJsonEncoder(flask.json.JSONEncoder):
    """
    Custom class used to handle GDS object to JSON
    """
    def default(self, obj):
        """
        Override the default JSON encoder to pull out a dictionary for our handled types for encoding with the default
        encoder built into flask
        :param obj: obj to encode
        :return: JSON
        """
        # If it is an object that is ours, handle it
        if isinstance(obj, (fprime_gds.common.templates.ch_template.ChTemplate,
                            fprime_gds.common.templates.cmd_template.CmdTemplate,
                            fprime_gds.common.templates.event_template.EventTemplate)):
            return self.get_all(obj)
        # Recursively handle serialize types
        elif isinstance(obj, fprime.common.models.serialize.serializable_type.SerializableType):
            members = {}
            for member in obj.mem_list:
                members[member[0]] = {"data": member[1], "format": member[2], "description": member[3]}
            return members
        # Check for f prime types
        elif isinstance(obj, fprime.common.models.serialize.type_base.BaseType):
            return {"value": obj.val, "type": str(obj)}
        # Dictionaries are "iterable", must handle them first
        elif isinstance(obj, collections.Mapping):
            return flask.json.JSONEncoder.default(self, obj)
        # Handle null objects
        elif obj is None:
            return "unset"
        # Enumerations
        elif isinstance(obj, enum.Enum):
            return str(obj)
        # Lists and iterables
        try:
            return list(iter(obj))
        except TypeError:
            pass
        return flask.json.JSONEncoder.default(self, obj)

    def get_all(self, obj):
        """
        Any getters defined on the object are copied-over verbatim
        :param obj: object to harvest
        :return: new object filled with getters
        """
        out_dict = {}
        for attr in [attr for attr in dir(obj) if attr.startswith("get_")]:
            item = getattr(obj, attr)()
            # Special handling for arguments
            if attr == "get_args":
                args = []
                for arg_spec in item:
                    args.append({"name": arg_spec[0], "description": arg_spec[1],
                                 "value": arg_spec[2].val, "type": str(arg_spec[2])})
                # Fill in our special handling
                item = args
            out_dict[attr.replace("get_", "")] = item
        return out_dict
