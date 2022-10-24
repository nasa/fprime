# module pyparsing.py
#
# Copyright (c) 2003-2006  Paul T. McGuire
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# from __future__ import generators

__doc__ = """
pyparsing module - Classes and methods to define and execute parsing grammars

The pyparsing module is an alternative approach to creating and executing simple grammars,
vs. the traditional lex/yacc approach, or the use of regular expressions.  With pyparsing, you
don't need to learn a new syntax for defining grammars or matching expressions - the parsing module
provides a library of classes that you use to construct the grammar directly in Python.

Here is a program to parse "Hello, World!" (or any greeting of the form "<salutation>, <addressee>!")::

    from pyparsing import Word, alphas

    # define grammar of a greeting
    greet = Word( alphas ) + "," + Word( alphas ) + "!"

    hello = "Hello, World!"
    print hello, "->", greet.parseString( hello )

The program outputs the following::

    Hello, World! -> ['Hello', ',', 'World', '!']

The Python representation of the grammar is quite readable, owing to the self-explanatory
class names, and the use of '+', '|' and '^' operators.

The parsed results returned from parseString() can be accessed as a nested list, a dictionary, or an
object with named attributes.

The pyparsing module handles some of the problems that are typically vexing when writing text parsers:
 - extra or missing whitespace (the above program will also handle "Hello,World!", "Hello  ,  World  !", etc.)
 - quoted strings
 - embedded comments
"""
__version__ = "1.4.5"
__versionTime__ = "16 December 2006 07:20"
__author__ = "Paul McGuire <ptmcg@users.sourceforge.net>"

import copy
import re
import sre_constants
import string
import sys
import warnings
import xml.sax.saxutils

# ~ sys.stderr.write( "testing pyparsing module, version %s, %s\n" % (__version__,__versionTime__ ) )


def _ustr(obj):
    """Drop-in replacement for str(obj) that tries to be Unicode friendly. It first tries
    str(obj). If that fails with a UnicodeEncodeError, then it tries unicode(obj). It
    then < returns the unicode object | encodes it with the default encoding | ... >.
    """
    try:
        # If this works, then _ustr(obj) has the same behavior as str(obj), so
        # it won't break any existing code.
        return str(obj)

    except UnicodeEncodeError:
        # The Python docs (http://docs.python.org/ref/customization.html#l2h-182)
        # state that "The return value must be a string object". However, does a
        # unicode object (being a subclass of basestring) count as a "string
        # object"?
        # If so, then return a unicode object:
        return str(obj)
        # Else encode it... but how? There are many choices... :)
        # Replace unprintables with escape codes?
        # return unicode(obj).encode(sys.getdefaultencoding(), 'backslashreplace_errors')
        # Replace unprintables with question marks?
        # return unicode(obj).encode(sys.getdefaultencoding(), 'replace')
        # ...


def _str2dict(strg):
    return {c: 0 for c in strg}
    # ~ return set( [c for c in strg] )


class _Constants:
    pass


alphas = string.ascii_letters
nums = string.digits
hexnums = nums + "ABCDEFabcdef"
alphanums = alphas + nums


class ParseBaseException(Exception):
    """base exception class for all parsing runtime exceptions"""

    __slots__ = ("loc", "msg", "pstr", "parserElement")
    # Performance tuning: we construct a *lot* of these, so keep this
    # constructor as small and fast as possible
    def __init__(self, pstr, loc, msg, elem=None):
        self.loc = loc
        self.msg = msg
        self.pstr = pstr
        self.parserElement = elem

    def __getattr__(self, aname):
        """supported attributes by name are:
        - lineno - returns the line number of the exception text
        - col - returns the column number of the exception text
        - line - returns the line containing the exception text
        """
        if aname == "lineno":
            return lineno(self.loc, self.pstr)
        elif aname in ("col", "column"):
            return col(self.loc, self.pstr)
        elif aname == "line":
            return line(self.loc, self.pstr)
        else:
            raise AttributeError(aname)

    def __str__(self):
        return "%s (at char %d), (line:%d, col:%d)" % (
            self.msg,
            self.loc,
            self.lineno,
            self.column,
        )

    def __repr__(self):
        return _ustr(self)

    def markInputline(self, markerString=">!<"):
        """Extracts the exception line from the input string, and marks
        the location of the exception with a special symbol.
        """
        line_str = self.line
        line_column = self.column - 1
        if markerString:
            line_str = "".join(
                [line_str[:line_column], markerString, line_str[line_column:]]
            )
        return line_str.strip()


class ParseException(ParseBaseException):
    """exception thrown when parse expressions don't match class"""

    """supported attributes by name are:
        - lineno - returns the line number of the exception text
        - col - returns the column number of the exception text
        - line - returns the line containing the exception text
    """


class ParseFatalException(ParseBaseException):
    """user-throwable exception thrown when inconsistent parse content
    is found; stops all parsing immediately"""


class ReparseException(ParseBaseException):
    def __init_(self, newstring, restartLoc):
        self.newParseText = newstring
        self.reparseLoc = restartLoc


class RecursiveGrammarException(Exception):
    """exception thrown by validate() if the grammar could be improperly recursive"""

    def __init__(self, parseElementList):
        self.parseElementTrace = parseElementList

    def __str__(self):
        return "RecursiveGrammarException: %s" % self.parseElementTrace


class ParseResults:
    """Structured parse results, to provide multiple means of access to the parsed data:
    - as a list (len(results))
    - by list index (results[0], results[1], etc.)
    - by attribute (results.<resultsName>)
    """

    __slots__ = (
        "__toklist",
        "__tokdict",
        "__doinit",
        "__name",
        "__parent",
        "__accumNames",
    )

    def __new__(cls, toklist, name=None, asList=True, modal=True):
        if isinstance(toklist, cls):
            return toklist
        retobj = object.__new__(cls)
        retobj.__doinit = True
        return retobj

    # Performance tuning: we construct a *lot* of these, so keep this
    # constructor as small and fast as possible
    def __init__(self, toklist, name=None, asList=True, modal=True):
        if self.__doinit:
            self.__doinit = False
            self.__name = None
            self.__parent = None
            self.__accumNames = {}
            if isinstance(toklist, list):
                self.__toklist = toklist[:]
            else:
                self.__toklist = [toklist]
            self.__tokdict = {}

        # this line is related to debugging the asXML bug
        # ~ asList = False

        if name:
            if not modal:
                self.__accumNames[name] = 0
            if isinstance(name, int):
                name = _ustr(
                    name
                )  # will always return a str, but use _ustr for consistency
            self.__name = name
            if not toklist in (None, "", []):
                if isinstance(toklist, str):
                    toklist = [toklist]
                if asList:
                    if isinstance(toklist, ParseResults):
                        self[name] = (toklist.copy(), -1)
                    else:
                        self[name] = (ParseResults(toklist[0]), -1)
                    self[name].__name = name
                else:
                    try:
                        self[name] = toklist[0]
                    except (KeyError, TypeError):
                        self[name] = toklist

    def __getitem__(self, i):
        if isinstance(i, (int, slice)):
            return self.__toklist[i]
        else:
            if i not in self.__accumNames:
                return self.__tokdict[i][-1][0]
            else:
                return ParseResults([v[0] for v in self.__tokdict[i]])

    def __setitem__(self, k, v):
        if isinstance(v, tuple):
            self.__tokdict[k] = self.__tokdict.get(k, list()) + [v]
            sub = v[0]
        elif isinstance(k, int):
            self.__toklist[k] = v
            sub = v
        else:
            self.__tokdict[k] = self.__tokdict.get(k, list()) + [(v, 0)]
            sub = v
        if isinstance(sub, ParseResults):
            sub.__parent = self

    def __delitem__(self, i):
        if isinstance(i, (int, slice)):
            del self.__toklist[i]
        else:
            del self._tokdict[i]

    def __contains__(self, k):
        return k in self.__tokdict

    def __len__(self):
        return len(self.__toklist)

    def __bool__(self):
        return len(self.__toklist) > 0

    def __iter__(self):
        return iter(self.__toklist)

    def keys(self):
        """Returns all named result keys."""
        return list(self.__tokdict.keys())

    def items(self):
        """Returns all named result keys and values as a list of tuples."""
        return [(k, self[k]) for k in list(self.__tokdict.keys())]

    def values(self):
        """Returns all named result values."""
        return [v[-1][0] for v in list(self.__tokdict.values())]

    def __getattr__(self, name):
        if name not in self.__slots__:
            if name in self.__tokdict:
                if name not in self.__accumNames:
                    return self.__tokdict[name][-1][0]
                else:
                    return ParseResults([v[0] for v in self.__tokdict[name]])
            else:
                return ""
        return None

    def __add__(self, other):
        ret = self.copy()
        ret += other
        return ret

    def __iadd__(self, other):
        if other.__tokdict:
            offset = len(self.__toklist)
            addoffset = lambda a: (a < 0 and offset) or (a + offset)
            otheritems = list(other.__tokdict.items())
            otherdictitems = [
                (k, (v[0], addoffset(v[1]))) for (k, vlist) in otheritems for v in vlist
            ]
            for k, v in otherdictitems:
                self[k] = v
                if isinstance(v[0], ParseResults):
                    v[0].__parent = self
        self.__toklist += other.__toklist
        self.__accumNames.update(other.__accumNames)
        del other
        return self

    def __repr__(self):
        return "({}, {})".format(repr(self.__toklist), repr(self.__tokdict))

    def __str__(self):
        out = "["
        sep = ""
        for i in self.__toklist:
            if isinstance(i, ParseResults):
                out += sep + _ustr(i)
            else:
                out += sep + repr(i)
            sep = ", "
        out += "]"
        return out

    def _asStringList(self, sep=""):
        out = []
        for item in self.__toklist:
            if out and sep:
                out.append(sep)
            if isinstance(item, ParseResults):
                out += item._asStringList()
            else:
                out.append(_ustr(item))
        return out

    def asList(self):
        """Returns the parse results as a nested list of matching tokens, all converted to strings."""
        out = []
        for res in self.__toklist:
            if isinstance(res, ParseResults):
                out.append(res.asList())
            else:
                out.append(res)
        return out

    def asDict(self):
        """Returns the named parse results as dictionary."""
        return dict(list(self.items()))

    def copy(self):
        """Returns a new copy of a ParseResults object."""
        ret = ParseResults(self.__toklist)
        ret.__tokdict = self.__tokdict.copy()
        ret.__parent = self.__parent
        ret.__accumNames.update(self.__accumNames)
        ret.__name = self.__name
        return ret

    def asXML(self, doctag=None, namedItemsOnly=False, indent="", formatted=True):
        """Returns the parse results as XML. Tags are created for tokens and lists that have defined results names."""
        nl = "\n"
        out = []
        namedItems = {
            v[1]: k for (k, vlist) in list(self.__tokdict.items()) for v in vlist
        }
        nextLevelIndent = indent + "  "

        # collapse out indents if formatting is not desired
        if not formatted:
            indent = ""
            nextLevelIndent = ""
            nl = ""

        selfTag = None
        if doctag is not None:
            selfTag = doctag
        else:
            if self.__name:
                selfTag = self.__name

        if not selfTag:
            if namedItemsOnly:
                return ""
            else:
                selfTag = "ITEM"

        out += [nl, indent, "<", selfTag, ">"]

        worklist = self.__toklist
        for i, res in enumerate(worklist):
            if isinstance(res, ParseResults):
                if i in namedItems:
                    out += [
                        res.asXML(
                            namedItems[i],
                            namedItemsOnly and doctag is None,
                            nextLevelIndent,
                            formatted,
                        )
                    ]
                else:
                    out += [
                        res.asXML(
                            None,
                            namedItemsOnly and doctag is None,
                            nextLevelIndent,
                            formatted,
                        )
                    ]
            else:
                # individual token, see if there is a name for it
                resTag = None
                if i in namedItems:
                    resTag = namedItems[i]
                if not resTag:
                    if namedItemsOnly:
                        continue
                    else:
                        resTag = "ITEM"
                xmlBodyText = xml.sax.saxutils.escape(_ustr(res))
                out += [
                    nl,
                    nextLevelIndent,
                    "<",
                    resTag,
                    ">",
                    xmlBodyText,
                    "</",
                    resTag,
                    ">",
                ]

        out += [nl, indent, "</", selfTag, ">"]
        return "".join(out)

    def __lookup(self, sub):
        for k, vlist in list(self.__tokdict.items()):
            for v, loc in vlist:
                if sub is v:
                    return k
        return None

    def getName(self):
        """Returns the results name for this token expression."""
        if self.__name:
            return self.__name
        elif self.__parent:
            par = self.__parent
            if par:
                return par.__lookup(self)
            else:
                return None
        elif (
            len(self) == 1
            and len(self.__tokdict) == 1
            and list(self.__tokdict.values())[0][0][1] in (0, -1)
        ):
            return list(self.__tokdict.keys())[0]
        else:
            return None

    def dump(self, indent="", depth=0):
        """Diagnostic method for listing out the contents of a ParseResults.
        Accepts an optional indent argument so that this string can be embedded
        in a nested display of other data."""
        out = [indent + str(self.asList())]
        keys = list(self.items())
        keys.sort()
        for k, v in keys:
            if out:
                out.append("\n")
            out.append("{}{}- {}: ".format(indent, ("  " * depth), k))
            if isinstance(v, ParseResults):
                if list(v.keys()):
                    # ~ out.append('\n')
                    out.append(v.dump(indent, depth + 1))
                    # ~ out.append('\n')
                else:
                    out.append(str(v))
            else:
                out.append(str(v))
        # ~ out.append('\n')
        return "".join(out)

    # add support for pickle protocol
    def __getstate__(self):
        return (
            self.__toklist,
            (self.__tokdict.copy(), self.__parent, self.__accumNames, self.__name),
        )

    def __setstate__(self, state):
        self.__toklist = state[0]
        self.__tokdict, self.__parent, inAccumNames, self.__name = state[1]
        self.__accumNames = {}
        self.__accumNames.update(inAccumNames)


def col(loc, strg):
    """Returns current column within a string, counting newlines as line separators.
    The first column is number 1.
    """
    return (
        (loc < len(strg) and strg[loc] == "\n") and 1 or loc - strg.rfind("\n", 0, loc)
    )


def lineno(loc, strg):
    """Returns current line number within a string, counting newlines as line separators.
    The first line is number 1.
    """
    return strg.count("\n", 0, loc) + 1


def line(loc, strg):
    """Returns the line of text containing loc within a string, counting newlines as line separators."""
    lastCR = strg.rfind("\n", 0, loc)
    nextCR = strg.find("\n", loc)
    if nextCR > 0:
        return strg[lastCR + 1 : nextCR]
    else:
        return strg[lastCR + 1 :]


def _defaultStartDebugAction(instring, loc, expr):
    print(
        "Match",
        expr,
        "at loc",
        loc,
        "(%d,%d)" % (lineno(loc, instring), col(loc, instring)),
    )


def _defaultSuccessDebugAction(instring, startloc, endloc, expr, toks):
    print("Matched", expr, "->", toks.asList())


def _defaultExceptionDebugAction(instring, loc, expr, exc):
    print("Exception raised:", exc)


def nullDebugAction(*args):
    """'Do-nothing' debug action, to suppress debugging output during parsing."""


class ParserElement:
    """Abstract base level parser element class."""

    DEFAULT_WHITE_CHARS = " \n\t\r"

    def setDefaultWhitespaceChars(chars):
        """Overrides the default whitespace chars"""
        ParserElement.DEFAULT_WHITE_CHARS = chars

    setDefaultWhitespaceChars = staticmethod(setDefaultWhitespaceChars)

    def __init__(self, savelist=False):
        self.parseAction = []
        self.failAction = None
        # ~ self.name = "<unknown>"  # don't define self.name, let subclasses try/except upcall
        self.strRepr = None
        self.resultsName = None
        self.saveAsList = savelist
        self.skipWhitespace = True
        self.whiteChars = ParserElement.DEFAULT_WHITE_CHARS
        self.copyDefaultWhiteChars = True
        self.mayReturnEmpty = False
        self.keepTabs = False
        self.ignoreExprs = []
        self.debug = False
        self.streamlined = False
        self.mayIndexError = True
        self.errmsg = ""
        self.modalResults = True
        self.debugActions = (None, None, None)
        self.re = None

    def copy(self):
        """Make a copy of this ParserElement.  Useful for defining different parse actions
        for the same parsing pattern, using copies of the original parse element."""
        cpy = copy.copy(self)
        cpy.parseAction = self.parseAction[:]
        cpy.ignoreExprs = self.ignoreExprs[:]
        if self.copyDefaultWhiteChars:
            cpy.whiteChars = ParserElement.DEFAULT_WHITE_CHARS
        return cpy

    def setName(self, name):
        """Define name for this expression, for use in debugging."""
        self.name = name
        self.errmsg = "Expected " + self.name
        return self

    def setResultsName(self, name, listAllMatches=False):
        """Define name for referencing matching tokens as a nested attribute
        of the returned parse results.
        NOTE: this returns a *copy* of the original ParserElement object;
        this is so that the client can define a basic element, such as an
        integer, and reference it in multiple places with different names.
        """
        newself = self.copy()
        newself.resultsName = name
        newself.modalResults = not listAllMatches
        return newself

    def normalizeParseActionArgs(f):
        """Internal method used to decorate parse actions that take fewer than 3 arguments,
        so that all parse actions can be called as f(s,l,t)."""
        STAR_ARGS = 4

        try:
            restore = None
            if isinstance(f, type):
                restore = f
                f = f.__init__
            if f.__code__.co_flags & STAR_ARGS:
                return f
            numargs = f.__code__.co_argcount
            if hasattr(f, "im_self"):
                numargs -= 1
            if restore:
                f = restore
        except AttributeError:
            try:
                # not a function, must be a callable object, get info from the
                # im_func binding of its bound __call__ method
                if f.__call__.__func__.__code__.co_flags & STAR_ARGS:
                    return f
                numargs = f.__call__.__func__.__code__.co_argcount
                if hasattr(f.__call__, "im_self"):
                    numargs -= 1
            except AttributeError:
                # not a bound method, get info directly from __call__ method
                if f.__call__.__code__.co_flags & STAR_ARGS:
                    return f
                numargs = f.__call__.__code__.co_argcount
                if hasattr(f.__call__, "im_self"):
                    numargs -= 1

        # ~ print "adding function %s with %d args" % (f.func_name,numargs)
        if numargs == 3:
            return f
        else:
            if numargs == 2:

                def tmp(s, l, t):
                    return f(l, t)

            elif numargs == 1:

                def tmp(s, l, t):
                    return f(t)

            else:  # ~ numargs == 0:

                def tmp(s, l, t):
                    return f()

            return tmp

    normalizeParseActionArgs = staticmethod(normalizeParseActionArgs)

    def setParseAction(self, *fns):
        """Define action to perform when successfully matching parse element definition.
        Parse action fn is a callable method with 0-3 arguments, called as fn(s,loc,toks),
        fn(loc,toks), fn(toks), or just fn(), where:
         - s   = the original string being parsed
         - loc = the location of the matching substring
         - toks = a list of the matched tokens, packaged as a ParseResults object
        If the functions in fns modify the tokens, they can return them as the return
        value from fn, and the modified list of tokens will replace the original.
        Otherwise, fn does not need to return any value."""
        self.parseAction = list(map(self.normalizeParseActionArgs, list(fns)))
        return self

    def addParseAction(self, *fns):
        """Add parse action to expression's list of parse actions. See setParseAction_."""
        self.parseAction += list(map(self.normalizeParseActionArgs, list(fns)))
        return self

    def setFailAction(self, fn):
        """Define action to perform if parsing fails at this expression.
        Fail action fn is a callable function that takes the arguments
        fn(s,loc,expr,err) where:
         - s = string being parsed
         - loc = location where expression match was attempted and failed
         - expr = the parse expression that failed
         - err = the exception thrown
        The function returns no value.  It may throw ParseFatalException
        if it is desired to stop parsing immediately."""
        self.failAction = fn
        return self

    def skipIgnorables(self, instring, loc):
        exprsFound = True
        while exprsFound:
            exprsFound = False
            for e in self.ignoreExprs:
                try:
                    while 1:
                        loc, dummy = e._parse(instring, loc)
                        exprsFound = True
                except ParseException:
                    pass
        return loc

    def preParse(self, instring, loc):
        if self.ignoreExprs:
            loc = self.skipIgnorables(instring, loc)

        if self.skipWhitespace:
            wt = self.whiteChars
            instrlen = len(instring)
            while loc < instrlen and instring[loc] in wt:
                loc += 1

        return loc

    def parseImpl(self, instring, loc, doActions=True):
        return loc, []

    def postParse(self, instring, loc, tokenlist):
        return tokenlist

    # ~ @profile
    def _parseNoCache(self, instring, loc, doActions=True, callPreParse=True):
        debugging = self.debug  # and doActions )

        if debugging or self.failAction:
            # ~ print "Match",self,"at loc",loc,"(%d,%d)" % ( lineno(loc,instring), col(loc,instring) )
            if self.debugActions[0]:
                self.debugActions[0](instring, loc, self)
            if callPreParse:
                preloc = self.preParse(instring, loc)
            else:
                preloc = loc
            tokensStart = loc
            try:
                try:
                    loc, tokens = self.parseImpl(instring, preloc, doActions)
                except IndexError:
                    raise ParseException(instring, len(instring), self.errmsg, self)
            # ~ except ReparseException as retryEx:
            # ~ pass
            except ParseException as err:
                # ~ print "Exception raised:", err
                if self.debugActions[2]:
                    self.debugActions[2](instring, tokensStart, self, err)
                if self.failAction:
                    self.failAction(instring, tokensStart, self, err)
                raise
        else:
            if callPreParse:
                preloc = self.preParse(instring, loc)
            else:
                preloc = loc
            tokensStart = loc
            if self.mayIndexError or loc >= len(instring):
                try:
                    loc, tokens = self.parseImpl(instring, preloc, doActions)
                except IndexError:
                    raise ParseException(instring, len(instring), self.errmsg, self)
            else:
                loc, tokens = self.parseImpl(instring, preloc, doActions)

        tokens = self.postParse(instring, loc, tokens)

        retTokens = ParseResults(
            tokens, self.resultsName, asList=self.saveAsList, modal=self.modalResults
        )
        if self.parseAction and doActions:
            if debugging:
                try:
                    for fn in self.parseAction:
                        tokens = fn(instring, tokensStart, retTokens)
                        if tokens is not None:
                            retTokens = ParseResults(
                                tokens,
                                self.resultsName,
                                asList=self.saveAsList
                                and isinstance(tokens, (ParseResults, list)),
                                modal=self.modalResults,
                            )
                except ParseException as err:
                    # ~ print "Exception raised in user parse action:", err
                    if self.debugActions[2]:
                        self.debugActions[2](instring, tokensStart, self, err)
                    raise
            else:
                for fn in self.parseAction:
                    tokens = fn(instring, tokensStart, retTokens)
                    if tokens is not None:
                        retTokens = ParseResults(
                            tokens,
                            self.resultsName,
                            asList=self.saveAsList
                            and isinstance(tokens, (ParseResults, list)),
                            modal=self.modalResults,
                        )

        if debugging:
            # ~ print "Matched",self,"->",retTokens.asList()
            if self.debugActions[1]:
                self.debugActions[1](instring, tokensStart, loc, self, retTokens)

        return loc, retTokens

    def tryParse(self, instring, loc):
        return self._parse(instring, loc, doActions=False)[0]

    # this method gets repeatedly called during backtracking with the same arguments -
    # we can cache these arguments and save ourselves the trouble of re-parsing the contained expression
    def _parseCache(self, instring, loc, doActions=True, callPreParse=True):
        if doActions and self.parseAction:
            return self._parseNoCache(instring, loc, doActions, callPreParse)
        lookup = (self, instring, loc, callPreParse)
        if lookup in ParserElement._exprArgCache:
            value = ParserElement._exprArgCache[lookup]
            if isinstance(value, Exception):
                if isinstance(value, ParseBaseException):
                    value.loc = loc
                raise value
            return value
        else:
            try:
                ParserElement._exprArgCache[lookup] = value = self._parseNoCache(
                    instring, loc, doActions, callPreParse
                )
                return value
            except ParseBaseException as pe:
                ParserElement._exprArgCache[lookup] = pe
                raise

    _parse = _parseNoCache

    # argument cache for optimizing repeated calls when backtracking through recursive expressions
    _exprArgCache = {}

    def resetCache():
        ParserElement._exprArgCache.clear()

    resetCache = staticmethod(resetCache)

    _packratEnabled = False

    def enablePackrat():
        """Enables "packrat" parsing, which adds memoizing to the parsing logic.
        Repeated parse attempts at the same string location (which happens
        often in many complex grammars) can immediately return a cached value,
        instead of re-executing parsing/validating code.  Memoizing is done of
        both valid results and parsing exceptions.

        This speedup may break existing programs that use parse actions that
        have side-effects.  For this reason, packrat parsing is disabled when
        you first import pyparsing.  To activate the packrat feature, your
        program must call the class method ParserElement.enablePackrat().  If
        your program uses psyco to "compile as you go", you must call
        enablePackrat before calling psyco.full().  If you do not do this,
        Python will crash.  For best results, call enablePackrat() immediately
        after importing pyparsing.
        """
        if not ParserElement._packratEnabled:
            ParserElement._packratEnabled = True
            ParserElement._parse = ParserElement._parseCache

    enablePackrat = staticmethod(enablePackrat)

    def parseString(self, instring):
        """Execute the parse expression with the given string.
        This is the main interface to the client code, once the complete
        expression has been built.
        """
        ParserElement.resetCache()
        if not self.streamlined:
            self.streamline()
            # ~ self.saveAsList = True
        for e in self.ignoreExprs:
            e.streamline()
        if self.keepTabs:
            loc, tokens = self._parse(instring, 0)
        else:
            loc, tokens = self._parse(instring.expandtabs(), 0)
        return tokens

    def scanString(self, instring, maxMatches=sys.maxsize):
        """Scan the input string for expression matches.  Each match will return the
        matching tokens, start location, and end location.  May be called with optional
        maxMatches argument, to clip scanning after 'n' matches are found."""
        if not self.streamlined:
            self.streamline()
        for e in self.ignoreExprs:
            e.streamline()

        if not self.keepTabs:
            instring = instring.expandtabs()
        instrlen = len(instring)
        loc = 0
        preparseFn = self.preParse
        parseFn = self._parse
        ParserElement.resetCache()
        matches = 0
        while loc <= instrlen and matches < maxMatches:
            try:
                preloc = preparseFn(instring, loc)
                nextLoc, tokens = parseFn(instring, preloc, callPreParse=False)
            except ParseException:
                loc = preloc + 1
            else:
                matches += 1
                yield tokens, preloc, nextLoc
                loc = nextLoc

    def transformString(self, instring):
        """Extension to scanString, to modify matching text with modified tokens that may
        be returned from a parse action.  To use transformString, define a grammar and
        attach a parse action to it that modifies the returned token list.
        Invoking transformString() on a target string will then scan for matches,
        and replace the matched text patterns according to the logic in the parse
        action.  transformString() returns the resulting transformed string."""
        out = []
        lastE = 0
        # force preservation of <TAB>s, to minimize unwanted transformation of string, and to
        # keep string locations straight between transformString and scanString
        self.keepTabs = True
        for t, s, e in self.scanString(instring):
            out.append(instring[lastE:s])
            if t:
                if isinstance(t, ParseResults):
                    out += t.asList()
                elif isinstance(t, list):
                    out += t
                else:
                    out.append(t)
            lastE = e
        out.append(instring[lastE:])
        return "".join(out)

    def searchString(self, instring, maxMatches=sys.maxsize):
        """Another extension to scanString, simplifying the access to the tokens found
        to match the given parse expression.  May be called with optional
        maxMatches argument, to clip searching after 'n' matches are found.
        """
        return ParseResults([t for t, s, e in self.scanString(instring, maxMatches)])

    def __add__(self, other):
        """Implementation of + operator - returns And"""
        if isinstance(other, str):
            other = Literal(other)
        if not isinstance(other, ParserElement):
            warnings.warn(
                "Cannot add element of type %s to ParserElement" % type(other),
                SyntaxWarning,
                stacklevel=2,
            )
        return And([self, other])

    def __radd__(self, other):
        """Implementation of += operator"""
        if isinstance(other, str):
            other = Literal(other)
        if not isinstance(other, ParserElement):
            warnings.warn(
                "Cannot add element of type %s to ParserElement" % type(other),
                SyntaxWarning,
                stacklevel=2,
            )
        return other + self

    def __or__(self, other):
        """Implementation of | operator - returns MatchFirst"""
        if isinstance(other, str):
            other = Literal(other)
        if not isinstance(other, ParserElement):
            warnings.warn(
                "Cannot add element of type %s to ParserElement" % type(other),
                SyntaxWarning,
                stacklevel=2,
            )
        return MatchFirst([self, other])

    def __ror__(self, other):
        """Implementation of |= operator"""
        if isinstance(other, str):
            other = Literal(other)
        if not isinstance(other, ParserElement):
            warnings.warn(
                "Cannot add element of type %s to ParserElement" % type(other),
                SyntaxWarning,
                stacklevel=2,
            )
        return other | self

    def __xor__(self, other):
        """Implementation of ^ operator - returns Or"""
        if isinstance(other, str):
            other = Literal(other)
        if not isinstance(other, ParserElement):
            warnings.warn(
                "Cannot add element of type %s to ParserElement" % type(other),
                SyntaxWarning,
                stacklevel=2,
            )
        return Or([self, other])

    def __rxor__(self, other):
        """Implementation of ^= operator"""
        if isinstance(other, str):
            other = Literal(other)
        if not isinstance(other, ParserElement):
            warnings.warn(
                "Cannot add element of type %s to ParserElement" % type(other),
                SyntaxWarning,
                stacklevel=2,
            )
        return other ^ self

    def __and__(self, other):
        """Implementation of & operator - returns Each"""
        if isinstance(other, str):
            other = Literal(other)
        if not isinstance(other, ParserElement):
            warnings.warn(
                "Cannot add element of type %s to ParserElement" % type(other),
                SyntaxWarning,
                stacklevel=2,
            )
        return Each([self, other])

    def __rand__(self, other):
        """Implementation of right-& operator"""
        if isinstance(other, str):
            other = Literal(other)
        if not isinstance(other, ParserElement):
            warnings.warn(
                "Cannot add element of type %s to ParserElement" % type(other),
                SyntaxWarning,
                stacklevel=2,
            )
        return other & self

    def __invert__(self):
        """Implementation of ~ operator - returns NotAny"""
        return NotAny(self)

    def suppress(self):
        """Suppresses the output of this ParserElement; useful to keep punctuation from
        cluttering up returned output.
        """
        return Suppress(self)

    def leaveWhitespace(self):
        """Disables the skipping of whitespace before matching the characters in the
        ParserElement's defined pattern.  This is normally only used internally by
        the pyparsing module, but may be needed in some whitespace-sensitive grammars.
        """
        self.skipWhitespace = False
        return self

    def setWhitespaceChars(self, chars):
        """Overrides the default whitespace chars"""
        self.skipWhitespace = True
        self.whiteChars = chars
        self.copyDefaultWhiteChars = False
        return self

    def parseWithTabs(self):
        """Overrides default behavior to expand <TAB>s to spaces before parsing the input string.
        Must be called before parseString when the input grammar contains elements that
        match <TAB> characters."""
        self.keepTabs = True
        return self

    def ignore(self, other):
        """Define expression to be ignored (e.g., comments) while doing pattern
        matching; may be called repeatedly, to define multiple comment or other
        ignorable patterns.
        """
        if isinstance(other, Suppress):
            if other not in self.ignoreExprs:
                self.ignoreExprs.append(other)
        else:
            self.ignoreExprs.append(Suppress(other))
        return self

    def setDebugActions(self, startAction, successAction, exceptionAction):
        """Enable display of debugging messages while doing pattern matching."""
        self.debugActions = (
            startAction or _defaultStartDebugAction,
            successAction or _defaultSuccessDebugAction,
            exceptionAction or _defaultExceptionDebugAction,
        )
        self.debug = True
        return self

    def setDebug(self, flag=True):
        """Enable display of debugging messages while doing pattern matching."""
        if flag:
            self.setDebugActions(
                _defaultStartDebugAction,
                _defaultSuccessDebugAction,
                _defaultExceptionDebugAction,
            )
        else:
            self.debug = False
        return self

    def __str__(self):
        return self.name

    def __repr__(self):
        return _ustr(self)

    def streamline(self):
        self.streamlined = True
        self.strRepr = None
        return self

    def checkRecursion(self, parseElementList):
        pass

    def validate(self, validateTrace=[]):
        """Check defined expressions for valid structure, check for infinite recursive definitions."""
        self.checkRecursion([])

    def parseFile(self, file_or_filename):
        """Execute the parse expression on the given file or filename.
        If a filename is specified (instead of a file object),
        the entire file is opened, read, and closed before parsing.
        """
        try:
            file_contents = file_or_filename.read()
        except AttributeError:
            f = open(file_or_filename, "rb")
            file_contents = f.read()
            f.close()
        return self.parseString(file_contents)


class Token(ParserElement):
    """Abstract ParserElement subclass, for defining atomic matching patterns."""

    def __init__(self):
        super().__init__(savelist=False)
        self.myException = ParseException("", 0, "", self)

    def setName(self, name):
        s = super().setName(name)
        self.errmsg = "Expected " + self.name
        self.myException.msg = self.errmsg
        return s


class Empty(Token):
    """An empty token, will always match."""

    def __init__(self):
        super().__init__()
        self.name = "Empty"
        self.mayReturnEmpty = True
        self.mayIndexError = False


class NoMatch(Token):
    """A token that will never match."""

    def __init__(self):
        super().__init__()
        self.name = "NoMatch"
        self.mayReturnEmpty = True
        self.mayIndexError = False
        self.errmsg = "Unmatchable token"
        self.myException.msg = self.errmsg

    def parseImpl(self, instring, loc, doActions=True):
        exc = self.myException
        exc.loc = loc
        exc.pstr = instring
        raise exc


class Literal(Token):
    """Token to exactly match a specified string."""

    def __init__(self, matchString):
        super().__init__()
        self.match = matchString
        self.matchLen = len(matchString)
        try:
            self.firstMatchChar = matchString[0]
        except IndexError:
            warnings.warn(
                "null string passed to Literal; use Empty() instead",
                SyntaxWarning,
                stacklevel=2,
            )
            self.__class__ = Empty
        self.name = '"%s"' % self.match
        self.errmsg = "Expected " + self.name
        self.mayReturnEmpty = False
        self.myException.msg = self.errmsg
        self.mayIndexError = False

    # Performance tuning: this routine gets called a *lot*
    # if this is a single character match string  and the first character matches,
    # short-circuit as quickly as possible, and avoid calling startswith
    # ~ @profile
    def parseImpl(self, instring, loc, doActions=True):
        if instring[loc] == self.firstMatchChar and (
            self.matchLen == 1 or instring.startswith(self.match, loc)
        ):
            return loc + self.matchLen, self.match
        # ~ raise ParseException( instring, loc, self.errmsg )
        exc = self.myException
        exc.loc = loc
        exc.pstr = instring
        raise exc


class Keyword(Token):
    """Token to exactly match a specified string as a keyword, that is, it must be
    immediately followed by a non-keyword character.  Compare with Literal::
      Literal("if") will match the leading 'if' in 'ifAndOnlyIf'.
      Keyword("if") will not; it will only match the leading 'if in 'if x=1', or 'if(y==2)'
    Accepts two optional constructor arguments in addition to the keyword string:
    identChars is a string of characters that would be valid identifier characters,
    defaulting to all alphanumerics + "_" and "$"; caseless allows case-insensitive
    matching, default is False.
    """

    DEFAULT_KEYWORD_CHARS = alphanums + "_$"

    def __init__(self, matchString, identChars=DEFAULT_KEYWORD_CHARS, caseless=False):
        super().__init__()
        self.match = matchString
        self.matchLen = len(matchString)
        try:
            self.firstMatchChar = matchString[0]
        except IndexError:
            warnings.warn(
                "null string passed to Keyword; use Empty() instead",
                SyntaxWarning,
                stacklevel=2,
            )
        self.name = '"%s"' % self.match
        self.errmsg = "Expected " + self.name
        self.mayReturnEmpty = False
        self.myException.msg = self.errmsg
        self.mayIndexError = False
        self.caseless = caseless
        if caseless:
            self.caselessmatch = matchString.upper()
            identChars = identChars.upper()
        self.identChars = _str2dict(identChars)

    def parseImpl(self, instring, loc, doActions=True):
        if self.caseless:
            if (
                (instring[loc : loc + self.matchLen].upper() == self.caselessmatch)
                and (
                    loc >= len(instring) - self.matchLen
                    or instring[loc + self.matchLen].upper() not in self.identChars
                )
                and (loc == 0 or instring[loc - 1].upper() not in self.identChars)
            ):
                return loc + self.matchLen, self.match
        else:
            if (
                instring[loc] == self.firstMatchChar
                and (self.matchLen == 1 or instring.startswith(self.match, loc))
                and (
                    loc >= len(instring) - self.matchLen
                    or instring[loc + self.matchLen] not in self.identChars
                )
                and (loc == 0 or instring[loc - 1] not in self.identChars)
            ):
                return loc + self.matchLen, self.match
        # ~ raise ParseException( instring, loc, self.errmsg )
        exc = self.myException
        exc.loc = loc
        exc.pstr = instring
        raise exc

    def copy(self):
        c = super().copy()
        c.identChars = Keyword.DEFAULT_KEYWORD_CHARS
        return c

    def setDefaultKeywordChars(chars):
        """Overrides the default Keyword chars"""
        Keyword.DEFAULT_KEYWORD_CHARS = chars

    setDefaultKeywordChars = staticmethod(setDefaultKeywordChars)


class CaselessLiteral(Literal):
    """Token to match a specified string, ignoring case of letters.
    Note: the matched results will always be in the case of the given
    match string, NOT the case of the input text.
    """

    def __init__(self, matchString):
        super().__init__(matchString.upper())
        # Preserve the defining literal.
        self.returnString = matchString
        self.name = "'%s'" % self.returnString
        self.errmsg = "Expected " + self.name
        self.myException.msg = self.errmsg

    def parseImpl(self, instring, loc, doActions=True):
        if instring[loc : loc + self.matchLen].upper() == self.match:
            return loc + self.matchLen, self.returnString
        # ~ raise ParseException( instring, loc, self.errmsg )
        exc = self.myException
        exc.loc = loc
        exc.pstr = instring
        raise exc


class CaselessKeyword(Keyword):
    def __init__(self, matchString, identChars=Keyword.DEFAULT_KEYWORD_CHARS):
        super().__init__(matchString, identChars, caseless=True)

    def parseImpl(self, instring, loc, doActions=True):
        if (instring[loc : loc + self.matchLen].upper() == self.caselessmatch) and (
            loc >= len(instring) - self.matchLen
            or instring[loc + self.matchLen].upper() not in self.identChars
        ):
            return loc + self.matchLen, self.match
        # ~ raise ParseException( instring, loc, self.errmsg )
        exc = self.myException
        exc.loc = loc
        exc.pstr = instring
        raise exc


class Word(Token):
    """Token for matching words composed of allowed character sets.
    Defined with string containing all allowed initial characters,
    an optional string containing allowed body characters (if omitted,
    defaults to the initial character set), and an optional minimum,
    maximum, and/or exact length.
    """

    def __init__(self, initChars, bodyChars=None, min=1, max=0, exact=0):
        super().__init__()
        self.initCharsOrig = initChars
        self.initChars = _str2dict(initChars)
        if bodyChars:
            self.bodyCharsOrig = bodyChars
            self.bodyChars = _str2dict(bodyChars)
        else:
            self.bodyCharsOrig = initChars
            self.bodyChars = _str2dict(initChars)

        self.maxSpecified = max > 0

        self.minLen = min

        if max > 0:
            self.maxLen = max
        else:
            self.maxLen = sys.maxsize

        if exact > 0:
            self.maxLen = exact
            self.minLen = exact

        self.name = _ustr(self)
        self.errmsg = "Expected " + self.name
        self.myException.msg = self.errmsg
        self.mayIndexError = False

        if " " not in self.initCharsOrig + self.bodyCharsOrig and (
            min == 1 and max == 0 and exact == 0
        ):
            if self.bodyCharsOrig == self.initCharsOrig:
                self.reString = "[%s]+" % _escapeRegexRangeChars(self.initCharsOrig)
            elif len(self.bodyCharsOrig) == 1:
                self.reString = "{}[{}]*".format(
                    re.escape(self.initCharsOrig),
                    _escapeRegexRangeChars(self.bodyCharsOrig),
                )
            else:
                self.reString = "[{}][{}]*".format(
                    _escapeRegexRangeChars(self.initCharsOrig),
                    _escapeRegexRangeChars(self.bodyCharsOrig),
                )
            try:
                self.re = re.compile(self.reString)
            except (re.error, TypeError):
                self.re = None

    def parseImpl(self, instring, loc, doActions=True):
        if self.re:
            result = self.re.match(instring, loc)
            if not result:
                exc = self.myException
                exc.loc = loc
                exc.pstr = instring
                raise exc

            loc = result.end()
            return loc, result.group()

        if not (instring[loc] in self.initChars):
            # ~ raise ParseException( instring, loc, self.errmsg )
            exc = self.myException
            exc.loc = loc
            exc.pstr = instring
            raise exc
        start = loc
        loc += 1
        instrlen = len(instring)
        bodychars = self.bodyChars
        maxloc = start + self.maxLen
        maxloc = min(maxloc, instrlen)
        while loc < maxloc and instring[loc] in bodychars:
            loc += 1

        throwException = False
        if loc - start < self.minLen:
            throwException = True
        if self.maxSpecified and loc < instrlen and instring[loc] in bodychars:
            throwException = True

        if throwException:
            # ~ raise ParseException( instring, loc, self.errmsg )
            exc = self.myException
            exc.loc = loc
            exc.pstr = instring
            raise exc

        return loc, instring[start:loc]

    def __str__(self):
        try:
            return super().__str__()
        except AttributeError:
            pass

        if self.strRepr is None:

            def charsAsStr(s):
                if len(s) > 4:
                    return s[:4] + "..."
                else:
                    return s

            if self.initCharsOrig != self.bodyCharsOrig:
                self.strRepr = "W:({},{})".format(
                    charsAsStr(self.initCharsOrig), charsAsStr(self.bodyCharsOrig)
                )
            else:
                self.strRepr = "W:(%s)" % charsAsStr(self.initCharsOrig)

        return self.strRepr


class Regex(Token):
    """Token for matching strings that match a given regular expression.
    Defined with string specifying the regular expression in a form recognized by the inbuilt Python re module.
    """

    def __init__(self, pattern, flags=0):
        """The parameters pattern and flags are passed to the re.compile() function as-is. See the Python re module for an explanation of the acceptable patterns and flags."""
        super().__init__()

        if len(pattern) == 0:
            warnings.warn(
                "null string passed to Regex; use Empty() instead",
                SyntaxWarning,
                stacklevel=2,
            )

        self.pattern = pattern
        self.flags = flags

        try:
            self.re = re.compile(self.pattern, self.flags)
            self.reString = self.pattern
        except sre_constants.error as e:
            warnings.warn(
                "invalid pattern (%s) passed to Regex" % pattern,
                SyntaxWarning,
                stacklevel=2,
            )
            raise

        self.name = _ustr(self)
        self.errmsg = "Expected " + self.name
        self.myException.msg = self.errmsg
        self.mayIndexError = False
        self.mayReturnEmpty = True

    def parseImpl(self, instring, loc, doActions=True):
        result = self.re.match(instring, loc)
        if not result:
            exc = self.myException
            exc.loc = loc
            exc.pstr = instring
            raise exc

        loc = result.end()
        d = result.groupdict()
        ret = ParseResults(result.group())
        if d:
            for k in list(d.keys()):
                ret[k] = d[k]
        return loc, ret

    def __str__(self):
        try:
            return super().__str__()
        except AttributeError:
            pass

        if self.strRepr is None:
            self.strRepr = "Re:(%s)" % repr(self.pattern)

        return self.strRepr


class QuotedString(Token):
    """Token for matching strings that are delimited by quoting characters."""

    def __init__(
        self,
        quoteChar,
        escChar=None,
        escQuote=None,
        multiline=False,
        unquoteResults=True,
        endQuoteChar=None,
    ):
        """
        Defined with the following parameters:
        - quoteChar - string of one or more characters defining the quote delimiting string
        - escChar - character to escape quotes, typically backslash (default=None)
        - escQuote - special quote sequence to escape an embedded quote string (such as SQL's "" to escape an embedded ") (default=None)
        - multiline - boolean indicating whether quotes can span multiple lines (default=False)
        - unquoteResults - boolean indicating whether the matched text should be unquoted (default=True)
        - endQuoteChar - string of one or more characters defining the end of the quote delimited string (default=None => same as quoteChar)
        """
        super().__init__()

        # remove white space from quote chars - won't work anyway
        quoteChar = quoteChar.strip()
        if len(quoteChar) == 0:
            warnings.warn(
                "quoteChar cannot be the empty string", SyntaxWarning, stacklevel=2
            )
            raise SyntaxError()

        if endQuoteChar is None:
            endQuoteChar = quoteChar
        else:
            endQuoteChar = endQuoteChar.strip()
            if len(endQuoteChar) == 0:
                warnings.warn(
                    "endQuoteChar cannot be the empty string",
                    SyntaxWarning,
                    stacklevel=2,
                )
                raise SyntaxError()

        self.quoteChar = quoteChar
        self.quoteCharLen = len(quoteChar)
        self.firstQuoteChar = quoteChar[0]
        self.endQuoteChar = endQuoteChar
        self.endQuoteCharLen = len(endQuoteChar)
        self.escChar = escChar
        self.escQuote = escQuote
        self.unquoteResults = unquoteResults

        if multiline:
            self.flags = re.MULTILINE | re.DOTALL
            self.pattern = r"{}(?:[^{}{}]".format(
                re.escape(self.quoteChar),
                _escapeRegexRangeChars(self.endQuoteChar[0]),
                (escChar is not None and _escapeRegexRangeChars(escChar) or ""),
            )
        else:
            self.flags = 0
            self.pattern = r"{}(?:[^{}\n\r{}]".format(
                re.escape(self.quoteChar),
                _escapeRegexRangeChars(self.endQuoteChar[0]),
                (escChar is not None and _escapeRegexRangeChars(escChar) or ""),
            )
        if len(self.endQuoteChar) > 1:
            self.pattern += (
                "|(?:"
                + ")|(?:".join(
                    [
                        "%s[^%s]"
                        % (
                            re.escape(self.endQuoteChar[:i]),
                            _escapeRegexRangeChars(self.endQuoteChar[i]),
                        )
                        for i in range(len(self.endQuoteChar) - 1, 0, -1)
                    ]
                )
                + ")"
            )
        if escQuote:
            self.pattern += r"|(?:%s)" % re.escape(escQuote)
        if escChar:
            self.pattern += r"|(?:%s.)" % re.escape(escChar)
            self.escCharReplacePattern = re.escape(self.escChar) + "(.)"
        self.pattern += r")*%s" % re.escape(self.endQuoteChar)

        try:
            self.re = re.compile(self.pattern, self.flags)
            self.reString = self.pattern
        except sre_constants.error as e:
            warnings.warn(
                "invalid pattern (%s) passed to Regex" % self.pattern,
                SyntaxWarning,
                stacklevel=2,
            )
            raise

        self.name = _ustr(self)
        self.errmsg = "Expected " + self.name
        self.myException.msg = self.errmsg
        self.mayIndexError = False
        self.mayReturnEmpty = True

    def parseImpl(self, instring, loc, doActions=True):
        result = (
            instring[loc] == self.firstQuoteChar
            and self.re.match(instring, loc)
            or None
        )
        if not result:
            exc = self.myException
            exc.loc = loc
            exc.pstr = instring
            raise exc

        loc = result.end()
        ret = result.group()

        if self.unquoteResults:

            # strip off quotes
            ret = ret[self.quoteCharLen : -self.endQuoteCharLen]

            if isinstance(ret, str):
                # replace escaped characters
                if self.escChar:
                    ret = re.sub(self.escCharReplacePattern, r"\g<1>", ret)

                # replace escaped quotes
                if self.escQuote:
                    ret = ret.replace(self.escQuote, self.endQuoteChar)

        return loc, ret

    def __str__(self):
        try:
            return super().__str__()
        except AttributeError:
            pass

        if self.strRepr is None:
            self.strRepr = "quoted string, starting with {} ending with {}".format(
                self.quoteChar, self.endQuoteChar
            )

        return self.strRepr


class CharsNotIn(Token):
    """Token for matching words composed of characters *not* in a given set.
    Defined with string containing all disallowed characters, and an optional
    minimum, maximum, and/or exact length.
    """

    def __init__(self, notChars, min=1, max=0, exact=0):
        super().__init__()
        self.skipWhitespace = False
        self.notChars = notChars

        self.minLen = min

        if max > 0:
            self.maxLen = max
        else:
            self.maxLen = sys.maxsize

        if exact > 0:
            self.maxLen = exact
            self.minLen = exact

        self.name = _ustr(self)
        self.errmsg = "Expected " + self.name
        self.mayReturnEmpty = self.minLen == 0
        self.myException.msg = self.errmsg
        self.mayIndexError = False

    def parseImpl(self, instring, loc, doActions=True):
        if instring[loc] in self.notChars:
            # ~ raise ParseException( instring, loc, self.errmsg )
            exc = self.myException
            exc.loc = loc
            exc.pstr = instring
            raise exc

        start = loc
        loc += 1
        notchars = self.notChars
        maxlen = min(start + self.maxLen, len(instring))
        while loc < maxlen and (instring[loc] not in notchars):
            loc += 1

        if loc - start < self.minLen:
            # ~ raise ParseException( instring, loc, self.errmsg )
            exc = self.myException
            exc.loc = loc
            exc.pstr = instring
            raise exc

        return loc, instring[start:loc]

    def __str__(self):
        try:
            return super().__str__()
        except AttributeError:
            pass

        if self.strRepr is None:
            if len(self.notChars) > 4:
                self.strRepr = "!W:(%s...)" % self.notChars[:4]
            else:
                self.strRepr = "!W:(%s)" % self.notChars

        return self.strRepr


class White(Token):
    """Special matching class for matching whitespace.  Normally, whitespace is ignored
    by pyparsing grammars.  This class is included when some whitespace structures
    are significant.  Define with a string containing the whitespace characters to be
    matched; default is " \\t\\n".  Also takes optional min, max, and exact arguments,
    as defined for the Word class."""

    whiteStrs = {" ": "<SPC>", "\t": "<TAB>", "\n": "<LF>", "\r": "<CR>", "\f": "<FF>"}

    def __init__(self, ws=" \t\r\n", min=1, max=0, exact=0):
        super().__init__()
        self.matchWhite = ws
        self.setWhitespaceChars(
            "".join([c for c in self.whiteChars if c not in self.matchWhite])
        )
        # ~ self.leaveWhitespace()
        self.name = "".join([White.whiteStrs[c] for c in self.matchWhite])
        self.mayReturnEmpty = True
        self.errmsg = "Expected " + self.name
        self.myException.msg = self.errmsg

        self.minLen = min

        if max > 0:
            self.maxLen = max
        else:
            self.maxLen = sys.maxsize

        if exact > 0:
            self.maxLen = exact
            self.minLen = exact

    def parseImpl(self, instring, loc, doActions=True):
        if not (instring[loc] in self.matchWhite):
            # ~ raise ParseException( instring, loc, self.errmsg )
            exc = self.myException
            exc.loc = loc
            exc.pstr = instring
            raise exc
        start = loc
        loc += 1
        maxloc = start + self.maxLen
        maxloc = min(maxloc, len(instring))
        while loc < maxloc and instring[loc] in self.matchWhite:
            loc += 1

        if loc - start < self.minLen:
            # ~ raise ParseException( instring, loc, self.errmsg )
            exc = self.myException
            exc.loc = loc
            exc.pstr = instring
            raise exc

        return loc, instring[start:loc]


class PositionToken(Token):
    def __init__(self):
        super().__init__()
        self.name = self.__class__.__name__
        self.mayReturnEmpty = True
        self.mayIndexError = False


class GoToColumn(PositionToken):
    """Token to advance to a specific column of input text; useful for tabular report scraping."""

    def __init__(self, colno):
        super().__init__()
        self.col = colno

    def preParse(self, instring, loc):
        if col(loc, instring) != self.col:
            instrlen = len(instring)
            if self.ignoreExprs:
                loc = self.skipIgnorables(instring, loc)
            while (
                loc < instrlen
                and instring[loc].isspace()
                and col(loc, instring) != self.col
            ):
                loc += 1
        return loc

    def parseImpl(self, instring, loc, doActions=True):
        thiscol = col(loc, instring)
        if thiscol > self.col:
            raise ParseException(instring, loc, "Text not in expected column", self)
        newloc = loc + self.col - thiscol
        ret = instring[loc:newloc]
        return newloc, ret


class LineStart(PositionToken):
    """Matches if current position is at the beginning of a line within the parse string"""

    def __init__(self):
        super().__init__()
        self.setWhitespaceChars(" \t")
        self.errmsg = "Expected start of line"
        self.myException.msg = self.errmsg

    def preParse(self, instring, loc):
        preloc = super().preParse(instring, loc)
        if instring[preloc] == "\n":
            loc += 1
        return loc

    def parseImpl(self, instring, loc, doActions=True):
        if not (
            loc == 0
            or (loc == self.preParse(instring, 0))
            or (instring[loc - 1] == "\n")
        ):  # col(loc, instring) != 1:
            # ~ raise ParseException( instring, loc, "Expected start of line" )
            exc = self.myException
            exc.loc = loc
            exc.pstr = instring
            raise exc
        return loc, []


class LineEnd(PositionToken):
    """Matches if current position is at the end of a line within the parse string"""

    def __init__(self):
        super().__init__()
        self.setWhitespaceChars(" \t")
        self.errmsg = "Expected end of line"
        self.myException.msg = self.errmsg

    def parseImpl(self, instring, loc, doActions=True):
        if loc < len(instring):
            if instring[loc] == "\n":
                return loc + 1, "\n"
            else:
                # ~ raise ParseException( instring, loc, "Expected end of line" )
                exc = self.myException
                exc.loc = loc
                exc.pstr = instring
                raise exc
        elif loc == len(instring):
            return loc + 1, []
        else:
            exc = self.myException
            exc.loc = loc
            exc.pstr = instring
            raise exc


class StringStart(PositionToken):
    """Matches if current position is at the beginning of the parse string"""

    def __init__(self):
        super().__init__()
        self.errmsg = "Expected start of text"
        self.myException.msg = self.errmsg

    def parseImpl(self, instring, loc, doActions=True):
        if loc != 0:
            # see if entire string up to here is just whitespace and ignorables
            if loc != self.preParse(instring, 0):
                # ~ raise ParseException( instring, loc, "Expected start of text" )
                exc = self.myException
                exc.loc = loc
                exc.pstr = instring
                raise exc
        return loc, []


class StringEnd(PositionToken):
    """Matches if current position is at the end of the parse string"""

    def __init__(self):
        super().__init__()
        self.errmsg = "Expected end of text"
        self.myException.msg = self.errmsg

    def parseImpl(self, instring, loc, doActions=True):
        if loc < len(instring):
            # ~ raise ParseException( instring, loc, "Expected end of text" )
            exc = self.myException
            exc.loc = loc
            exc.pstr = instring
            raise exc
        elif loc == len(instring):
            return loc + 1, []
        else:
            exc = self.myException
            exc.loc = loc
            exc.pstr = instring
            raise exc


class ParseExpression(ParserElement):
    """Abstract subclass of ParserElement, for combining and post-processing parsed tokens."""

    def __init__(self, exprs, savelist=False):
        super().__init__(savelist)
        if isinstance(exprs, list):
            self.exprs = exprs
        elif isinstance(exprs, str):
            self.exprs = [Literal(exprs)]
        else:
            self.exprs = [exprs]

    def __getitem__(self, i):
        return self.exprs[i]

    def append(self, other):
        self.exprs.append(other)
        self.strRepr = None
        return self

    def leaveWhitespace(self):
        """Extends leaveWhitespace defined in base class, and also invokes leaveWhitespace on
        all contained expressions."""
        self.skipWhitespace = False
        self.exprs = [e.copy() for e in self.exprs]
        for e in self.exprs:
            e.leaveWhitespace()
        return self

    def ignore(self, other):
        if isinstance(other, Suppress):
            if other not in self.ignoreExprs:
                super().ignore(other)
                for e in self.exprs:
                    e.ignore(self.ignoreExprs[-1])
        else:
            super().ignore(other)
            for e in self.exprs:
                e.ignore(self.ignoreExprs[-1])
        return self

    def __str__(self):
        try:
            return super().__str__()
        except AttributeError:
            pass

        if self.strRepr is None:
            self.strRepr = "{}:({})".format(self.__class__.__name__, _ustr(self.exprs))
        return self.strRepr

    def streamline(self):
        super().streamline()

        for e in self.exprs:
            e.streamline()

        # collapse nested And's of the form And( And( And( a,b), c), d) to And( a,b,c,d )
        # but only if there are no parse actions or resultsNames on the nested And's
        # (likewise for Or's and MatchFirst's)
        if len(self.exprs) == 2:
            other = self.exprs[0]
            if (
                isinstance(other, self.__class__)
                and not (other.parseAction)
                and other.resultsName is None
                and not other.debug
            ):
                self.exprs = other.exprs[:] + [self.exprs[1]]
                self.strRepr = None
                self.mayReturnEmpty |= other.mayReturnEmpty
                self.mayIndexError |= other.mayIndexError

            other = self.exprs[-1]
            if (
                isinstance(other, self.__class__)
                and not (other.parseAction)
                and other.resultsName is None
                and not other.debug
            ):
                self.exprs = self.exprs[:-1] + other.exprs[:]
                self.strRepr = None
                self.mayReturnEmpty |= other.mayReturnEmpty
                self.mayIndexError |= other.mayIndexError

        return self

    def setResultsName(self, name, listAllMatches=False):
        ret = super().setResultsName(name, listAllMatches)
        return ret

    def validate(self, validateTrace=[]):
        tmp = validateTrace[:] + [self]
        for e in self.exprs:
            e.validate(tmp)
        self.checkRecursion([])


class And(ParseExpression):
    """Requires all given ParseExpressions to be found in the given order.
    Expressions may be separated by whitespace.
    May be constructed using the '+' operator.
    """

    def __init__(self, exprs, savelist=True):
        super().__init__(exprs, savelist)
        self.mayReturnEmpty = True
        for e in self.exprs:
            if not e.mayReturnEmpty:
                self.mayReturnEmpty = False
                break
        self.setWhitespaceChars(exprs[0].whiteChars)
        self.skipWhitespace = exprs[0].skipWhitespace

    def parseImpl(self, instring, loc, doActions=True):
        # pass False as last arg to _parse for first element, since we already
        # pre-parsed the string as part of our And pre-parsing
        loc, resultlist = self.exprs[0]._parse(
            instring, loc, doActions, callPreParse=False
        )
        for e in self.exprs[1:]:
            loc, exprtokens = e._parse(instring, loc, doActions)
            if exprtokens or list(exprtokens.keys()):
                resultlist += exprtokens
        return loc, resultlist

    def __iadd__(self, other):
        if isinstance(other, str):
            other = Literal(other)
        return self.append(other)  # And( [ self, other ] )

    def checkRecursion(self, parseElementList):
        subRecCheckList = parseElementList[:] + [self]
        for e in self.exprs:
            e.checkRecursion(subRecCheckList)
            if not e.mayReturnEmpty:
                break

    def __str__(self):
        if hasattr(self, "name"):
            return self.name

        if self.strRepr is None:
            self.strRepr = "{" + " ".join([_ustr(e) for e in self.exprs]) + "}"

        return self.strRepr


class Or(ParseExpression):
    """Requires that at least one ParseExpression is found.
    If two expressions match, the expression that matches the longest string will be used.
    May be constructed using the '^' operator.
    """

    def __init__(self, exprs, savelist=False):
        super().__init__(exprs, savelist)
        self.mayReturnEmpty = False
        for e in self.exprs:
            if e.mayReturnEmpty:
                self.mayReturnEmpty = True
                break

    def parseImpl(self, instring, loc, doActions=True):
        maxExcLoc = -1
        maxMatchLoc = -1
        for e in self.exprs:
            try:
                loc2 = e.tryParse(instring, loc)
            except ParseException as err:
                if err.loc > maxExcLoc:
                    maxException = err
                    maxExcLoc = err.loc
            except IndexError:
                if len(instring) > maxExcLoc:
                    maxException = ParseException(
                        instring, len(instring), e.errmsg, self
                    )
                    maxExcLoc = len(instring)
            else:
                if loc2 > maxMatchLoc:
                    maxMatchLoc = loc2
                    maxMatchExp = e

        if maxMatchLoc < 0:
            if self.exprs:
                raise maxException
            else:
                raise ParseException(
                    instring, loc, "no defined alternatives to match", self
                )

        return maxMatchExp._parse(instring, loc, doActions)

    def __ixor__(self, other):
        if isinstance(other, str):
            other = Literal(other)
        return self.append(other)  # Or( [ self, other ] )

    def __str__(self):
        if hasattr(self, "name"):
            return self.name

        if self.strRepr is None:
            self.strRepr = "{" + " ^ ".join([_ustr(e) for e in self.exprs]) + "}"

        return self.strRepr

    def checkRecursion(self, parseElementList):
        subRecCheckList = parseElementList[:] + [self]
        for e in self.exprs:
            e.checkRecursion(subRecCheckList)


class MatchFirst(ParseExpression):
    """Requires that at least one ParseExpression is found.
    If two expressions match, the first one listed is the one that will match.
    May be constructed using the '|' operator.
    """

    def __init__(self, exprs, savelist=False):
        super().__init__(exprs, savelist)
        if exprs:
            self.mayReturnEmpty = False
            for e in self.exprs:
                if e.mayReturnEmpty:
                    self.mayReturnEmpty = True
                    break
        else:
            self.mayReturnEmpty = True

    def parseImpl(self, instring, loc, doActions=True):
        maxExcLoc = -1
        for e in self.exprs:
            try:
                ret = e._parse(instring, loc, doActions)
                return ret
            except ParseException as err:
                if err.loc > maxExcLoc:
                    maxException = err
                    maxExcLoc = err.loc
            except IndexError:
                if len(instring) > maxExcLoc:
                    maxException = ParseException(
                        instring, len(instring), e.errmsg, self
                    )
                    maxExcLoc = len(instring)

        # only got here if no expression matched, raise exception for match that made it the furthest
        else:
            if self.exprs:
                raise maxException
            else:
                raise ParseException(
                    instring, loc, "no defined alternatives to match", self
                )

    def __ior__(self, other):
        if isinstance(other, str):
            other = Literal(other)
        return self.append(other)  # MatchFirst( [ self, other ] )

    def __str__(self):
        if hasattr(self, "name"):
            return self.name

        if self.strRepr is None:
            self.strRepr = "{" + " | ".join([_ustr(e) for e in self.exprs]) + "}"

        return self.strRepr

    def checkRecursion(self, parseElementList):
        subRecCheckList = parseElementList[:] + [self]
        for e in self.exprs:
            e.checkRecursion(subRecCheckList)


class Each(ParseExpression):
    """Requires all given ParseExpressions to be found, but in any order.
    Expressions may be separated by whitespace.
    May be constructed using the '&' operator.
    """

    def __init__(self, exprs, savelist=True):
        super().__init__(exprs, savelist)
        self.mayReturnEmpty = True
        for e in self.exprs:
            if not e.mayReturnEmpty:
                self.mayReturnEmpty = False
                break
        self.skipWhitespace = True
        self.optionals = [e.expr for e in exprs if isinstance(e, Optional)]
        self.multioptionals = [e.expr for e in exprs if isinstance(e, ZeroOrMore)]
        self.multirequired = [e.expr for e in exprs if isinstance(e, OneOrMore)]
        self.required = [
            e for e in exprs if not isinstance(e, (Optional, ZeroOrMore, OneOrMore))
        ]
        self.required += self.multirequired

    def parseImpl(self, instring, loc, doActions=True):
        tmpLoc = loc
        tmpReqd = self.required[:]
        tmpOpt = self.optionals[:]
        matchOrder = []

        keepMatching = True
        while keepMatching:
            tmpExprs = tmpReqd + tmpOpt + self.multioptionals + self.multirequired
            failed = []
            for e in tmpExprs:
                try:
                    tmpLoc = e.tryParse(instring, tmpLoc)
                except ParseException:
                    failed.append(e)
                else:
                    matchOrder.append(e)
                    if e in tmpReqd:
                        tmpReqd.remove(e)
                    elif e in tmpOpt:
                        tmpOpt.remove(e)
            if len(failed) == len(tmpExprs):
                keepMatching = False

        if tmpReqd:
            missing = ", ".join([_ustr(e) for e in tmpReqd])
            raise ParseException(
                instring, loc, "Missing one or more required elements (%s)" % missing
            )

        resultlist = []
        for e in matchOrder:
            loc, results = e._parse(instring, loc, doActions)
            resultlist.append(results)

        finalResults = ParseResults([])
        for r in resultlist:
            dups = {}
            for k in list(r.keys()):
                if k in list(finalResults.keys()):
                    tmp = ParseResults(finalResults[k])
                    tmp += ParseResults(r[k])
                    dups[k] = tmp
            finalResults += ParseResults(r)
            for k, v in list(dups.items()):
                finalResults[k] = v
        return loc, finalResults

    def __str__(self):
        if hasattr(self, "name"):
            return self.name

        if self.strRepr is None:
            self.strRepr = "{" + " & ".join([_ustr(e) for e in self.exprs]) + "}"

        return self.strRepr

    def checkRecursion(self, parseElementList):
        subRecCheckList = parseElementList[:] + [self]
        for e in self.exprs:
            e.checkRecursion(subRecCheckList)


class ParseElementEnhance(ParserElement):
    """Abstract subclass of ParserElement, for combining and post-processing parsed tokens."""

    def __init__(self, expr, savelist=False):
        super().__init__(savelist)
        if isinstance(expr, str):
            expr = Literal(expr)
        self.expr = expr
        self.strRepr = None
        if expr is not None:
            self.mayIndexError = expr.mayIndexError
            self.setWhitespaceChars(expr.whiteChars)
            self.skipWhitespace = expr.skipWhitespace
            self.saveAsList = expr.saveAsList

    def parseImpl(self, instring, loc, doActions=True):
        if self.expr is not None:
            return self.expr._parse(instring, loc, doActions, callPreParse=False)
        else:
            raise ParseException("", loc, self.errmsg, self)

    def leaveWhitespace(self):
        self.skipWhitespace = False
        self.expr = self.expr.copy()
        if self.expr is not None:
            self.expr.leaveWhitespace()
        return self

    def ignore(self, other):
        if isinstance(other, Suppress):
            if other not in self.ignoreExprs:
                super().ignore(other)
                if self.expr is not None:
                    self.expr.ignore(self.ignoreExprs[-1])
        else:
            super().ignore(other)
            if self.expr is not None:
                self.expr.ignore(self.ignoreExprs[-1])
        return self

    def streamline(self):
        super().streamline()
        if self.expr is not None:
            self.expr.streamline()
        return self

    def checkRecursion(self, parseElementList):
        if self in parseElementList:
            raise RecursiveGrammarException(parseElementList + [self])
        subRecCheckList = parseElementList[:] + [self]
        if self.expr is not None:
            self.expr.checkRecursion(subRecCheckList)

    def validate(self, validateTrace=[]):
        tmp = validateTrace[:] + [self]
        if self.expr is not None:
            self.expr.validate(tmp)
        self.checkRecursion([])

    def __str__(self):
        try:
            return super().__str__()
        except AttributeError:
            pass

        if self.strRepr is None and self.expr is not None:
            self.strRepr = "{}:({})".format(self.__class__.__name__, _ustr(self.expr))
        return self.strRepr


class FollowedBy(ParseElementEnhance):
    """Lookahead matching of the given parse expression.  FollowedBy
    does *not* advance the parsing position within the input string, it only
    verifies that the specified parse expression matches at the current
    position.  FollowedBy always returns a null token list."""

    def __init__(self, expr):
        super().__init__(expr)
        self.mayReturnEmpty = True

    def parseImpl(self, instring, loc, doActions=True):
        self.expr.tryParse(instring, loc)
        return loc, []


class NotAny(ParseElementEnhance):
    """Lookahead to disallow matching with the given parse expression.  NotAny
    does *not* advance the parsing position within the input string, it only
    verifies that the specified parse expression does *not* match at the current
    position.  Also, NotAny does *not* skip over leading whitespace. NotAny
    always returns a null token list.  May be constructed using the '~' operator."""

    def __init__(self, expr):
        super().__init__(expr)
        # ~ self.leaveWhitespace()
        self.skipWhitespace = (
            False  # do NOT use self.leaveWhitespace(), don't want to propagate to exprs
        )
        self.mayReturnEmpty = True
        self.errmsg = "Found unwanted token, " + _ustr(self.expr)
        self.myException = ParseException("", 0, self.errmsg, self)

    def parseImpl(self, instring, loc, doActions=True):
        try:
            self.expr.tryParse(instring, loc)
        except (ParseException, IndexError):
            pass
        else:
            # ~ raise ParseException(instring, loc, self.errmsg )
            exc = self.myException
            exc.loc = loc
            exc.pstr = instring
            raise exc
        return loc, []

    def __str__(self):
        if hasattr(self, "name"):
            return self.name

        if self.strRepr is None:
            self.strRepr = "~{" + _ustr(self.expr) + "}"

        return self.strRepr


class ZeroOrMore(ParseElementEnhance):
    """Optional repetition of zero or more of the given expression."""

    def __init__(self, expr):
        super().__init__(expr)
        self.mayReturnEmpty = True

    def parseImpl(self, instring, loc, doActions=True):
        tokens = []
        try:
            loc, tokens = self.expr._parse(instring, loc, doActions, callPreParse=False)
            hasIgnoreExprs = len(self.ignoreExprs) > 0
            while 1:
                if hasIgnoreExprs:
                    preloc = self.skipIgnorables(instring, loc)
                else:
                    preloc = loc
                loc, tmptokens = self.expr._parse(instring, preloc, doActions)
                if tmptokens or list(tmptokens.keys()):
                    tokens += tmptokens
        except (ParseException, IndexError):
            pass

        return loc, tokens

    def __str__(self):
        if hasattr(self, "name"):
            return self.name

        if self.strRepr is None:
            self.strRepr = "[" + _ustr(self.expr) + "]..."

        return self.strRepr

    def setResultsName(self, name, listAllMatches=False):
        ret = super().setResultsName(name, listAllMatches)
        ret.saveAsList = True
        return ret


class OneOrMore(ParseElementEnhance):
    """Repetition of one or more of the given expression."""

    def parseImpl(self, instring, loc, doActions=True):
        # must be at least one
        loc, tokens = self.expr._parse(instring, loc, doActions, callPreParse=False)
        try:
            hasIgnoreExprs = len(self.ignoreExprs) > 0
            while 1:
                if hasIgnoreExprs:
                    preloc = self.skipIgnorables(instring, loc)
                else:
                    preloc = loc
                loc, tmptokens = self.expr._parse(instring, preloc, doActions)
                if tmptokens or list(tmptokens.keys()):
                    tokens += tmptokens
        except (ParseException, IndexError):
            pass

        return loc, tokens

    def __str__(self):
        if hasattr(self, "name"):
            return self.name

        if self.strRepr is None:
            self.strRepr = "{" + _ustr(self.expr) + "}..."

        return self.strRepr

    def setResultsName(self, name, listAllMatches=False):
        ret = super().setResultsName(name, listAllMatches)
        ret.saveAsList = True
        return ret


class _NullToken:
    def __bool__(self):
        return False

    def __str__(self):
        return ""


_optionalNotMatched = _NullToken()


class Optional(ParseElementEnhance):
    """Optional matching of the given expression.
    A default return string can also be specified, if the optional expression
    is not found.
    """

    def __init__(self, exprs, default=_optionalNotMatched):
        super().__init__(exprs, savelist=False)
        self.defaultValue = default
        self.mayReturnEmpty = True

    def parseImpl(self, instring, loc, doActions=True):
        try:
            loc, tokens = self.expr._parse(instring, loc, doActions, callPreParse=False)
        except (ParseException, IndexError):
            if self.defaultValue is not _optionalNotMatched:
                tokens = [self.defaultValue]
            else:
                tokens = []
        return loc, tokens

    def __str__(self):
        if hasattr(self, "name"):
            return self.name

        if self.strRepr is None:
            self.strRepr = "[" + _ustr(self.expr) + "]"

        return self.strRepr


class SkipTo(ParseElementEnhance):
    """Token for skipping over all undefined text until the matched expression is found.
    If include is set to true, the matched expression is also consumed.  The ignore
    argument is used to define grammars (typically quoted strings and comments) that
    might contain false matches.
    """

    def __init__(self, other, include=False, ignore=None):
        super().__init__(other)
        if ignore is not None:
            self.expr = self.expr.copy()
            self.expr.ignore(ignore)
        self.mayReturnEmpty = True
        self.mayIndexError = False
        self.includeMatch = include
        self.asList = False
        self.errmsg = "No match found for " + _ustr(self.expr)
        self.myException = ParseException("", 0, self.errmsg, self)

    def parseImpl(self, instring, loc, doActions=True):
        startLoc = loc
        instrlen = len(instring)
        expr = self.expr
        while loc <= instrlen:
            try:
                loc = expr.skipIgnorables(instring, loc)
                expr._parse(instring, loc, doActions=False, callPreParse=False)
                if self.includeMatch:
                    skipText = instring[startLoc:loc]
                    loc, mat = expr._parse(instring, loc)
                    if mat:
                        return loc, [skipText, mat]
                    else:
                        return loc, [skipText]
                else:
                    return loc, [instring[startLoc:loc]]
            except (ParseException, IndexError):
                loc += 1
        exc = self.myException
        exc.loc = loc
        exc.pstr = instring
        raise exc


class Forward(ParseElementEnhance):
    """Forward declaration of an expression to be defined later -
    used for recursive grammars, such as algebraic infix notation.
    When the expression is known, it is assigned to the Forward variable using the '<<' operator.

    Note: take care when assigning to Forward not to overlook precedence of operators.
    Specifically, '|' has a lower precedence than '<<', so that::
       fwdExpr << a | b | c
    will actually be evaluated as::
       (fwdExpr << a) | b | c
    thereby leaving b and c out as parseable alternatives.  It is recommended that you
    explicitly group the values inserted into the Forward::
       fwdExpr << (a | b | c)
    """

    def __init__(self, other=None):
        super().__init__(other, savelist=False)

    def __lshift__(self, other):
        if isinstance(other, str):
            other = Literal(other)
        self.expr = other
        self.mayReturnEmpty = other.mayReturnEmpty
        self.strRepr = None
        return self

    def leaveWhitespace(self):
        self.skipWhitespace = False
        return self

    def streamline(self):
        if not self.streamlined:
            self.streamlined = True
            if self.expr is not None:
                self.expr.streamline()
        return self

    def validate(self, validateTrace=[]):
        if self not in validateTrace:
            tmp = validateTrace[:] + [self]
            if self.expr is not None:
                self.expr.validate(tmp)
        self.checkRecursion([])

    def __str__(self):
        if hasattr(self, "name"):
            return self.name

        self.__class__ = _ForwardNoRecurse
        try:
            if self.expr is not None:
                retString = _ustr(self.expr)
            else:
                retString = "None"
        finally:
            self.__class__ = Forward
        return "Forward: " + retString

    def copy(self):
        if self.expr is not None:
            return super().copy()
        else:
            ret = Forward()
            ret << self
            return ret


class _ForwardNoRecurse(Forward):
    def __str__(self):
        return "..."


class TokenConverter(ParseElementEnhance):
    """Abstract subclass of ParseExpression, for converting parsed results."""

    def __init__(self, expr, savelist=False):
        super().__init__(expr)  # , savelist )
        self.saveAsList = False


class Upcase(TokenConverter):
    """Converter to upper case all matching tokens."""

    def __init__(self, *args):
        super().__init__(*args)
        warnings.warn(
            "Upcase class is deprecated, use upcaseTokens parse action instead",
            DeprecationWarning,
            stacklevel=2,
        )

    def postParse(self, instring, loc, tokenlist):
        return list(map(lambda token: token.upper(), tokenlist))


class Combine(TokenConverter):
    """Converter to concatenate all matching tokens to a single string.
    By default, the matching patterns must also be contiguous in the input string;
    this can be disabled by specifying 'adjacent=False' in the constructor.
    """

    def __init__(self, expr, joinString="", adjacent=True):
        super().__init__(expr)
        # suppress whitespace-stripping in contained parse expressions, but re-enable it on the Combine itself
        if adjacent:
            self.leaveWhitespace()
        self.adjacent = adjacent
        self.skipWhitespace = True
        self.joinString = joinString

    def ignore(self, other):
        if self.adjacent:
            ParserElement.ignore(self, other)
        else:
            super().ignore(other)
        return self

    def postParse(self, instring, loc, tokenlist):
        retToks = tokenlist.copy()
        del retToks[:]
        retToks += ParseResults(
            ["".join(tokenlist._asStringList(self.joinString))], modal=self.modalResults
        )

        if self.resultsName and len(list(retToks.keys())) > 0:
            return [retToks]
        else:
            return retToks


class Group(TokenConverter):
    """Converter to return the matched tokens as a list - useful for returning tokens of ZeroOrMore and OneOrMore expressions."""

    def __init__(self, expr):
        super().__init__(expr)
        self.saveAsList = True

    def postParse(self, instring, loc, tokenlist):
        return [tokenlist]


class Dict(TokenConverter):
    """Converter to return a repetitive expression as a list, but also as a dictionary.
    Each element can also be referenced using the first token in the expression as its key.
    Useful for tabular report scraping when the first column can be used as an item key.
    """

    def __init__(self, exprs):
        super().__init__(exprs)
        self.saveAsList = True

    def postParse(self, instring, loc, tokenlist):
        for i, tok in enumerate(tokenlist):
            ikey = _ustr(tok[0]).strip()
            if len(tok) == 1:
                tokenlist[ikey] = ("", i)
            elif len(tok) == 2 and not isinstance(tok[1], ParseResults):
                tokenlist[ikey] = (tok[1], i)
            else:
                dictvalue = tok.copy()  # ParseResults(i)
                del dictvalue[0]
                if len(dictvalue) != 1 or (
                    isinstance(dictvalue, ParseResults) and list(dictvalue.keys())
                ):
                    tokenlist[ikey] = (dictvalue, i)
                else:
                    tokenlist[ikey] = (dictvalue[0], i)

        if self.resultsName:
            return [tokenlist]
        else:
            return tokenlist


class Suppress(TokenConverter):
    """Converter for ignoring the results of a parsed expression."""

    def postParse(self, instring, loc, tokenlist):
        return []

    def suppress(self):
        return self


class OnlyOnce:
    """Wrapper for parse actions, to ensure they are only called once."""

    def __init__(self, methodCall):
        self.callable = ParserElement.normalizeParseActionArgs(methodCall)
        self.called = False

    def __call__(self, s, l, t):
        if not self.called:
            results = self.callable(s, l, t)
            self.called = True
            return results
        raise ParseException(s, l, "")

    def reset(self):
        self.called = False


def traceParseAction(f):
    """Decorator for debugging parse actions."""
    f = ParserElement.normalizeParseActionArgs(f)

    def z(*paArgs):
        thisFunc = f.__name__
        s, l, t = paArgs[-3:]
        if len(paArgs) > 3:
            thisFunc = paArgs[0].__class__.__name__ + "." + thisFunc
        sys.stderr.write(
            ">>entering %s(line: '%s', %d, %s)\n" % (thisFunc, line(l, s), l, t)
        )
        try:
            ret = f(*paArgs)
        except Exception as exc:
            sys.stderr.write("<<leaving {} (exception: {})\n".format(thisFunc, exc))
            raise
        sys.stderr.write("<<leaving {} (ret: {})\n".format(thisFunc, ret))
        return ret

    return z


#
# global helpers
#
def delimitedList(expr, delim=",", combine=False):
    """Helper to define a delimited list of expressions - the delimiter defaults to ','.
    By default, the list elements and delimiters can have intervening whitespace, and
    comments, but this can be overridden by passing 'combine=True' in the constructor.
    If combine is set to True, the matching tokens are returned as a single token
    string, with the delimiters included; otherwise, the matching tokens are returned
    as a list of tokens, with the delimiters suppressed.
    """
    dlName = _ustr(expr) + " [" + _ustr(delim) + " " + _ustr(expr) + "]..."
    if combine:
        return Combine(expr + ZeroOrMore(delim + expr)).setName(dlName)
    else:
        return (expr + ZeroOrMore(Suppress(delim) + expr)).setName(dlName)


def countedArray(expr):
    """Helper to define a counted list of expressions.
    This helper defines a pattern of the form::
        integer expr expr expr...
    where the leading integer tells how many expr expressions follow.
    The matched tokens returns the array of expr tokens as a list - the leading count token is suppressed.
    """
    arrayExpr = Forward()

    def countFieldParseAction(s, l, t):
        n = int(t[0])
        arrayExpr << (n and Group(And([expr] * n)) or Group(empty))
        return []

    return Word(nums).setParseAction(countFieldParseAction) + arrayExpr


def _flatten(L):
    if type(L) is not list:
        return [L]
    if L == []:
        return L
    return _flatten(L[0]) + _flatten(L[1:])


def matchPreviousLiteral(expr):
    """Helper to define an expression that is indirectly defined from
    the tokens matched in a previous expression, that is, it looks
    for a 'repeat' of a previous expression.  For example::
        first = Word(nums)
        second = matchPreviousLiteral(first)
        matchExpr = first + ":" + second
    will match "1:1", but not "1:2".  Because this matches a
    previous literal, will also match the leading "1:1" in "1:10".
    If this is not desired, use matchPreviousExpr.
    Do *not* use with packrat parsing enabled.
    """
    rep = Forward()

    def copyTokenToRepeater(s, l, t):
        if t:
            if len(t) == 1:
                rep << t[0]
            else:
                # flatten t tokens
                tflat = _flatten(t.asList())
                rep << And([Literal(tt) for tt in tflat])
        else:
            rep << Empty()

    expr.addParseAction(copyTokenToRepeater)
    return rep


def matchPreviousExpr(expr):
    """Helper to define an expression that is indirectly defined from
    the tokens matched in a previous expression, that is, it looks
    for a 'repeat' of a previous expression.  For example::
        first = Word(nums)
        second = matchPreviousExpr(first)
        matchExpr = first + ":" + second
    will match "1:1", but not "1:2".  Because this matches by
    expressions, will *not* match the leading "1:1" in "1:10";
    the expressions are evaluated first, and then compared, so
    "1" is compared with "10".
    Do *not* use with packrat parsing enabled.
    """
    rep = Forward()
    e2 = expr.copy()
    rep << e2

    def copyTokenToRepeater(s, l, t):
        matchTokens = _flatten(t.asList())

        def mustMatchTheseTokens(s, l, t):
            theseTokens = _flatten(t.asList())
            if theseTokens != matchTokens:
                raise ParseException("", 0, "")

        rep.setParseAction(mustMatchTheseTokens)

    expr.addParseAction(copyTokenToRepeater)
    return rep


def _escapeRegexRangeChars(s):
    # ~  escape these chars: ^-]
    for c in r"\^-]":
        s = s.replace(c, "\\" + c)
    s = s.replace("\n", r"\n")
    s = s.replace("\t", r"\t")
    return _ustr(s)


def oneOf(strs, caseless=False, useRegex=True):
    """Helper to quickly define a set of alternative Literals, and makes sure to do
    longest-first testing when there is a conflict, regardless of the input order,
    but returns a MatchFirst for best performance.

    Parameters:
     - strs - a string of space-delimited literals, or a list of string literals
     - caseless - (default=False) - treat all literals as caseless
     - useRegex - (default=True) - as an optimization, will generate a Regex
       object; otherwise, will generate a MatchFirst object (if caseless=True, or
       if creating a Regex raises an exception)
    """
    if caseless:
        isequal = lambda a, b: a.upper() == b.upper()
        masks = lambda a, b: b.upper().startswith(a.upper())
        parseElementClass = CaselessLiteral
    else:
        isequal = lambda a, b: a == b
        masks = lambda a, b: b.startswith(a)
        parseElementClass = Literal

    if isinstance(strs, (list, tuple)):
        symbols = strs[:]
    elif isinstance(strs, str):
        symbols = strs.split()
    else:
        warnings.warn(
            "Invalid argument to oneOf, expected string or list",
            SyntaxWarning,
            stacklevel=2,
        )

    i = 0
    while i < len(symbols) - 1:
        cur = symbols[i]
        for j, other in enumerate(symbols[i + 1 :]):
            if isequal(other, cur):
                del symbols[i + j + 1]
                break
            elif masks(cur, other):
                del symbols[i + j + 1]
                symbols.insert(i, other)
                cur = other
                break
        else:
            i += 1

    if not caseless and useRegex:
        # ~ print strs,"->", "|".join( [ _escapeRegexChars(sym) for sym in symbols] )
        try:
            if len(symbols) == len("".join(symbols)):
                return Regex(
                    "[%s]" % "".join([_escapeRegexRangeChars(sym) for sym in symbols])
                )
            else:
                return Regex("|".join([re.escape(sym) for sym in symbols]))
        except:
            warnings.warn(
                "Exception creating Regex for oneOf, building MatchFirst",
                SyntaxWarning,
                stacklevel=2,
            )

    # last resort, just use MatchFirst
    return MatchFirst([parseElementClass(sym) for sym in symbols])


def dictOf(key, value):
    """Helper to easily and clearly define a dictionary by specifying the respective patterns
    for the key and value.  Takes care of defining the Dict, ZeroOrMore, and Group tokens
    in the proper order.  The key pattern can include delimiting markers or punctuation,
    as long as they are suppressed, thereby leaving the significant key text.  The value
    pattern can include named results, so that the Dict results can include named token
    fields.
    """
    return Dict(ZeroOrMore(Group(key + value)))


_bslash = "\\"
printables = "".join([c for c in string.printable if c not in string.whitespace])

# convenience constants for positional expressions
empty = Empty().setName("empty")
lineStart = LineStart().setName("lineStart")
lineEnd = LineEnd().setName("lineEnd")
stringStart = StringStart().setName("stringStart")
stringEnd = StringEnd().setName("stringEnd")

_escapedPunc = Word(_bslash, r"\[]-*.$+^?()~ ", exact=2).setParseAction(
    lambda s, l, t: t[0][1]
)
_printables_less_backslash = "".join([c for c in printables if c not in r"\]"])
_escapedHexChar = Combine(Suppress(_bslash + "0x") + Word(hexnums)).setParseAction(
    lambda s, l, t: chr(int(t[0], 16))
)
_escapedOctChar = Combine(Suppress(_bslash) + Word("0", "01234567")).setParseAction(
    lambda s, l, t: chr(int(t[0], 8))
)
_singleChar = (
    _escapedPunc
    | _escapedHexChar
    | _escapedOctChar
    | Word(_printables_less_backslash, exact=1)
)
_charRange = Group(_singleChar + Suppress("-") + _singleChar)
_reBracketExpr = (
    "["
    + Optional("^").setResultsName("negate")
    + Group(OneOrMore(_charRange | _singleChar)).setResultsName("body")
    + "]"
)

_expanded = lambda p: (
    isinstance(p, ParseResults)
    and "".join([chr(c) for c in range(ord(p[0]), ord(p[1]) + 1)])
    or p
)


def srange(s):
    r"""Helper to easily define string ranges for use in Word construction.  Borrows
    syntax from regexp '[]' string range definitions::
       srange("[0-9]")   -> "0123456789"
       srange("[a-z]")   -> "abcdefghijklmnopqrstuvwxyz"
       srange("[a-z$_]") -> "abcdefghijklmnopqrstuvwxyz$_"
    The input string must be enclosed in []'s, and the returned string is the expanded
    character set joined into a single string.
    The values enclosed in the []'s may be::
       a single character
       an escaped character with a leading backslash (such as \- or \])
       an escaped hex character with a leading '\0x' (\0x21, which is a '!' character)
       an escaped octal character with a leading '\0' (\041, which is a '!' character)
       a range of any of the above, separated by a dash ('a-z', etc.)
       any combination of the above ('aeiouy', 'a-zA-Z0-9_$', etc.)
    """
    try:
        return "".join([_expanded(part) for part in _reBracketExpr.parseString(s).body])
    except:
        return ""


def replaceWith(replStr):
    """Helper method for common parse actions that simply return a literal value.  Especially
    useful when used with transformString().
    """

    def _replFunc(*args):
        return [replStr]

    return _replFunc


def removeQuotes(s, l, t):
    """Helper parse action for removing quotation marks from parsed quoted strings.
    To use, add this parse action to quoted string using::
      quotedString.setParseAction( removeQuotes )
    """
    return t[0][1:-1]


def upcaseTokens(s, l, t):
    """Helper parse action to convert tokens to upper case."""
    return [str(tt).upper() for tt in t]


def downcaseTokens(s, l, t):
    """Helper parse action to convert tokens to lower case."""
    return [str(tt).lower() for tt in t]


def keepOriginalText(s, startLoc, t):
    import inspect

    """Helper parse action to preserve original parsed text,
       overriding any nested parse actions."""
    f = inspect.stack()[1][0]
    try:
        endloc = f.f_locals["loc"]
    finally:
        del f
    return s[startLoc:endloc]


def _makeTags(tagStr, xml):
    """Internal helper to construct opening and closing tag expressions, given a tag name"""
    if isinstance(tagStr, str):
        resname = tagStr
        tagStr = Keyword(tagStr, caseless=not xml)
    else:
        resname = tagStr.name

    tagAttrName = Word(alphas, alphanums + "_-")
    if xml:
        tagAttrValue = dblQuotedString.copy().setParseAction(removeQuotes)
        openTag = (
            Suppress("<")
            + tagStr
            + Dict(ZeroOrMore(Group(tagAttrName + Suppress("=") + tagAttrValue)))
            + Optional("/", default=[False])
            .setResultsName("empty")
            .setParseAction(lambda s, l, t: t[0] == "/")
            + Suppress(">")
        )
    else:
        printablesLessRAbrack = "".join([c for c in printables if c not in ">"])
        tagAttrValue = quotedString.copy().setParseAction(removeQuotes) | Word(
            printablesLessRAbrack
        )
        openTag = (
            Suppress("<")
            + tagStr
            + Dict(
                ZeroOrMore(
                    Group(
                        tagAttrName.setParseAction(downcaseTokens)
                        + Suppress("=")
                        + tagAttrValue
                    )
                )
            )
            + Optional("/", default=[False])
            .setResultsName("empty")
            .setParseAction(lambda s, l, t: t[0] == "/")
            + Suppress(">")
        )
    closeTag = Combine("</" + tagStr + ">")

    openTag = openTag.setResultsName(
        "start" + "".join(resname.replace(":", " ").title().split())
    ).setName("<%s>" % tagStr)
    closeTag = closeTag.setResultsName(
        "end" + "".join(resname.replace(":", " ").title().split())
    ).setName("</%s>" % tagStr)

    return openTag, closeTag


def makeHTMLTags(tagStr):
    """Helper to construct opening and closing tag expressions for HTML, given a tag name"""
    return _makeTags(tagStr, False)


def makeXMLTags(tagStr):
    """Helper to construct opening and closing tag expressions for XML, given a tag name"""
    return _makeTags(tagStr, True)


opAssoc = _Constants()
opAssoc.LEFT = object()
opAssoc.RIGHT = object()


def operatorPrecedence(baseExpr, opList):
    """Helper method for constructing grammars of expressions made up of
    operators working in a precedence hierarchy.  Operators may be unary or
    binary, left- or right-associative.  Parse actions can also be attached
    to operator expressions.

    Parameters:
     - baseExpr - expression representing the most basic element for the nested
     - opList - list of tuples, one for each operator precedence level in the expression grammar; each tuple is of the form
       (opExpr, numTerms, rightLeftAssoc, parseAction), where:
        - opExpr is the pyparsing expression for the operator;
           may also be a string, which will be converted to a Literal
        - numTerms is the number of terms for this operator (must
           be 1 or 2)
        - rightLeftAssoc is the indicator whether the operator is
           right or left associative, using the pyparsing-defined
           constants opAssoc.RIGHT and opAssoc.LEFT.
        - parseAction is the parse action to be associated with
           expressions matching this operator expression (the
           parse action tuple member may be omitted)
    """
    ret = Forward()
    lastExpr = baseExpr | (Suppress("(") + ret + Suppress(")"))
    for i, operDef in enumerate(opList):
        opExpr, arity, rightLeftAssoc, pa = (operDef + (None,))[:4]
        thisExpr = Forward().setName("expr%d" % i)
        if rightLeftAssoc == opAssoc.LEFT:
            if arity == 1:
                matchExpr = Group(lastExpr + opExpr)
            elif arity == 2:
                matchExpr = Group(lastExpr + OneOrMore(opExpr + lastExpr))
            else:
                raise ValueError("operator must be unary (1) or binary (2)")
        elif rightLeftAssoc == opAssoc.RIGHT:
            if arity == 1:
                # try to avoid LR with this extra test
                if not isinstance(opExpr, Optional):
                    opExpr = Optional(opExpr)
                matchExpr = FollowedBy(opExpr.expr + thisExpr) + Group(
                    opExpr + thisExpr
                )
            elif arity == 2:
                matchExpr = Group(lastExpr + OneOrMore(opExpr + thisExpr))
            else:
                raise ValueError("operator must be unary (1) or binary (2)")
        else:
            raise ValueError("operator must indicate right or left associativity")
        if pa:
            matchExpr.setParseAction(pa)
        thisExpr << (matchExpr | lastExpr)
        lastExpr = thisExpr
    ret << lastExpr
    return ret


alphas8bit = srange(r"[\0xc0-\0xd6\0xd8-\0xf6\0xf8-\0xff]")
punc8bit = srange(r"[\0xa1-\0xbf\0xd7\0xf7]")

dblQuotedString = Regex(r'"(?:[^"\n\r\\]|(?:"")|(?:\\.))*"').setName(
    "string enclosed in double quotes"
)
sglQuotedString = Regex(r"'(?:[^'\n\r\\]|(?:'')|(?:\\.))*'").setName(
    "string enclosed in single quotes"
)
quotedString = Regex(
    r"""(?:"(?:[^"\n\r\\]|(?:"")|(?:\\.))*")|(?:'(?:[^'\n\r\\]|(?:'')|(?:\\.))*')"""
).setName("quotedString using single or double quotes")

anyOpenTag, anyCloseTag = makeHTMLTags(Word(alphas, alphanums + "_"))
commonHTMLEntity = Combine(
    "&" + oneOf("gt lt amp nbsp quot").setResultsName("entity") + ";"
)
_htmlEntityMap = dict(list(zip("gt lt amp nbsp quot".split(), "><& '")))
replaceHTMLEntity = (
    lambda t: t.entity in _htmlEntityMap and _htmlEntityMap[t.entity] or None
)

# it's easy to get these comment structures wrong - they're very common, so may as well make them available
cStyleComment = Regex(r"/\*(?:[^*]*\*+)+?/").setName("C style comment")

htmlComment = Regex(r"<!--[\s\S]*?-->")
restOfLine = Regex(r".*").leaveWhitespace()
dblSlashComment = Regex(r"\/\/(\\\n|.)*").setName("// comment")
cppStyleComment = Regex(
    r"/(?:\*(?:[^*]*\*+)+?/|/[^\n]*(?:\n[^\n]*)*?(?:(?<!\\)|\Z))"
).setName("C++ style comment")

javaStyleComment = cppStyleComment
pythonStyleComment = Regex(r"#.*").setName("Python style comment")
_noncomma = "".join([c for c in printables if c != ","])
_commasepitem = (
    Combine(
        OneOrMore(Word(_noncomma) + Optional(Word(" \t") + ~Literal(",") + ~LineEnd()))
    )
    .streamline()
    .setName("commaItem")
)
commaSeparatedList = delimitedList(
    Optional(quotedString | _commasepitem, default="")
).setName("commaSeparatedList")


if __name__ == "__main__":

    def test(teststring):
        print(teststring, "->", end=" ")
        try:
            tokens = simpleSQL.parseString(teststring)
            tokenlist = tokens.asList()
            print(tokenlist)
            print("tokens = ", tokens)
            print("tokens.columns =", tokens.columns)
            print("tokens.tables =", tokens.tables)
            print(tokens.asXML("SQL", True))
        except ParseException as err:
            print(err.line)
            print(" " * (err.column - 1) + "^")
            print(err)
        print()

    selectToken = CaselessLiteral("select")
    fromToken = CaselessLiteral("from")

    ident = Word(alphas, alphanums + "_$")
    columnName = delimitedList(ident, ".", combine=True).setParseAction(upcaseTokens)
    columnNameList = Group(delimitedList(columnName))  # .setName("columns")
    tableName = delimitedList(ident, ".", combine=True).setParseAction(upcaseTokens)
    tableNameList = Group(delimitedList(tableName))  # .setName("tables")
    simpleSQL = (
        selectToken
        + ("*" | columnNameList).setResultsName("columns")
        + fromToken
        + tableNameList.setResultsName("tables")
    )

    test("SELECT * from XYZZY, ABC")
    test("select * from SYS.XYZZY")
    test("Select A from Sys.dual")
    test("Select AA,BB,CC from Sys.dual")
    test("Select A, B, C from Sys.dual")
    test("Select A, B, C from Sys.dual")
    test("Xelect A, B, C from Sys.dual")
    test("Select A, B, C frox Sys.dual")
    test("Select")
    test("Select ^^^ frox Sys.dual")
    test("Select A, B, C from Sys.dual, Table2   ")
