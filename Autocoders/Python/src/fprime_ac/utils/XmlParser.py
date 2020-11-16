#!/usr/bin/env python3
# ===============================================================================
# NAME: XmlParser.py
#
# DESCRIPTION:  This is a general xml parser class based on
#               the pythom xml2obj receipe.
#
# USAGE:
#
# AUTHOR: reder
# EMAIL:  reder@jpl.nasa.gov
# DATE CREATED  : Aug. 14, 2007
#
# Copyright 2007, California Institute of Technology.
# ALL RIGHTS RESERVED. U.S. Government Sponsorship acknowledged.
# ===============================================================================
#
# Python standard modules
#
import logging
import os

from genshi import XML

#
# Python extention modules and custom interfaces
#

#
# Universal globals used within module go here.
# (DO NOT USE MANY!)
#
# Global logger init. below.
PRINT = logging.getLogger("output")
DEBUG = logging.getLogger("debug")
#
class Parser:
    """
    An XML parser class that uses Genshi streams to consume an XML document
    into a tree of Element objects. The class is instanced with an optional
    XML file argument. User define external validation visitors can be attached
    to validate the particulars of the XML and then the method parse() returns
    the root Element object of the tree of parsed XML.
    """

    __root = None
    __node_stack = None
    __node_end_visit_dict = None
    __node_start_visit_dict = None

    __xml_string = None

    def get_node_stack(self):
        return self.__node_stack

    def __init__(self, xml_file=None):
        """
        Given a well formed XML file (xml_file), read it and turn it into
        a big string.
        """

        self.__node_stack = list()
        self.__node_end_visit_dict = dict()
        self.__node_start_visit_dict = dict()

        if xml_file is None:
            self.__xml_string = None
        else:
            self.__xml_string = self._make_string(xml_file)

    def _make_string(self, filename):
        """
        Convert and return the filename as a single string.
        """

        if os.path.isfile(filename) == False:
            str = "ERROR: Could not find specified XML file %s." % filename
            PRINT.info(str)
            raise OSError(str)

        return open(filename).read()

    def addStartVisitor(self, element_name, visitor):
        """
        Attach a visitor to the start of the element
        to validate.

        Visitor is best a method of a derived class,
        in which case will receive the 'self' of the derived
        class of XmlParser.Parser: useful!!

        The visitor will receive the element (class XmlParser.Element)
        that was just started, therefore can check only attributes and
        NOT content.

        Special value of ANY_XML_ELEMENT_NAME can be used to connect a
        single visitor to the start of any element: might be useful for debugging.
        """
        self.__node_start_visit_dict[element_name] = visitor

    def exampleStartVisitor(self, element):
        """
        This is an example of a method you might register via addStartVisitor.
        It might be useful to see your XML as its parsed in during debugging.
        """
        if True != isinstance(element, Parser.Element):
            print("Usage: exampleStartVisitor( Parser.Element )")
            print(
                "Argument is not a Parser.Element (it is a %s)"
                % element.__class__.__name__
            )
            raise TypeError(
                "XmlParser.Parser.exampleStartVisitor argument"
                + " is not a Parser.Element (it is a %s)" % element.__class__.__name__
            )

        s = "exampleStartVisitor element %s attributes=[" % element.getName()
        for attrName in list(element.getAttr().keys()):
            attrValue = element.getAttr(attrName)
            s += ' %s="%s"' % attrName, attrValue
        s += " ]"
        print(s)

    def addEndVisitor(self, element_name, visitor):
        """
        Attach a visitor to the end of the element
        to validate.

        Visitor is best a method of a derived class,
        in which case will receive the 'self' of the derived
        class of XmlParser.Parser: useful!!

        The visitor will receive the element (class XmlParser.Element)
        that was just started, therefore can check BOTH attributes AND
        content. But usually, better to report on content only, as
        the line containing the start is hard to get here.

        Special value of ANY_XML_ELEMENT_NAME can be used to connect a
        single visitor to the start of any element: might be useful for
        debugging.
        """

        self.__node_end_visit_dict[element_name] = visitor

    def parse(self):
        """
        Read through the stringified XML and parse it into a tree of Element
        objects. This method assumes a filename was specified when the class
        constructor was called. Otherwise, there will be nothing to parse.

        A new tree of Element objects is created each time this method is
        called. This is not the way to process multiple XML files into a
        single parse tree.
        """

        if self.__xml_string is None:
            return
        else:
            self.__node_stack = list()

        for token, content, loc in XML(self.__xml_string):

            # This next line presents a problem procesing XML with special
            # formatting characters. It generates an exception. Since it is
            # only debug, we'll just comment this out until the XML is
            # updated to remove the special characters.
            #
            #            DEBUG.debug( 'token, content, loc are %s=%s %s=%s %s=%s'
            #                         % (type(token).__name__,str(token),
            #                            type(content).__name__,str(content),
            #                            type(loc).__name__,str(loc) ) )

            if token == "START":
                name = content[0]
                attr = content[1]
                self._startElement(name, attr)
            elif token == "TEXT":
                self._cData(content)
            elif token == "END":
                name = content[0]
                self._endElement(name)

        return self.__root

    def parse_file(self, filename):
        """
        Parse the specified XML file into a tree of Element objects. Since the
        objects are appended to any existing Element objects, this method can
        be used to parse multiple XML files into a single tree. Just call this
        method with each filename.
        """

        root_name_checked = False

        xml_text = self._make_string(filename)

        for token, content, loc in XML(xml_text):

            if token == "START":

                name = content[0]
                attr = content[1]

                # We are building a tree of Element objects. The issue with
                # parsing multiple files is that each XML file needs a common
                # root Element to form the top of the tree. Therefore, the
                # requirement for parsing a second file is that it has a same
                # named root Element.

                if self.__root is not None and root_name_checked == False:

                    if self.__root.getName() == name:

                        # We already have a root element, and that root element
                        # has the same name as the root element in this second
                        # file. Continue with the parse.

                        # Potential issue is that doing this will not call the
                        # registered start visitor. Is that okay since we are
                        # treating it as a common node.

                        self.__node_stack.append(self.__root)

                    else:

                        # We already have a root element, but that root element
                        # name does not match the root element name from the
                        # previously parsed file. Stop since this will result
                        # in an orphaned tree branch.

                        print(
                            "XML file (%s) has invalid root name: %s (expected: %s)."
                            % (filename, name, self.__root.getName())
                        )
                        return

                else:
                    self._startElement(name, attr)

                root_name_checked = True

            elif token == "TEXT":
                self._cData(content)

            elif token == "END":
                name = content[0]
                self._endElement(name)

        return self.__root

    def _startElement(self, name, attr):
        """
        Start element handler.
        """

        # Instantiate an Element object
        element = Element(name, attr)

        # Push element onto the stack and make it a child of parent
        if len(self.__node_stack) > 0:
            parent = self.__node_stack[-1]
            parent.addChild(element)
        else:
            self.__root = element

        self.__node_stack.append(element)

        if name in list(self.__node_start_visit_dict.keys()):
            self.__node_start_visit_dict[name](self._Parser__node_stack[-1])

    def _endElement(self, name):
        """
        End element event handler.
        """
        name = self.__node_stack[-1].getName()
        if name in list(self.__node_end_visit_dict.keys()):
            self.__node_end_visit_dict[name](self.__node_stack[-1])

        self.__node_stack = self.__node_stack[:-1]

    def _cData(self, data):
        """
        Data event handler.
        """
        if data.strip():
            data = data.strip()
            element = self.__node_stack[-1]
            element.cdata += data
            return

    def validateElement(self, element, validAttrs):
        """
        Validate the attributes of a single element. Note that attributes
        cannot be repeated, or its not valid XML, so we won't even get here
        with repeated attributes.

        Returns five items as a list, so use like this:

        (probs,vals,badAttrs,errMsg) = self.validateElement(e,listOfAttrNames)

        @return problems: a count of the number of problems found.
        @return value: a dictionary, values indexed by attribute name
        @return invalidAttrs: a list of names of unexpected attributes.
        @return errorMsg: a single line message describing the problems,

        if any. It is not printed herein: the caller can print this message if
        desired. It always has something in it, even if no problems, so it can
        be used as the beginning of an error message for more detailed checks
        done by the caller (subclass' element validation method).
        """

        problems = 0
        value = dict()
        invalidAttrs = list()

        errorMsg = "Error: XML element <%s> is invalid" % (element.getName())

        for attrName in validAttrs:
            value[attrName] = None

        for attrName in list(element.getAttr().keys()):
            attrValue = element.getAttr(attrName)
            if attrName in validAttrs:
                value[attrName] = attrValue
            else:
                invalidAttrs.append(attrName)

        for elemName in validAttrs:
            if not value[elemName]:
                errorMsg += '; did not find "%s" attribute' % elemName
                problems += 1
        if len(invalidAttrs) != 0:
            for attrName in invalidAttrs:
                errorMsg += '; found unexpected attribute "%s"' % attrName
                problems += 1

        return (problems, value, invalidAttrs, errorMsg)

    def countSubtree(self, element, expectedChildren):
        """
        Count the children of an element that have names (XML tags) from
        a specified list. The shape of the tree is not specified, so this only
        confirms the existence, and the count of, specified elements, but does
        not confirm order nor depth nor the shape of the subtree.

        Returns four items as a list, so use like this:

        (found,unexpectedChildren,errMsg) = self.validateSubtree(e,listOfExpectedChildrenNames)

        @param element: The root of a tree of elements.
        @param expectedChildren: Names of children nodes.

        @return found: a dictionary, indexed by child name, containing the count of
        children of each name.
        @return unexpectedChildren: a list of names of unexpected children elements.
        @return errMsg: an error message. Never printed herein, and always non-empty even
        if no problems, so can be used as the beginning of an error message for more
        detailed validation performed by the caller (the subclass' element validation
        methods).
        """
        found = dict()
        unexpectedChildren = list()
        errMsg = "Error: The XML element <%s> is invalid" % (element.getName())

        children = element.getElements()

        for child in children:
            childName = child.getName()
            DEBUG.debug(
                "countSubtree: element {} childName={}".format(
                    element.getName(), childName
                )
            )
            if childName in expectedChildren:
                try:
                    found[childName] += 1
                    DEBUG.debug(
                        "duplicate %s for %d time" % (childName, found[childName])
                    )
                except KeyError:
                    found[childName] = 1
                    DEBUG.debug("first %s" % childName)
            else:
                unexpectedChildren.append(childName)
                DEBUG.debug("unexpected %s" % childName)

        return (found, unexpectedChildren, errMsg)

    def dump(self):
        """
        Dump from root over the tree.
        """
        if self.__root is None:
            return

        elist = self.__root.getElements()
        if len(elist) > 0:
            for e in elist:
                print("Element: %s" % e.getName())
                print("Data: %s" % e.getData())
                print("Attr:")
                for attr in list(e.getAttr().keys()):
                    print("  {} = {}".format(attr, e.getAttr(attr)))
                child = e.getElements()
                self.dump2(child)

    def dump2(self, element, level=1):
        """
        Called by dump to recurse down into command XML.
        """
        elist = element
        if len(elist) == 0:
            return
        else:
            for e in elist:
                tabs = level * "  "
                print("{}Element: {}".format(tabs, e.getName()))
                print("{}Data: {}".format(tabs, e.getData()))
                print("%sAttr:" % tabs)
                for attr in list(e.getAttr().keys()):
                    print("{}{} = {}".format(tabs, attr, e.getAttr(attr)))
                self.dump2(e.getElements(), level + 1)


class Element:
    """
    A parsed XML element.
    """

    def __init__(self, name, attributes):
        """
        Element constructor.
        """
        # The element's tag name
        self.name = name
        # The element's attribute dictionary
        if len(attributes) > 0:
            attr = dict(attributes)
        else:
            attr = dict()
        self.attribute = attr
        # The element's cdata
        self.cdata = ""
        # The element's child element list (sequence)
        self.children = []

    def addChild(self, element):
        """
        Add a reference to a child element object.
        """
        self.children.append(element)

    def getAttr(self, key=None):
        """
        Get an attribute value.
        """
        if key is None:
            return self.attribute
        else:
            return self.attribute.get(key)

    def getData(self):
        """
        Get the cdata.
        """
        return self.cdata

    def getName(self):
        """
        Get the tag name for this element.
        """
        return self.name

    def getElements(self, name=""):
        """
        Get a list of child elements

        If no tag name is given, return list of all children.
        else return only those children with a matching tag name.
        """

        if not name:
            return self.children
        else:
            elements = list()
            for element in self.children:
                if element.name == name:
                    elements.append(element)
            return elements

    def __getitem__(self, item):
        """
        Make getData, getName and getElemnets
        look like dictionary calls.
        """
        if item == "attr":
            e = self.getAttr()
        elif item == "cdata":
            e = self.getData()
        elif item == "element":
            e = self.getName()
        elif item == "child":
            e = self.getElements()
        else:
            e = self.getElements(item)
        return e


def node_visit(element):
    """
    Demonstration node visitor
    """
    print("Visit element %s" % element.getName())
    print("Visit data    %s" % element.getData())
    print("Visit attr    %s" % element.getAttr())


if __name__ == "__main__":

    xmlfile1 = os.environ["MSL_ROOT"] + "/cmd/cmd_ai_ipc.xml"
    xmlfile2 = os.environ["MSL_ROOT"] + "/apxs/apxs_ai_ipc.xml"

    print("IPC parse test (genshi)")

    # There are two ways to use this class:
    #  1. Pass file to constructor, and call parse method
    #  2. Use no arg constructor and call parse_file method
    #
    #    p = Parser(xmlfile1)
    p = Parser()

    #    p.addStartVisitor('id',node_visit)
    #    p.addEndVisitor('max',node_visit)

    #    root = p.parse()
    root = p.parse_file(xmlfile2)

    print("Dump the xml file %s" % xmlfile1)
    p.dump()

    for iface in root.getElements("iface"):
        print(iface.getElements("id")[0].getData())
