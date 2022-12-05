# ===============================================================================
# NAME: formatters.py
#
# DESCRIPTION: This file contains the Formatters class
#              which contains various routines to output
#              formatted strings for code generation.
#
# USAGE: Nominally each visitor will instance this and
#        use the methods privately in conjunction with
#        various templates.
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Feb 6, 2007
#
# Copyright 2007, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================

import logging
import os
import re

from fprime_ac.utils import ConfigManager

PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")


class CommentFormatters:
    """
    This class contains a collection of format methods to format a comment
    string. These are derived from the legacy code in gen_msg.py but may
    not be the same.
    """

    __config = None

    def __init__(self):
        """
        Constructor.
        """
        self.__config = ConfigManager.ConfigManager.getInstance()

    def _dlog(self, ddt, arg):
        """
        Protected debugging method.
        """
        if ddt == 1:
            print(arg)

    def _strip_without_code(self, line_list):
        """
        A protected method that strips spaces, carriage returns, etc. off each line
        in the list of lines.  Except that lines between a start @code line and
        an end @code line are left alone.

        For the @code lines the leading number of spaces before the @ of
        the first @code is stripped off each line.
        """

        new_line_list = []

        code_flag = False

        for line in line_list:

            if "@code" in line and not code_flag:
                leading_spaces = line.find("@code")
                new_line_list.append(line.strip())
                code_flag = True
            elif "@code" in line and code_flag:
                new_line_list.append(line.strip())
                code_flag = False
            elif (not "@code" in line) and code_flag:
                new_line_list.append(line[leading_spaces:])
            else:
                new_line_list.append(line.strip())

        return new_line_list

    def _prefix_line(self, str, lines):
        """
        Prefix a string of lines with str, each line must end with '\n'.
        """

        new_lines = ""

        line_list = lines.split("\n")

        # Added since an empty line seems to be added.
        del line_list[-1]

        for l in line_list:
            new_lines = new_lines + str + l + "\n"

        return new_lines

    def _getStartExcludingNewlines(self, line_list):
        """
        Eddie B. added 7 Aug. 2007
        Returns the first non-return element in a list
        Returns -1 if everything is a newline
        """

        for count, item in enumerate(line_list):
            item = item.strip()
            if item != "":
                return count
        return -1

    def _getEndsExcludingNewlines(self, line_list):
        """
        Eddie B. added 7 Aug. 2007
        Return the index last element of the list + 1, with the thrown away
        elements being all white space.
        Return -1 if all white space.
        """

        index = len(line_list) - 1

        while index >= 0:
            item = line_list[index]
            item = item.strip()
            if item != "":
                return index + 1
            index -= 1

        return -1

    def _wrapText(self, text, indent, prefix, width=70):
        """
        This function formats a block of text. The text is broken into
        tokens. (Whitespace is NOT preserved.) The tokens are reassembled
        at the specified level of indentation and line width.  A string is
        returned.
        """

        out = []

        width = width - indent

        stack = [word for word in text.replace("\n", " ").split(" ") if word]

        while stack:

            line = ""

            while stack:
                if len(line) + len(" " + stack[0]) > width:
                    break
                if line:
                    line += " "
                line += stack.pop(0)

            out.append(prefix + " " * indent + line)

        return "\n".join(out)

    def _commentWrap(self, message, text, length):
        """
        Return text comments wrapped at the specified length.

        Steve: 03-01-08
        """

        comment = message + " " + text

        return self._wrapText(comment, 0, "* ", 70)

    def commentDraw(self, user_comment, arguments, type="iface", check_and_send=False):
        """
        This method is used to format the comments for a function. The
        comments will include information about each argument. The text
        is formatted in doxygen style.
        """

        line_list = user_comment.split("\n")

        comment_str = self.__config.get("ipc", "comment_title_token")
        comment_str += "! \\brief "

        started = False
        trim_left_spaces = 0
        trimwhitespace = True

        start = self._getStartExcludingNewlines(line_list)
        end = self._getEndsExcludingNewlines(line_list)

        if start == -1:
            return

        for index in range(start, end):

            line = line_list[index]

            if "@code" == line.strip():
                trim_left_spaces = line.find("@code")
                trimwhitespace = False
            elif r"\code" == line.strip():
                trim_left_spaces = line.find(r"\code")
                trimwhitespace = False
            elif "@endcode" == line.strip() or r"\endcode" == line.strip():
                trimwhitespace = True
            elif not trimwhitespace and "@code" == line.strip():
                trimwhitespace = True
            elif not trimwhitespace and r"\code" == line.strip():
                trimwhitespace = True

            if started:
                comment_str += "* "
            else:
                started = True

            if trimwhitespace:
                comment_str += line.strip() + "\n"
            else:
                comment_str += line[trim_left_spaces:] + "\n"

        if type == "iface":
            comment_str += "*\n"
            comment_str += "* TYPE: THIS IS AN ASYNCHRONOUS INTERFACE.\n"
        elif type == "cface":
            comment_str += "*\n"
            comment_str += "* TYPE: THIS IS AN ASYNCHRONOUS COMMAND INTERFACE.\n"
        elif type == "sface":
            comment_str += "*\n"
            comment_str += "* TYPE: THIS IS A SYNCHRONOUS INTERFACE.\n"

        if check_and_send:
            comment_str += "*\n"
            comment_str += "* Note: This interface uses an IPC check and send call. The return status\n"
            comment_str += "* indicates if the message was sent, or if the message was not sent due to\n"
            comment_str += "* a full queue condition.\n"

        if len(arguments) > 0:
            comment_str += "*\n"

            # Go through the args and provide additional comments for repeat args.
            for args in arguments:
                if args[3] != "":
                    comment_str += "* NOTE: This command contains an array argument. The size shows an allocation\n"
                    comment_str += "* to indicate the maximum size. The actual size can be from 1 to the maximum\n"
                    comment_str += "* size.\n"

        # Go through and generate doxygen for each argument.
        if len(arguments) > 0:
            comment_str += "*\n"

            for args in arguments:
                msg = "\\param " + args[0]
                comment_str += self._commentWrap(msg, args[2], 70)
                comment_str += "\n"

            comment_str += "*\n"

        comment_str += "*/"

        return comment_str

    ########################################
    # Comment Manipulation
    ########################################

    def commentHeaderFormat(self, comment_stuff, type="iface"):
        """
        Method to format header comments and add interface type label. No limit
        on line length is imposed so it is assumed the XML is of reasonable length
        text lines. Recognizes @code directives for doxygen.
        """

        line_list = comment_stuff.split("\n")

        comment_str = self.__config.get("ipc", "comment_title_token")
        comment_str += "! \\brief "

        started = False
        trim_left_spaces = 0
        trimwhitespace = True

        start = self._getStartExcludingNewlines(line_list)
        end = self._getEndsExcludingNewlines(line_list)

        if start == -1:
            return

        for index in range(start, end):

            line = line_list[index]

            if "@code" == line.strip():
                trim_left_spaces = line.find("@code")
                trimwhitespace = False
            elif r"\code" == line.strip():
                trim_left_spaces = line.find(r"\code")
                trimwhitespace = False
            elif "@endcode" == line.strip() or r"\endcode" == line.strip():
                trimwhitespace = True
            elif not trimwhitespace and "@code" == line.strip():
                trimwhitespace = True
            elif not trimwhitespace and r"\code" == line.strip():
                trimwhitespace = True

            if started:
                comment_str += "* "
            else:
                started = True

            if trimwhitespace:
                comment_str += line.strip() + "\n"
            else:
                comment_str += line[trim_left_spaces:] + "\n"

        comment_str += "*\n"

        if type == "iface":
            comment_str += "* TYPE: THIS IS AN ASYNCHRONOUS INTERFACE."
        elif type == "cface":
            comment_str += "* TYPE: THIS IS AN ASYNCHRONOUS COMMAND INTERFACE."
        elif type == "sface":
            comment_str += "* TYPE: THIS IS A SYNCHRONOUS INTERFACE."

        comment_str += "\n*/\n"

        return comment_str

    ########################################
    # Comment Manipulation - from legacy gen_msg.py
    ########################################

    def commentFormat(self, stuff_in_brackets):
        ddt = 0
        ddt2 = 0
        self._dlog(ddt, "org == \n%s" % stuff_in_brackets)

        line_list = stuff_in_brackets.split("\n")
        this_line = ""
        paragraph = ""
        para_list = []

        for line in line_list:
            self._dlog(ddt, "line ==%s==" % line)
            if line != "":
                self._dlog(ddt, "... appended")
                paragraph = paragraph + " " + line.strip()
            else:
                self._dlog(ddt, "... new paragraph")
                if paragraph != "":
                    para_list.append(paragraph)
                else:
                    self._dlog(ddt, "... is empty so ignored")
                paragraph = ""

        # Make sure the last paragraph is appended to paraList
        if paragraph != "":
            para_list.append(paragraph)

        formatted_comment = ""

        for paragraph in para_list:
            self._dlog(ddt, "paragraph ==%s==" % paragraph)

            # Separate paragraphs with a blank line -- not at beginning, not at end
            if formatted_comment != "":
                formatted_comment = formatted_comment + "\n *"

            word_list = paragraph.split()
            this_line = ""

            for this_word in word_list:
                self._dlog(ddt2, "this_word is: %s" % this_word)
                if this_word == "":
                    continue
                len_this_line = len(this_line)
                len_this_word = len(this_word)
                len_with_word = len_this_line + len_this_word

                # 76 'cuz we'll prepend " * ", and "less than" because we append a blank
                if len_with_word < 76:
                    this_line = this_line + " " + this_word
                else:
                    # This even handles the case where this_word is longer than the line limit
                    # remember to get rid of the trailing space at end of the line
                    self._dlog(ddt2, "...append formatted_comment here.")
                    if formatted_comment == "":
                        if this_line != "":
                            this_line = this_line.strip()
                            formatted_comment = " * %s" % this_line
                    else:
                        this_line = this_line.strip()
                        formatted_comment = "{}\n * {}".format(
                            formatted_comment, this_line
                        )
                    this_line = this_word
                self._dlog(ddt2, "len_with_word = %d" % len_with_word)
                self._dlog(ddt2, "this_line is: %s" % this_line)
                self._dlog(ddt2, "formatted_comment is: %s" % formatted_comment)

        # Make sure we append the last (short) line to the body of the comment
        if formatted_comment == "":
            this_line = this_line.strip()
            formatted_comment = " * %s" % this_line
        else:
            this_line = this_line.strip()
            formatted_comment = "{}\n * {}".format(formatted_comment, this_line)

        self._dlog(ddt, "formatted_comment ==\n%s" % formatted_comment)
        return formatted_comment


class Formatters:
    """
    Class that provides a collection of routines to format strings for
    code generation.  The start of this class is generic for doing
    all sorts of string name manipulations for formatting.
    The rest of these routines are a legacy for renaming things towards
    the MSL naming conventions or test something exists.  As time
    goes by, the MSL naming routines will be removed and replaced
    by newer ones.
    """

    __config = None
    __instance = None

    def __init__(self):
        """
        Constructor.
        """
        self.__config = ConfigManager.ConfigManager.getInstance()

    def getInstance():
        """
        Return instance of singleton.
        """
        if Formatters.__instance is None:
            Formatters.__instance = Formatters()
        return Formatters.__instance

    # define static method
    getInstance = staticmethod(getInstance)

    def _dlog(self, ddt, arg):
        """
        Protected debugging method.
        """
        if ddt == 1:
            print(arg)

    def capFirstCharTuple3(self, t, en):
        """
        For three element tuple capitalize the first character of select
        elements.   They are enabled by binary bits (e.g. en=111 sets
        each element, en=010 set element 1, etc.)
        @param t: A three element tuple
        @param en: A binary selection of which elements to capitalize.
        """
        i = en
        t2 = []
        if len(t) != 3:
            PRINT.info("Tuple must be 3 element only!")
            raise TypeError

        for s in t:
            if 1 & i:
                t2.append(self.capFirstChar(s))
            else:
                t2.append(s)
            i = i >> 1

        return (t2[0], t2[1], t2[2])

    def capFirstChar(self, s):
        """
        Capitalize the first character of string name.
        """
        return s[0].upper() + s[1:]

    #########################################################################
    #
    #  OLD MSL FORMATTERS FROM HERE DOWN...
    #
    #########################################################################
    def functionStringName(self, id, name, str, itype="iface", verbose=True):
        """
        Return a function name as specified in the MSL coding style guidelines
        document. If the type is a command interface (cface), then check for
        a trailing _cmd as direct in the coding guidelines document.

        @param id: The module id (name)
        @param name: The user specified function name
        @param str:  A short string with '_' separation.
        """

        name = name.lower()
        name_list = name.split("_")

        # If the first part is a module name with an '_' then assume
        # the form of the name is module_subtask and split them.

        sub_task = ""
        id_list = id.split("_")

        if len(id_list) == 2:
            id = id_list[0]
            sub_task = id_list[1]
        elif len(id_list) > 2:
            str = "ERROR: Invalid module_subtask name: %s" % (id)
            PRINT.info(str)
            raise SyntaxError(str)

        # If first item of name_list is module id take it off.
        if name_list[0] == id:
            name_list = name_list[1:]
        else:
            pass

        # If new first item is subtask name take it off.
        if name_list[0] == sub_task:
            name_list = name_list[1:]
        else:
            if sub_task != "":
                pass

        if sub_task == "":
            name_str = id + str
        else:
            name_str = id + "_" + sub_task + str

        for n in name_list:
            name_str = name_str + "_" + n

        if itype == "cface":
            end_name = name_str.split("_")[-1]
            if end_name != "cmd":
                name_str = name_str + "_cmd"
        # Steve: removed, this is too verbose
        #                if verbose:
        #                    PRINT.info("WARNING: Interface %s renamed to %s."  % (name2,name_str))

        # Steve: removed, this is too verbose
        #        if warning_flag == True and verbose == True:
        #            PRINT.info("WARNING: Interface %s renamed to %s."  % (name2,name_str))

        return name_str

    def functionOkName(self, id, name, itype="iface", verbose=False):
        """
        Return a mod_ok_name form name.
        @param id: The module id.
        @param name: Original function name with '_' separation.
        """
        return self.functionStringName(id, name, "_ac_ok", itype, verbose)

    def functionParseName(self, id, name, itype="iface", verbose=False):
        """
        Return a mod_ok_name form name.
        @param id: The module id.
        @param name: Original function name with '_' separation.
        """
        return self.functionStringName(id, name, "_ac_parse", itype, verbose)

    def functionUnpackName(self, id, name, itype="iface", verbose=False):
        """
        Return a mod_unpack_name form name.
        @param id: The module id.
        @param name: Original function name with '_' separation.
        """
        return self.functionStringName(id, name, "_ac_unpack", itype, verbose)

    def function_handler_name(self, id, name):
        """
        Return a mod_unpack_name form name.
        @param id: The module id.
        @param name: Original function name with '_' separation.
        """
        name_str = name + "_handler"
        return name_str

    def msgTypedefName(self, id, name, name_sep="AcMsg"):
        """
        Return the name of msg typedef from an import function name.
        @param id: The module id.
        @param name: Original function name with '_' separation.
        @param name_sep: A string placed between the id and interface name
        """

        # print id
        id_list = id.strip("_").split("_")

        name_list = name.strip("_").split("_")

        # print name_list
        name_list = list(map((lambda x: x[0].upper() + x[1:]), name_list))

        if len(id_list) == 1:
            # main module thread naming
            name_list = [id[0].upper() + id[1:]] + [name_sep] + name_list[1:]
        elif len(id_list) == 2:
            # sub task thread naming
            # ModSubtaskAcMsgInterface
            name_list = (
                [id_list[0][0].upper() + id_list[0][1:]]
                + [id_list[1][0].upper() + id_list[1][1:]]
                + [name_sep]
                + name_list[2:]
            )

        # Error check done in subThreadDir method for module name list size.
        new_name = ""
        new_name = new_name.join(name_list)
        # print new_name
        return new_name

    def msgUnionArgName(self, msg_type):
        """
        Return the name of a standard argument for a union
        used to determine the maximum size of a message type.
        @param msg_type: The Typedef msg name (must include 'AcMsg')
        """
        if -1 != msg_type.find("AcMsg"):
            name = msg_type.split("AcMsg")[1]
            new_name = ""
            for c in name:
                if c.isupper():
                    new_name += "_" + c.lower()
                else:
                    new_name += c
            return new_name[1:]
        else:
            return "None"

    def msgTokenName(self, id, name):
        """
        Return the name of msg typedef
        from an input function name.
        @param id: The module id.
        @param name: Original function name with '_' separation.
        """
        id_list = id.split("_")
        name_list = name.split("_")
        name_list = list(map((lambda x: x.upper()), name_list))
        # original single thread case
        if len(id_list) == 1:
            name_list = [id.upper()] + ["AC_MSG"] + name_list[1:]
        elif len(id_list) == 2:
            name_list = (
                [id_list[0].upper()] + [id_list[1].upper()] + ["AC_MSG"] + name_list[2:]
            )
        new_name = "_"
        new_name = new_name.join(name_list)
        return new_name

    def opcodeName(self, id, name):
        """
        Return the name of command opcode
        from an input function name.
        @param id: The module id.
        @param name: Original function name with '_' separation.
        """
        name_list = name.split("_")
        name_list = list(map((lambda x: x.upper()), name_list))
        # Test for trailing _CMD and remove.
        if name_list[-1] == "CMD":
            del name_list[-1]
        name_list = [id.upper()] + ["AC_OPCODE"] + name_list[1:]
        new_name = "_"
        new_name = new_name.join(name_list)
        return new_name

    def opcodeStemName(self, id, name):
        """
        Return a command stem name per MSL naming rules.
        1. Convert all characters to uppercase.
        2. Test for trailing _cmd or _CMD and remove.
        3. Test for leading module name and if none add.
        4. Test for any char not [A-Z] or [0-9] or _.
        5. Test for 32 character limit on command name (configurable)
           and if exceeded then clip name.
        5. If all was ok return the command stem name.
        @param id: The module id.
        @param name: Original function name with '_' separation.
        @return: Valid command stem name stri
        """

        max_stem_length = int(self.__config.get("ipc", "max_stem_length"))

        name_list = name.upper().split("_")
        mod_id = id.upper()
        # Test for trailing _CMD and remove.
        if name_list[-1] == "CMD":
            del name_list[-1]
        # Test for leading module name.
        if name_list[0] != mod_id:
            name_list = [mod_id] + name_list
        # Make the string and clip to 32 chars.
        name_string = "_"
        name_string = name_string.join(name_list)[:max_stem_length]
        # Check characters
        if len(re.findall("[^A-Z0-9_]", name_string)) != 0:
            PRINT.info(
                "ERROR: DETECTED AN INVALID CHARACTER IN COMMAND STEM NAME (%s)."
                % name_string
            )
            raise ValueError(
                "Fatal error, detected an invalid character in command stem name."
            )
        # All is ok
        return name_string

    def opcodeStemNameValidate(self, id, cmd_name_list):
        """
        Called for generation of the mod_ac_msg.h
        code file.  If there are repeated stem
        names than through exception and stop
        everything.
        @param cmd_name_list: list of command function names.
        @return: TRUE if all command stem names are unique, else raise an exception.
        """
        cmds = []

        for c in cmd_name_list:
            cmds.append(self.opcodeStemName(id, c))

        for c in cmds:
            if sum([int(x == c) for x in cmds]) > 1:
                PRINT.info("ERROR: DETECTED %s COMMAND STEM NAME REPEATED." % c)
                raise ValueError("Error detected repeated command stem name.")
        return True

    def evrNamePrefix(self, name):
        """
        Return an evr prefix name to use
        after the module id but before
        the evr description in the EVR
        id's within the templates.
        """
        name_list = name.split("_")
        name_list = list(map((lambda x: x.upper()), name_list))
        name_list = name_list[1:]
        new_name = "_"
        new_name = new_name.join(name_list)
        return new_name

    def bufferArgsPresent(self, args):
        """
        Given a list of argument tuples
        return True if an array arg is
        found, else return False.
        """
        return any(arg[3] != "" for arg in args)

    def commentInArgsPresent(self, args):
        """
        Given a list of argument tuples
        return True if a comment for
        an arg is found, else return False.
        """
        return any(arg[2] != "" for arg in args)

    ##########################################
    # Methods for argument handling.
    ##########################################

    ##########################################
    # Methods formatting methods.
    ##########################################

    def argNameConvert(self, arg):
        """
        Return function argument in the
        form of type arg, for either
        scalar or array.
        @param arg: tuple of (id, type, comment, max_size, size)
        """

        if arg[3] == "":
            return "{} {}".format(arg[1], arg[0])
        else:
            return "{} {}[{}]".format(arg[1], arg[0], arg[3])

    def oneLineFun(self, name, args):
        """
        Return function as a single one line
        string.  Form is name( args ) where
        name is a string name and args is
        a list of tuples for the arguments.
        If there is a comment with any arg
        then pad the function with 80 " "
        so arguments will format on new line.
        @param name: Name of the function.
        @param args: List of argument tuples.
        """
        if self.commentInArgsPresent(args):
            func_string = "{}{}(".format(name, 80 * " ")
        else:
            func_string = "%s( " % name

        if len(args) == 0:
            func_string += "void )"
        else:
            for arg in args[:-1]:
                func_string += self.argNameConvert(arg) + ", "
            func_string += self.argNameConvert(args[-1]) + " )"

        return func_string

    def formComment(self, comment):
        """
        If comment greater than max truncate
        and add '...' else leave it alone.

        If comment is not "" prefix with '///< '.
        Note that '///<
        """
        max_len = int(self.__config.get("ipc", "max_arg_comment_length"))
        comment = comment.strip()
        # Filter extra spaces and \n.
        comment = " ".join(comment.split())
        #
        if len(comment) > max_len:
            comment = comment[0:max_len] + "..."
        #
        if comment != "":
            comment = self.__config.get("ipc", "arg_comment_symbol") + " " + comment
        return comment

    def formatFunCall(self, name, args, indent=0, prefix=""):
        """
        Method to format a function call.
        @param name: Name of function.
        @param args: List of arguments in tuple form.
        @param indent: Indent args a little more.
        @param prefix: Prefix each arg with string.
        """
        # Make into one line function call.
        func_string = "%s(" % name
        # Put args together
        if len(args) == 0:
            func_string += ")"
        else:
            for arg in args[:-1]:
                # if a message and pointer then dereference &
                # use &@ instead of &( since two ( break formatFun method.
                #
                #
                # EGB pass by pointer: This is to allow pointers to be passed directly via IPC
                pass_by_pointer = self.getPassByPointer(arg)
                if prefix == "message->" and arg[1][-1] == "*" and not pass_by_pointer:
                    func_string += "&@" + prefix + arg[0] + "),"
                else:
                    func_string += prefix + arg[0] + ", "
            # if a msg and point then dereference &
            # use &@ instead of &( since two ( break formatFun method.
            pass_by_pointer = self.getPassByPointer(args[-1])
            if prefix == "message->" and args[-1][1][-1] == "*" and not pass_by_pointer:
                func_string += "&@" + prefix + args[-1][0] + "))"
            elif prefix == "message." and args[-1][1] == "GblReply*":
                ## STEVE: Make it work statement. Need to pass reply without the prefix.
                func_string += args[-1][0] + ")"
            else:
                func_string += prefix + args[-1][0] + ")"

        format_func = self.formatFun(indent, func_string)
        format_func = format_func.replace("@", "(")
        return format_func

    def simpleFuncDraw(self, name, args, proto=True, indent=0):
        """
        This is a simple C function formatter. The arguments are a function
        name and its arguments. A string is returned that is a C code
        function declaration. The proto flag is used to properly terminate
        the function.
        """

        # Get the simple case out of the way.
        if len(args) == 0:

            if proto:
                function_str = name.strip() + "();"
            else:
                function_str = name.strip() + "() {"

            return function_str

        # The remaining code is used to handle a function with at least
        # one argument. A one argument function is built on one line. All
        # other numbers of arguments will use multiple lines.

        arg_list = []
        type_list = []

        for arg in args:

            a = self.argNameConvert(arg).split()

            # In cases where the user specified an argument with a pointer(*),
            # and inserted a space between the type and variable, the split
            # will actually return 3 values. We want to assign the pointer to
            # the variable in this case. That is probably why the user did it
            # that way.

            if a[0] == "const":
                if len(a) == 3:
                    arg_list.append(a[2])
                    type_list.append(a[0] + " " + a[1])
                else:
                    arg_list.append(a[2] + a[3])
                    type_list.append(a[0] + " " + a[1])
            else:
                if len(a) == 2:
                    arg_list.append(a[1])
                    type_list.append(a[0])
                else:
                    arg_list.append(a[1] + a[2])
                    type_list.append(a[0])

        if len(args) == 1:

            if proto:
                function_str = name.strip() + "(" + a[0] + " " + a[1] + ");"
            else:
                function_str = name.strip() + "(" + a[0] + " " + a[1] + ") {"

            return function_str

        # Determine the longest argument type name. We will use that to
        # align the remaining arguments. This is done solely to make the
        # function look better.

        max_type_len = max(list(map(len, type_list)))

        function_str = name.strip() + "(\n"

        for index in range(len(args) - 1):
            function_str += (indent + 4) * " "
            function_str += type_list[index].ljust(max_type_len + 2)
            function_str += arg_list[index] + ",\n"

        if proto:
            function_str += (indent + 4) * " "
            function_str += type_list[-1].ljust(max_type_len + 2)
            function_str += arg_list[-1] + ");"
        else:
            function_str += (indent + 4) * " "
            function_str += type_list[-1].ljust(max_type_len + 2)
            function_str += arg_list[-1] + ") {"

        return function_str

    def formatFunComment(self, name, args, proto=True, ident=0):
        """
        This method is used to format a function line. This line includes
        the function name, and any arguments that function contains.

        NOTICE: I HAVE REMOVED THE COMMENTS FROM APPEARING AT THE END OF
        EACH ARGUMENT. I FEEL THEY ARE OF LITTLE VALUE WITH THE WAY THEY
        ARE BEING TRUNCATED. A BETTER WAY IS TO SPECIFY THE WHOLE COMMENT
        IN THE FUNCTION COMMENT BLOCK. I PROBABLY WON'T GET THERE, BUT THAT
        IS A TASK WHEN THERE IS MORE TIME.

        @param name: The name of the function
        @param args: A list of function arguments in tuple form
        @param proto: Flag indicating if this is for a function prototype
        """

        pad = 0

        fname = name.strip()

        arg_list = []
        type_list = []
        comment_list = []

        # Get the no argument case out of the way -- just add void argument.
        if len(args) == 0:

            format_func = fname + "(void)"

            if proto:
                format_func += ";"
            else:
                format_func += " {"

            return format_func

        # The args are in tuple form. Go through and separate all the tuple
        # values into their argument type, argument name, and comments.

        for arg in args:
            l = self.argNameConvert(arg).split()
            arg_list.append(l[1])
            type_list.append(l[0])
            comment_list.append(self.formComment(arg[2].strip()))

        # Build a list of strings that have all the argument types, and
        # argument names aligned on the longest lines.

        type_args_list = self.argStringAlign(type_list, arg_list, pad)

        new_list = []
        for line in type_args_list[:-1]:
            new_list.append(line + ",")
        if proto:
            new_list.append(type_args_list[-1] + ");")
        else:
            new_list.append(type_args_list[-1] + ")")

        type_args_list = new_list

        # print "type_args_list"
        # print type_args_list

        #### STEVE: REMOVED THE TRUNCATED COMMENTS.
        ####        # Build the type/arg/comment aligned in a list of strings.
        ####        type_args_comments_list = self.argStringAlign(
        ####             type_args_list, comment_list, 1)
        ####
        ####        #print type_args_comments_list
        ####
        #### STEVE: REMOVED ATTEMPT TO PUT EVERYTHING ON ONE LINE. JUST USE TWO
        #### AND MAKE IT SIMPLE.

        format_func = fname + "(\n"

        for line in type_args_list[:-1]:
            format_func += "{}{}\n".format(pad * " ", line)

        # Last line if not a prototype then no '\n' at end.
        if len(args) > 1:
            if proto:
                format_func += "{}{}\n".format(pad * " ", type_args_list[-1])
            else:
                format_func += "{}{}".format(pad * " ", type_args_list[-1])

        # print "Formatted function call"
        # print format_func
        return format_func

    def formatFunCommentOldVersion(self, name, args, proto=True, indent=0):
        """
        Method to format a function line
        formatFun but add argument comments
        if they exist.
        @param name: Name of the function.
        @param args: List of arguments in tuple form.
        @param proto: Prototype flag for non-header file use.
        """
        format_func = self.formatFun(indent, self.oneLineFun(name, args))
        if proto:
            format_func = format_func.replace(")", ");")

        line_length = 80
        # If there are arg comments add them...
        if self.commentInArgsPresent(args):
            format_func_list = format_func.split("\n")
            # Trim the trailing spaces from the function name.
            format_func_name = format_func_list[0].strip(" (") + "( \n"
            # Scan arg type_name and comments for maximums.
            comment_max = max([len(x[2]) for x in args])
            type_max = max([len(x.strip()) for x in format_func_list[1:]])
            # Set where to put them.
            comment_list = list(map(self.formComment, [x[2] for x in args]))

            # cpos is comment position from left on line
            # apos is argument position from left on line
            cpos = line_length - comment_max
            apos = line_length - (type_max + 1 + comment_max)
            # always indent args 8 or more spaces
            if apos < 8:
                apos = 8
            apad = apos * " "
            # always make sure there is a space between args and comment
            if cpos <= apos + type_max + 1:
                cpos = apos + type_max + 1

            # place args and comments and put together the string.
            func_arg_list = []
            for i, a in enumerate(format_func_list[1:]):
                pad = (cpos - (apos + len(a.strip()))) * " "
                str = (
                    apad + a.strip() + pad + comment_list[i] + "\n"
                )  #  + pad + comment_list[i]
                func_arg_list.append(str)
            #
            # print 'Last arg: ',func_arg_list[-1].replace(') ',');')
            # TODO: add switch so a ; is inserted - end of last arg after ).
            func_arg_string = ""
            format_func = (
                format_func_name + func_arg_string.join(func_arg_list)
            ).strip("\n")
        return format_func

    ##########################################
    # Function Name or Declaration Formatting
    # from legacy gen_msg.py
    ##########################################

    def formatFun(self, indent, one_line):
        """
        Take a one line function prototype
        and format it to fit within 78 char
        line.
        """
        ddt = 0
        str_len = len(one_line)

        if (indent + str_len) < 78:
            return one_line

        l_paren = one_line.find("(")
        if l_paren == -1:
            PRINT.info(
                "ERROR: No left paren in function name passed to formatFun: %s."
                % one_line
            )
            raise ValueError("No left paren in function name passed to formatFun.")

        two_chunks = one_line.split("(")
        if len(two_chunks) != 2:
            PRINT.info(
                "ERROR: Too many left parens in name passed to formatFun: %s" % one_line
            )
            raise ValueError("Too many left parens in name passed to formatFun.")

        type_and_name = two_chunks[0]
        args = two_chunks[1]

        self._dlog(ddt, "type_and_name=%s" % type_and_name)
        self._dlog(ddt, "         args=%s" % args)

        # NOTE we split args on commas, which means the last arg will contain ")"
        #
        arg_list_untrimmed = args.split(",")
        self._dlog(ddt, "arg_list_untrimmed=%s" % arg_list_untrimmed)
        num_args = len(arg_list_untrimmed)

        # Trim leading and trailing blanks
        #
        arg_list = [x.strip() for x in arg_list_untrimmed]
        self._dlog(ddt, "arg_list=%s" % arg_list)
        max_arg_len = max(list(map(len, arg_list)))
        type_and_name_len = len(type_and_name)

        if (indent + type_and_name_len + 2 + max_arg_len + 2) <= 78:
            # This fits:
            # name( arg,
            #       arg );

            # pad is a string of blanks: its the indent for subsequent lines
            #
            # if indent == 3:
            #    pad_len = type_and_name_len + 2
            #    pad     = " * " + (pad_len * " ")
            # else:
            pad_len = indent + type_and_name_len + 2
            pad = pad_len * " "
            # Build up formatted lines
            #
            formatted_line = type_and_name + "(\n"
            formatted_line += pad + arg_list[0] + ","
            self._dlog(ddt, "A formatted_line=\n%s" % formatted_line)

            for cnt in range(1, num_args - 1):
                nxt = arg_list[cnt]
                formatted_line = formatted_line + "\n" + pad + nxt + ","
                self._dlog(ddt, "A formatted_line=\n%s" % formatted_line)

            formatted_line = formatted_line + "\n" + pad + arg_list[-1]
            self._dlog(ddt, "A formatted_line=\n%s" % formatted_line)
        else:
            # Each argument goes on a new line, and we can't align with the lParen.
            # First line is just function name, subsequent lines are just args.
            # Each arg is aligned, such that they are as far to right as possible.
            #
            # if indent == 3:
            #    pad_len = 75 - max_arg_len
            #    pad     = " *" + (" " * pad_len)
            # else:
            pad_len = 76 - max_arg_len
            pad = " " * pad_len
            # Build up list of lines
            #
            formatted_line = type_and_name + "("
            self._dlog(ddt, "B formatted_line=\n%s" % formatted_line)
            for cnt in range(0, num_args - 1):
                nxt = arg_list[cnt]
                formatted_line = formatted_line + "\n" + pad + nxt + ","
                self._dlog(ddt, "B formattedLine=\n%s" % formatted_line)

            formatted_line = formatted_line + "\n" + pad + arg_list[-1]
            self._dlog(ddt, "A formatted_line=\n%s" % formatted_line)

        return formatted_line

    def argStringAlign(self, type_list, arg_list, pad_spaces=4):
        """
        Method to align generic type/argument
        sets found in struct, typedefs and unions.

        Run this function again for alignment of
        comments on the end.
        @param type_list: list of argument type names.
        @param arg_list:  list of arguments corresponding to type names.
        @return str_list: a list of strings with args aligned.
        """

        str_list = []

        if len(type_list) > 0:

            max_type_length = max(list(map(len, type_list)))

            for i, type in enumerate(type_list):
                pad = (max_type_length + pad_spaces) - len(type)
                str = type + pad * " " + arg_list[i]
                str_list.append(str)

        return str_list

    def typeValue(self, arg):
        """
        Function to return type as a value.
        """
        if arg[-1] == "*":
            return arg[:-1]
        else:
            return arg

    def getPassByPointer(self, arg):
        # EGB pass by pointer: This is to allow pointers to be passed directly via IPC
        if len(arg) == 7:
            # 7 is the case for a properly parsed iface
            value = arg[6]
            # make sure it really came from an iface
            if isinstance(value, bool):
                pass_by_pointer = value
            else:
                return False
        else:
            pass_by_pointer = False
        return pass_by_pointer

    def argTypedefStringAlign(self, args):
        """
        Method to align the type/argument/comment
        combination that appears within typedefs.
        @param args: list of tuple args from xml.
        """

        type_list = []
        arg_list = []
        comment_list = []

        for arg in args:
            # EGB pass by pointer: This is to allow pointers to be passed directly via IPC
            pass_by_pointer = self.getPassByPointer(arg)
            if pass_by_pointer:
                type_list.append(self.typeValue(arg[1].strip()) + " * ")
            else:
                type_list.append(self.typeValue(arg[1].strip()))

            if arg[3] == "":
                arg_list.append(arg[0].strip() + ";")
            else:
                arg_list.append("{}[{}];".format(arg[0].strip(), arg[3].strip()))

            comment_list.append(self.formComment(arg[2].strip()))

        str = self.argStringAlign(
            type_list, arg_list, int(self.__config.get("ipc", "type_arg_spaces"))
        )

        ## STEVE: Removing the comment here to improve some of the
        ## code readability. At times, these comments are truncated
        ## to where they are only partially useful.
        ##
        #        str = self.argStringAlign(str, comment_list, int(self.__config.get('ipc','arg_comment_spaces')))

        return str

    #
    # These methods are added to provide multi-thread naming convention
    # within a module.
    #
    def subThreadTest(self, mod_id):
        """
        Return True if sub-task else return False if main task.
        """
        return mod_id != self.subThreadDir(mod_id)

    def subThreadDir(self, module_id):
        """
        Return the module directory for single thread and multi-thread modules.
        """
        mod_id_list = module_id.split("_")
        if len(mod_id_list) == 1 or len(mod_id_list) == 2:
            mod_id_dir = mod_id_list[0]
        else:
            str = (
                "ERROR: Internal module id must be of form module_subtask or module (%s)"
                % (module_id)
            )
            PRINT.info(str)
            raise ValueError(str)
        return mod_id_dir

    def subThreadModuleFirstCap(self, mod_id):
        """
        Return the module id and subtask with first capital letter
        and no '_' in string form.
        """
        mod_id_list = mod_id.strip("_").split("_")
        mod_id_cap = [x[0].upper() + x[1:] for x in mod_id_list]
        # size of mod_id list error in subThreadDir method.
        mod_id_cap_str = "".join(mod_id_cap)
        return mod_id_cap_str

    def subThreadInstanceFirstArg(self, mod_id, args, context_list):
        """
        Prepend instance id enum arg to args and return new list.
        """
        # if context_list is None:
        #    return args

        if self.subThreadTest(mod_id) and len(context_list) > 0:
            instance_enum = self.subThreadModuleFirstCap(mod_id) + "AcInstanceId"
            d = self.subThreadDir(mod_id)
            file = os.path.join(d, mod_id + "_ac_pub.h")
            arg_comment = (
                "This argument specifies which %s subtask will receive the command."
                % (mod_id)
            )
            id = (
                "id",
                instance_enum,
                arg_comment,
                "",
                "",
                [],
                ["16", instance_enum, file],
            )
            args = [id] + args

        return args

    def subThreadInstanceRangeList(self, mod_id, args, context_list):
        """
        Return the min,max range tuple for the instance ENUM.
        """
        module_id_dir = self.subThreadDir(mod_id)
        # Handle context only if it is a sub_task here.
        if mod_id != module_id_dir:
            context_id_list = [x.context_id_name for x in context_list]
            module_id_first_cap = self.subThreadModuleFirstCap(mod_id)
            module_id_caps = module_id_first_cap.upper()
            first_value = module_id_caps + "_AC_INST0"
            l = len(context_id_list) - 1
            last_value = module_id_caps + "_AC_INST" + ("%d" % l)
            first_enum_range = [((0, first_value), (l, last_value))]
            enum_range_list = [first_enum_range]
        else:
            enum_range_list = [None]
        return enum_range_list

    #
    # End of multi-thread naming support.
    #
    def argFilter(self, module, args, name="foo"):
        """
        Filter of any STRING or SCLK fsw_type argument.
        Changes the STRING to a U8 for use as U8 array.
        Changes the SCLK to ModArgSclk struct type of U32, U16.
        """
        new_args = []
        for arg in args:
            id, type, comment, array_max, array_len, range_list, enum_type_list = arg[
                0:7
            ]

            if type == "STRING":
                id2 = id + "_size"
                array_len = id2
                new_args.append(
                    (
                        id,
                        "U8",
                        comment,
                        array_max,
                        array_len,
                        range_list,
                        enum_type_list,
                    )
                )

                comment = "Size of string argument %s" % id
                new_args.append((id2, "U16", comment, "", "", [], []))

            elif type == "SCLK":
                type = "TimSpacecraftTime"
                new_args.append(
                    (
                        id,
                        type,
                        comment,
                        array_max,
                        array_len,
                        range_list,
                        enum_type_list,
                    )
                )

            elif type == "REPEAT":

                type = self.msgTypedefName(module, name, "")
                array_max = range_list[0][-1]
                array_len = id + "_size"
                enum_type_list = []

                new_args.append(
                    (
                        id,
                        type,
                        comment,
                        array_max,
                        array_len,
                        range_list,
                        enum_type_list,
                    )
                )
                # print new_args[-1]

                id2 = array_len
                comment = "Size of repeat argument %s array" % id
                new_args.append((id2, "U16", comment, "", "", [], []))
                # print (id, type, comment, array_max, array_len, range_list, enum_type_list)

            else:
                new_args.append(arg)

        return new_args
