import re
from datetime import datetime, timedelta

from fprime_gds.common.data_types import exceptions as gseExceptions
from fprime_gds.common.models.common.command import Descriptor


class SeqFileParser:
    def parse(self, filename):
        """
        Generator that parses an input sequence file and returns a tuple
        for each valid line of the sequence file.
        @param seqfile: A sequence file name (usually a .seq extension)
        @return A list of tuples:
            (lineNumber, descriptor, seconds, useconds, mnemonic, arguments)
        """

        def subQuoted(f, string):
            """
            Run a substitution function on only substrings within a string that are surrounded
            by single or double quotes
            @param f: a string substitution acting on matchobjs
            @param string: the string to perform the substitution on
            @return the substituted string
            """
            s = re.sub(r'"[^"]*"', f, string)
            return re.sub(r"'[^']*'", f, s)

        def removeTrailingComments(string):
            """
            Remove any trailing comments (preceded by ';') in a string
            @param string: the string to perform comment removal on
            @return the string without trailing comments
            """

            def replaceSemis(matchobj):
                return matchobj.group(0).replace(";", " ")

            # ignore all semicolons in quotes:
            s = subQuoted(replaceSemis, string)
            s = subQuoted(replaceSemis, s)
            # get index of first semicolon, and return everything before it:
            if ";" in s:
                index = s.index(";")
                return string[:index]
            # return original string if no semicolon found:
            return string

        def splitString(string):
            """
            Split a string with ' ' or ',' as a delimiter. Ignore any delimiters
            found within quoted substrings.
            @param string: the string to perform the split on
            @return a list representing the split string
            """

            def replaceSpacesAndCommas(matchobj):
                s = re.sub(r"\s", "_", matchobj.group(0))
                s = re.sub(r"\,", "_", s)
                return s

            # ignore all spaces in quotes:
            s = subQuoted(replaceSpacesAndCommas, string)
            # replace all commas with spaces, since either can be a delimiter:
            s = s.replace(",", " ")
            # get the split indices of the modified string:
            indices = [(m.start(), m.end()) for m in re.finditer(r"\S+", s)]
            toReturn = []
            for start, end in indices:
                toReturn.append(string[start:end])
            return toReturn

        def parseArgs(args):
            """
            Turn .seq command argument list into their appropriate python types
            @param args: a list of parsed arguments
            @return a list of arguments as native python types
            """

            def parseArg(arg):
                # See if argument is a string, if so remove the quotes and return it:
                if (arg[0] == '"' and arg[-1] == '"') or (
                    arg[0] == "'" and arg[-1] == "'"
                ):
                    return arg[1:-1]
                # If the string contains a "." assume that it is a float:
                elif "." in arg:
                    return float(arg)
                elif arg == "True" or arg == "true" or arg == "TRUE":
                    return True
                elif arg == "False" or arg == "false" or arg == "FALSE":
                    return False
                else:
                    try:
                        # See if it translates to an integer:
                        return int(arg, 0)
                    except ValueError:
                        try:
                            # See if it translates to a float:
                            return float(arg)
                        except ValueError:
                            # Otherwise it is an enum type:
                            return str(arg)

            return list(map(parseArg, args))

        def parseTime(lineNumber, time):
            """
            Parse a time string and return the command descriptor, seconds, and useconds of the time string
            @param lineNumber: the current line number where the time string was parsed
            @param time: the time string to parse
            @return a tuple (descriptor, seconds, useconds)
            """

            def parseTimeStringOption(timeStr, timeFmts):
                """
                Parse a time string by trying to use different time formats, until one succeeds
                @param timeStr: the time string
                @param timeFmts: the time format used to parse the string
                @return the datetime object containing the parsed string
                """

                def parseTimeString(timeFmt):
                    try:
                        return datetime.strptime(timeStr, timeFmt)
                    except ValueError:
                        return None

                for fmt in timeFmts:
                    dt = parseTimeString(fmt)
                    if dt:
                        return dt
                raise BaseException

            def parseRelative(timeStr):
                """
                Parse a relative time string
                @param timeStr: the time string
                @return the datetime object containing the parsed string
                """
                options = ["%H:%M:%S.%f", "%H:%M:%S"]
                return parseTimeStringOption(timeStr, options)

            def parseAbsolute(timeStr):
                """
                Parse an absolute time string
                @param timeStr: the time string
                @return the datetime object containing the parsed string
                """
                options = ["%Y-%jT%H:%M:%S.%f", "%Y-%jT%H:%M:%S"]
                return parseTimeStringOption(timeStr, options)

            descriptor = None
            d = time[0]
            t = time[1:]
            if d == "R":
                descriptor = Descriptor.RELATIVE
                dt = parseRelative(t)
                delta = timedelta(
                    hours=dt.hour,
                    minutes=dt.minute,
                    seconds=dt.second,
                    microseconds=dt.microsecond,
                ).total_seconds()
            elif d == "A":
                descriptor = Descriptor.ABSOLUTE
                dt = parseAbsolute(t)
                # See if timezone was specified. If not, use UTC
                if dt.tzinfo is not None:
                    print("Using timezone %s" % dt.tzinfo.tzname())
                    epoch = datetime.fromtimestamp(0, dt.tzinfo)
                else:
                    print("Using UTC timezone")
                    epoch = datetime.utcfromtimestamp(0)
                delta = (dt - epoch).total_seconds()
            else:
                raise gseExceptions.GseControllerParsingException(
                    "Line %d: %s"
                    % (
                        lineNumber + 1,
                        "Invalid time descriptor '"
                        + d
                        + "' found. Descriptor should either be 'A' for absolute times or 'R' for relative times",
                    )
                )
            seconds = int(delta)
            useconds = int((delta - seconds) * 1000000)
            return descriptor, seconds, useconds

        # Open the sequence file and parse each line:
        with open(filename) as inputFile:
            for i, line in enumerate(inputFile):
                line = line.strip()
                # ignore blank lines and comments
                if line and line[0] != ";":
                    line = removeTrailingComments(line)
                    line = splitString(line)
                    length = len(line)
                    if length < 2:
                        raise gseExceptions.GseControllerParsingException(
                            "Line %d: %s"
                            % (
                                i + 1,
                                "Each line must contain a minimum of two fields, time and command mnemonic\n",
                            )
                        )
                    else:
                        try:
                            descriptor, seconds, useconds = parseTime(i, line[0])
                        except:
                            raise gseExceptions.GseControllerParsingException(
                                "Line %d: %s"
                                % (i + 1, "Encountered syntax error parsing timestamp")
                            )
                        mnemonic = line[1]
                        args = []
                        if length > 2:
                            args = line[2:]
                            try:
                                args = parseArgs(args)
                            except:
                                raise gseExceptions.GseControllerParsingException(
                                    "Line %d: %s"
                                    % (
                                        i + 1,
                                        "Encountered syntax error parsing arguments",
                                    )
                                )
                    yield i, descriptor, seconds, useconds, mnemonic, args
