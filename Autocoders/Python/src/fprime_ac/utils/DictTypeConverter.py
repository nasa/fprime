import sys


class DictTypeConverter:
    def __init__(self):
        pass

    def convert(self, t, size):

        # check for various type variations
        type_string = ""
        type_name = t
        ser_import = None
        use_size = False if size is None else True
        # check for enums
        if isinstance(t, tuple):
            # extract enumeration arguments
            # to match the C rules, we have to start
            # counting member values from 0 or the
            # last member value
            curr_memb_val = 0
            # make sure it's an enum
            if t[0][0].upper() != "ENUM":
                print("ERROR: Expected ENUM type in channel args list...")
                sys.exit(-1)
            type_string += 'EnumType("' + t[0][1] + '",{'

            for (mname, mval, mcomment) in t[1]:
                # check for member value
                if mval is not None:
                    curr_memb_val = int(mval)
                type_string += '"%s":%d,' % (mname, curr_memb_val)
                curr_memb_val += 1
            type_string += "})"
            type_name = "enum"
        # otherwise, lookup type translation in table
        elif t == "string":
            use_size = False
            type_string += "StringType(max_string_len=%s)" % size
        else:
            type_lookup = {
                "U8": "U8Type()",
                "I8": "I8Type()",
                "I16": "I16Type()",
                "U16": "U16Type()",
                "I32": "I32Type()",
                "U32": "U32Type()",
                "I64": "I64Type()",
                "U64": "U64Type()",
                "F32": "F32Type()",
                "F64": "F64Type()",
                "bool": "BoolType()",
            }
            if t in type_lookup:
                type_string += type_lookup[t]
            else:  # set up serializable imports
                # Path to serializable is going to be the namespace.type
                ser_type = t.split("::")
                type_string += "{}.{}()".format(".".join(ser_type), ser_type[-1])
                ser_import = ".".join(ser_type)
        return (type_string, ser_import, type_name, use_size)

    def format_replace(self, format_string, spec_num, old, new):
        """
        Search the format specifier string and replace tokens
        Mainly a special case to handle enumerations. Software
        needs "%d", while Gse needs "%s"
        spec_num = instance of token (0..n)
        """
        # split into list
        flist = format_string.split("%")
        # make sure we're not looking past the list
        if spec_num + 1 >= len(flist):
            return None
        # replace token
        flist[spec_num + 1] = flist[spec_num + 1].replace(old, new, 1)
        # rejoin string
        return "%".join(flist)
