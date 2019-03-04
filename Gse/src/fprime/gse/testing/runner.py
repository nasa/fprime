'''
FPrimeTestRunner

A runner used to set the colorization of the results, and interactivity when regarding
suites and tests. This overrides the standard unittest test runner.

@author: mstarch
'''
from __future__ import print_function
import re
import sys
import collections
import unittest

try:
    import colour_runner.result
    TestClass = colour_runner.result.ColourTextTestResult
except ImportError:
    TestClass = unittest.runner.TextTestResult

class FPrimeTestRunner(unittest.runner.TextTestRunner):
    '''
    An FPrime variant of the standard text runner used to set the test
    runner's result class to use the colorized variant if it exists
    '''
    resultclass = TestClass
    NO_OPT = 0
    YES_OPT = 1
    ALL_OPT = 2
    YES_NO_RE = re.compile("^((?:yes)|(?:no)|(?:y)|(?:n))$", re.IGNORECASE)
    YES_NO_ALL_RE = re.compile("^((?:yes)|(?:no)|(?:y)|(?:n)|(?:all)|(?:a))$", re.IGNORECASE)

    def __init__(self, *args, **kwargs):
        '''
        Setup this test, passing in input to super
        @param args: arguments (positional) to forward
        @param kwargs: key word arguments (interactive) will be stripped before
                       pass through to super
        '''
        self.interactive = False
        #Interactive KWArgs striping
        if "interactive" in kwargs:
            self.interactive = kwargs["interactive"]
            del kwargs["interactive"]
        self.testbed = {"name": "default"}
        #testbed KWArgs striping
        if "testbed" in kwargs:
            self.testbed = kwargs["testbed"]
            del kwargs["testbed"]
        #no_color
        if "no_color" in kwargs:
            self.no_color = kwargs["no_color"]
            del kwargs["no_color"]
            if self.no_color:
                FPrimeTestRunner.resultclass = unittest.runner.TextTestResult
        if "list_only" in kwargs:
            self.list_only = kwargs["list_only"]
            del kwargs["list_only"]
            if self.list_only:
                self.interactive = True #Needed to iterate tests
        else:
            self.list_only = False
        unittest.runner.TextTestRunner.__init__(self, *args, **kwargs)

    def run(self, test):
        '''
        Run the test
        '''
        print_top = True
        spec = self.recurse_for_tests(test)
        for name, tests in spec.iteritems():
            #Get the test suite documentation
            doc = "No suite documentation"
            if tests:
                _, first_func = tests[0]
                doc = sys.modules[first_func.__module__].__doc__
            suite_resp = self.get_interactive_response(name, doc=doc)
            #Loop through the suite asking for each if required
            for indv, func in tests:
                #Check global responses
                if suite_resp == self.ALL_OPT:
                    continue
                elif suite_resp == self.NO_OPT:
                    self.skip(func)
                    continue
                #Get individual response
                resp = self.get_interactive_response(indv, self.YES_NO_RE, doc=func.__doc__)
                if resp == self.NO_OPT:
                    self.skip(func)
        return unittest.runner.TextTestRunner.run(self, test)

    @staticmethod
    def skip(func):
        '''
        Skip a given test
        @param func: function to skip
        '''
        #Python 2/3
        try:
            setattr(func, "__unittest_skip__", True)
            setattr(func, "__unittest_skip_why__", "User interactively skipped test")
        except AttributeError:
            setattr(func.__func__, "__unittest_skip__", True)
            setattr(func.__func__, "__unittest_skip_why__", "User interactively skipped test")

    def recurse_for_tests(self, test, rdict=None):
        '''
        Recurse through the TestSuite to look for individual tests, and then roll up the tests
        by module in order to get a list that we can interact with.
        @param test: test or testsuite
        '''
        #Prevent run-on-init default parameters
        if rdict is None:
            rdict = collections.OrderedDict()
        #If it is not a test suite, add test
        if not hasattr(test, "_tests"):
            module = test.__module__
            name = test._testMethodName
            test = getattr(test, name)
            tup = (name, test)
            #Setup dictionary entry if not there and add the tuple
            if not module in rdict:
                rdict[module] = []
            rdict[module].append(tup)
        #For test cases, recurse
        else:
            for tmp in getattr(test, "_tests"):
                self.recurse_for_tests(tmp, rdict)
        return rdict

    def get_interactive_response(self, test, reg=YES_NO_ALL_RE, doc="No documentation provided"):
        '''
        Gets an interactive response from a user. It must match one of the following:
          - y or yes: run this test (don't skip)
          - n or no: do not run this script (skip)
          - a or all: run all the tests in this test case
        @param test: test/test suite name
        @return: YES, NO, ALL
        '''
        #Short circuit a response if non-interactive and skipped tests
        if not self.interactive:
            return self.ALL_OPT
        #Attempted, not always working
        elif getattr(test, "__unittest_skip__", False):
            return self.YES_OPT
        #Check for test suit
        prompt = "(yes, no, all)" if reg is self.YES_NO_ALL_RE else "(yes, no)"
        #Match a response
        text = ""
        if self.list_only:
            tlisting = "'{0}':\n{1}\nTests:" if reg is self.YES_NO_ALL_RE else "    - {0}:{1}"
            print(tlisting.format(test, doc))
            return self.YES_OPT if reg is self.YES_NO_ALL_RE else self.NO_OPT#Need a response
        else:
            while not reg.match(text):
                text = raw_input("Run {0} {1}:".format(test, prompt))
        text = reg.match(text).group(1).lower()
        return self.YES_OPT if text.startswith("y") else \
            (self.ALL_OPT if text.startswith("a") else self.NO_OPT)
