#!/usr/bin/env python3

# This file contains a set of Python functions that parse C code. The
# idea is to build this library as we need more capability. The functions
# use the pyparsing module to perform text level parsing.
#
# FIXME: These routines currently don't handle code that contains conditional
# compilation. We need to decide if these routines must support parsing of
# conditional compilation code.

import os

from .pyparsing import (
    Forward,
    Literal,
    Optional,
    Word,
    ZeroOrMore,
    alphas,
    cStyleComment,
    nums,
    restOfLine,
)

# Constants
comma = Literal(",").suppress()
equal = Literal("=").suppress()
space = Literal(" ").suppress()
left_brace = Literal("{").suppress()
left_paren = Literal("(").suppress()
right_brace = Literal("}").suppress()
right_paren = Literal(")").suppress()
semicolon = Literal(";").suppress()
identifier = Word(alphas + "_", alphas + nums + "_$")  # c-identifier
decORhex = nums + "ABCDEFXabcdefx"


def ParseNumDefine(defname, filename, loadfile=True):
    """
    This function is used to parse a C #define statement, and return the
    value as an integer. This only works for integers numbers that are
    defined.

    @param: defname - the name of the #define
    @param: filename - the filename to parse for the enum
    @param: loadfile - True = read/open from file; False = don't read/open

    @return: the integer value, or exception

    Note: A ValueError exception is raised if the value is not found,
    or if the value cannot be represented as an integer.
    """
    if loadfile not in (True, False):
        raise ValueError("%r: invalid loadfile argument" % loadfile)

    results = ""

    # Open and read the entire source code file into a single
    # variable. We only do this if the loadfile argument is
    # set to True.

    if loadfile:

        if not os.path.isfile(filename):
            raise OSError("%r: file not found." % filename)

        fd = open(filename)

        data = fd.read()
        fd.close()

    else:
        # The file is already in memory. We just need to get
        # reference where we need it. No loading needed.
        data = filename

    # Configure a parser to pickoff the #define. Should we restrict the
    # define name to be uppercase? We will use any case for now. So,
    # we are looking for '#define id value'.

    _ = Forward()

    keyword = Literal("#define").suppress()
    name = Literal(defname).suppress()

    # FIXME: We may want to expand this function to parse generic
    # define values, and return them as strings. The string value
    # may be more useful.

    parser = keyword + name + Optional(left_paren) + Word(nums) + Optional(right_paren)

    parser.ignore(cStyleComment)
    parser.ignore(Literal("//") + restOfLine)

    # Parse the file using the configured parser. Extract the results
    # as a single string. This string is the value of the define without
    # optional parenthesis.

    for toks, start, end in parser.scanString(data, 1):
        # Return the first one we find. There should only be one, but
        # any conditional compilation in the code could result in a
        # wrong value returned.
        results = int(toks[0])
        break

    return int(results)


def ParseTypedefEnum(typename, filename, loadfile=True):
    """
    This function is used to parse a C enumeration typedef from the
    specified string. The results of the parse return a dictionary
    value of the enumeration name/value pairs. This only works for
    named typedefs.

    The loadfile argument is used to indicate if the specified file
    should be opened and read, or if the file is already loaded into
    the filename argument.

    @param: typename, The name of the enum typedef
    @param: filename, The filename to parse for the enum
    @param: loadfile, True to open filename; False if already in memory
    @return: a dictionary of enumeration name/value pairs
    """

    if loadfile not in (True, False):
        raise ValueError("%r: invalid loadfile argument" % loadfile)

    dictionary = {}

    # Open and read the entire source code file into a single
    # variable. We only do this if the loadfile argument is
    # set to True.

    if loadfile:

        if not os.path.isfile(filename):
            ##################
            # EddieB ENUM path fix but I replace try/except
            # with existence test for MSL_ROOT.
            # LJR - 15 Nov. 2007
            ##################
            if "MSL_ROOT" in os.environ:
                msl_root = os.environ["MSL_ROOT"]
            else:
                str = "MSL_ROOT is not defined in the environment."
                print(str)
                raise KeyError(str)
            filename = os.path.join(msl_root, filename)
            if not os.path.isfile(filename):
                ######################
                str = (
                    "ERROR: utils.ParseC.ParseTypedefEnum: %s file not found."
                    % filename
                )
                print(str)
                raise OSError(str)

        fd = open(filename)

        data = fd.read()
        fd.close()

    else:
        # The file is already in memory.  We just need to get
        # reference where we need it. No loading required.
        data = filename

    if typename == "" or typename is None:
        str = (
            "ERROR: utils.ParseC.ParseTypedefEnum typename argument empty (%s)"
            % typename
        )
        print(str)
        raise ValueError(str)

    #
    # Configure a parser to pickoff a typedef enumeration. This only works
    # for named typedefs of enumerations. We are looking for 'typedef enum'
    # followed by the typename.

    _ = Forward()

    keyword = Literal("typedef enum").suppress()
    typetoken = Literal(typename).suppress()

    # 04/23/07: Fix allows for negative numbers.
    _ = Word("=" + " " + "-" + nums) + restOfLine.suppress()

    # 03/21/08: Fix allows hexadecimal numbers.
    _ = Word("=" + " " + "-" + decORhex) + restOfLine.suppress()

    # 04/04/08: Fix allows (int) cast enumerations. TBD: Must fine
    # some way to suppress the inclusion of the (int) cast. We want
    # is parsed but not included. For now, it is removed from follow
    # on search.

    val = Word("=" + " " + "-" + decORhex + "(int)") + restOfLine.suppress()

    enums = identifier + Optional(comma + restOfLine.suppress() ^ val)
    parser = (
        keyword
        + Optional(identifier).suppress()
        + left_brace
        + ZeroOrMore(enums)
        + right_brace
        + typetoken
        + semicolon
    )

    parser.ignore(cStyleComment)
    parser.ignore(Literal("//") + restOfLine)
    #
    # Check that the enum exists within the file
    # and if it does not then raise an exception.
    #
    # LJR Added 30 July 2007 to flag missing enum definitions.
    #
    if typename not in data:
        str = "ERROR: could not find ENUM type (%s)" % typename
        print(str)
        raise ValueError(str)

    # Parse the file using the configured parser and scanString. The
    # processing loop is empty since there is only one match. The parse
    # should contain one value since only one typedef can exist with
    # this name.
    #
    # Forget about the start, and end values. Those contains the start
    # and end locations of the keywords. We are interested in the parsed
    # tokens. There should only be one instance returned. This corresponds
    # to the one named typedef in the file. There is a C code error if
    # the typedef is multiply defined.

    toks = []
    for toks, start, end in parser.scanString(data, 1):
        continue

        # Nothing in the loop. We just want the tokens. The tokens are
        # further parsed into a dictionary. This second parse is needed
        # since some enums may be missing values. We need to supply a
        # value in those cases.

    # In some cases, the value of the enumeration is not specified. We
    # then need to supply a value in those cases. The value will be one
    # more than the last value -- just like C. All default values will
    # start at 0.

    if len(toks) == 0:
        str = "ERROR: could not find ENUM ({}) in {}".format(typename, filename)
        print(str)
        raise ValueError(str)

    # In ANSI/ISO C, enums must be legal int values. On MSL FSW target,
    # that means 32-bits, signed. Need to convert hex values to signed int.
    sign_bit = int("0x8" + "0" * 7, 0)
    sign_mask = int("0x7" + "F" * 7, 0)
    pt = ""
    val = 0

    # To debug this loop, set the watchForType value to be what you
    # are interested in debugging, such as 'MrfType'
    debugOnType = "no_debugging_desired_here_sir_carry_on"

    for i in range(len(toks)):

        t = toks[i]

        if typename == debugOnType:
            print(typename + "(0):")
            print(t)

        # Kludge from above. Remove the cast that we eventually
        # want to have suppressed during the parse. This does
        # not account for all cast, just the specific int cast
        # with no spaces.

        t = t.replace("(int)", "")

        if typename == debugOnType:
            print(typename + "(1):")
            print(t)

        # Use the supplied value if specified.
        if t[0:1] == "=":
            # Use int to convert hex2dec.
            if typename == debugOnType:
                print(typename + "(2):")
                print(t[1:])
                print(int(t[1:], 0))
            val = int(t[1:], 0)
            if val & sign_bit:
                val = -((~val & sign_mask) + 1)
            dictionary[pt] = val
        else:
            dictionary[t] = val

        if typename == debugOnType:
            print(typename + "(3):")
            print(val)
        val = val + 1
        pt = t

    return dictionary


def ParseTypedefEnumValue(name, typename, filename, loadfile=True):
    """
    This function returns the value for a C enum value that exists in
    the specified C filename. A numeric value is returned representing
    the value of the enum. An exception is raised if nothing is found.

    @param: enumname - the name of the enumeration to find
    @param: typename - the name of the enumerated type
    @param: filename - the name of the C file
    @param: loadfile - True to load filename; False if already loaded.
    """

    enums = {}

    # Use the ParseTypedefEnum function to perform the first
    # half of this function. This will return all the enumeration
    # values of the typedef.

    enums = ParseTypedefEnum(typename, filename, loadfile)

    if name not in enums:
        raise ValueError("%r: enumeration not found" % name)

    return int(enums[name])


testdata = """

#include <upl/upl_pub.h>
#include <gbl/gbl_pub.h>

/*! \brief Command status
*
* This is the command status. This is for each command process to
* indicate the command processing status. This is used in conjunction
* with the command reply handler.
*
* Note: The first two entries in this enumeration are intentionally
* made to match the status of MsapFswStatus. Keep the primary success,
* and fail conditions as 0 and 1 in this list.
*/
typedef enum cmd_status {
    CMD_PROC_SUCCESS   = 0,            //!< Command processed successfully
    CMD_PROC_FAILURE,                  //!< Command failed module processing
    CMD_FAILED_VALIDATE = 0xa2,        //!< Command failed validation
    CMD_FAILED_SIDE_CHECK,             //!< Command failed side check
    CMD_FAILED_MODE_CHECK =  8,        //!< Command failed mode check
    CMD_FAILED_CUSTOM_VALIDATE,        //!< Command failed custom validation
    CMD_FAILED_IPC_ENQUEUE,            //!< Command failed ipc enqueue
    CMD_FAILED_IPC_ENQUEUEX= (int) 7   //!< Command failed ipc enqueue
} CmdStatus;

#define APPLE                         (this is a test.)
#define ORANGE                        (A + B * C / D)
#define NULL                          (0)
#define PEAR                          (11)

"""

if __name__ == "__main__":

    ##################################################

    typename = "CmdStatus"
    enumname = "CMD_FAILED_MODE_CHECK"

    print()
    print("Checking ParseTypedefEnumValue...")
    print("Typedef: %s" % typename)

    result = ParseTypedefEnumValue(enumname, typename, testdata, False)

    print("The value of %s is: %d." % (enumname, result))

    ##################################################

    index = 0
    typename = "CmdStatus"

    print()
    print("Checking ParseTypedefEnum...")
    print("Typedef: %s" % typename)

    results = ParseTypedefEnum(typename, testdata, False)

    for key, value in list(results.items()):
        print("    " + str(key) + " -> " + str(value))

    ##################################################

    print()
    print("Checking ParseNumDefine...")
    define = ["APPLE", "PEAR", "ORANGE"]

    for d in define:
        try:
            value = ParseNumDefine(d, testdata, False)
            print("The value of " + d + " is: " + str(value))
        except ValueError:
            print(d + ": could not find define.")


##################################################
