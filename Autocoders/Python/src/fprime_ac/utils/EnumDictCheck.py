#!/usr/bin/env python3

import os
import sys


class EnumCheckResults:
    """
    A structure used to report enumeration size errors.
    """

    def __init__(self):

        # Create a container to hold the overrun details. This will be
        # an array. Each element of the array is a tuple containing the
        # the following:
        #
        #    (overrun_type, bit_length, max_value, signed)

        self.overrun_details = list()
        self.underrun_details = list()

        self.overruns = 0  # not enough uplink bits (error)
        self.underruns = 0  # too many uplink bits (warning)
        self.key_errors = 0  # no enum typedef generated (error)
        self.stale_bools = 0  # extra enum typedef generated (warning)

    def add_overrun(self):
        self.overruns = self.overruns + 1

    def add_underrun(self):
        self.underruns = self.underruns + 1

    def add_over_err(self, type, length, max_value, sign):
        self.overrun_details.append((type, length, max_value, sign))

    def add_under_err(self, type, length, max_value, sign):
        self.underrun_details.append((type, length, max_value, sign))

    def add_key_error(self):
        self.key_errors = self.key_errors + 1

    def add_stale_boolean(self):
        self.stale_bools = self.stale_bools + 1

    def has_warnings(self):
        if self.underruns > 0 or self.stale_bools > 0:
            return True
        else:
            return False

    def has_errors(self):
        if self.overruns > 0 or self.key_errors > 0:
            return True
        else:
            return False

    def report_warnings(self):
        print("Enum Check Warnings: (%d)" % (self.underruns + self.stale_bools))
        print("===========================")
        print("    stale boolean: %d" % (self.stale_bools))
        for i in self.underrun_details:
            print("    enumeration underrun: type=%s" % (i[0]))
            print("       bits=%d, max_value=%d, signed=%d" % (i[1], i[2], i[3]))

    def report_errors(self):

        print("Enum Check Errors: (%d)" % (self.overruns + self.key_errors))
        print("===========================")
        print("    missing key: %d" % (self.key_errors))
        for i in self.overrun_details:
            print("    enumeration overrrun: type=%s" % (i[0]))
            print("       bits=%d, max_value=%d, signed=%d" % (i[1], i[2], i[3]))


class EnumInfo:
    def __init__(self, type_name):

        self.signed = False
        self.name = type_name

        self.bits = 0
        self.max_abs_value = 0

    def is_signed(self):
        return self.signed

    def get_bits(self):
        return self.bits

    def max_value(self):
        return self.max_abs_value

    def check_max_value(self, value):

        if value < 0:
            self.signed = True

        if abs(value) > self.max_abs_value:
            self.max_abs_value = abs(value)

    def set_bits(self, bits):

        if bits in (8, 16, 32):
            self.bits = bits
        else:
            str = "ERROR: %s invalid bit length %d." % (self.name, bits)
            raise ValueError(str)


def cmd_dict_enum_size_check(filename, verbose=False):
    """
    This function will read the specified command dictionary file, and
    validate the size of enumerated arguments. The specified uplink size
    must match the size of the enumerated argument.
    """

    if not os.path.exists(filename):
        if verbose == True:
            print("The specified dictionary does not exist: %s" % (filename))
            return None

    enum_info = {}
    enums = {}

    enum = None
    results = EnumCheckResults()

    fd = open(filename)

    for line in fd:

        # Build a map for each enumeration. The key is the name of the
        # enumeration. The value is the highest numeric value of all
        # the items.

        if line.find("<enum_typedef ") != -1:

            fields = line.split('"')

            # The key is the type name.
            key = fields[1]
            enum = EnumInfo(key)
            # print "***Initial max value of %s (%s) is %d" % (key,enum.name,enum.max_abs_value)

            # Find the largest value needed for this enumeration. We also
            # need to know if this is a signed or unsigned enumeration. The
            # assumption is that the enumeration is unsigned if there are
            # only positive values.

            for values in fd:

                if values.find("numeric_value=") != -1:
                    fields = values.split('"')
                    value = int(fields[1])
                    enum.check_max_value(value)

                if values.find("</enum_typedef") != -1:
                    break

            # print ">>>>>Final max value of %s (%s) is %d" % (key,enum.name,enum.max_abs_value)

            if key in enums:
                print("Duplicate enumerations: %s." % (key))
                return None

            # enum_types[key] = value
            enums[key] = enum

        # Build a map of the enumeration usage. The key is again the name of
        # the enumeration. The value is the size in bits that was specified
        # for the enumeration representation.

        if line.find("enum name=") != -1:

            fields = line.split('"')

            # The key is the type name.
            key = fields[3]
            # now this is I32, I16 or I8.  Chop off the I
            integertypename = fields[5]
            integertypename = integertypename[1:]
            value = int(integertypename)
            if key[0:3] != "All":
                enums[key].set_bits(value)

                if key in enum_info:
                    # Skip the ones we generated.
                    if key.find("InstanceId") == -1:
                        print("Duplicate enumeration usages: %s." % (key))
                        return None

                enum_info[key] = enums[key]
            # print "----Stored max value of %s (%s) is %d" % (key,enum_info[key].name,enum_info[key].max_abs_value)

    fd.close()

    if verbose == True:
        print("Done gathering %d enumerations." % (len(enums)))
        print("Done gathering %d usages." % (len(enum_info)))

    for type in enums.keys():

        # Calculate the allowed maximum value for this type based
        # on the bit length that was specified. We want the maximum
        # number that can be represented in those bits.

        try:

            e = enum_info[type]

            signed = 0
            max_value_8 = 0
            max_value_16 = 0
            max_fsw_value = 0

            if e.is_signed() == True:
                # Do not include the sign bit for signed numbers
                signed = 1
                max_value_8 = 128
                max_value_16 = 32768
                max_fsw_value = 2 ** (e.get_bits() - 1)
            else:
                # All bits used to determine max value
                signed = 0
                max_value_8 = 256
                max_value_16 = 65536
                max_fsw_value = 2 ** e.get_bits()

            # Check for enumeration overruns errors
            if e.max_value() > max_fsw_value:
                results.add_overrun()
                results.add_over_err(type, e.get_bits(), e.max_value(), signed)

            # Check for enumeration underruns warnings
            if e.max_value() < max_value_8 and not e.get_bits() == 8:
                # Could use only 8 uplink bits.
                results.add_underrun()
                results.add_under_err(type, e.get_bits(), e.max_value(), signed)

            elif e.max_value() <= max_value_16 and e.get_bits() == 32:
                # Could use only 16 uplink bits.
                results.add_underrun()
                results.add_under_err(type, e.get_bits(), e.max_value(), signed)

        except KeyError:
            if type.startswith("Bool"):
                results.add_stale_boolean()
            else:
                results.add_key_error()

    if verbose == True:
        print("Done performing dictionary enumeration check.")

    return results


if __name__ == "__main__":

    filename = "../../fsw/msl/currentxml/command.xml"

    status = cmd_dict_enum_size_check(filename, True)

    if status is None:
        print("Error perform dictionary enumeration check.")
        sys.exit(-1)

    status.report_warnings()
    status.report_errors()

    sys.exit(0)
