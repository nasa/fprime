'''
Support module for F Prime testing

Contains support functions and classes used in the F Prime testing
modules.

@author: mstarch
'''
import inspect
import unittest

class StreamForker(object):
    '''
    Forks a stream into N number of output streams, enabling one-to-many expansion of stream
    writes. This enables output logging to both the console (sys.stderr) and to a file at the
    same time.
    '''
    def __init__(self, streams=None):
        '''
        Init this object with empty list of streams
        @param streams: list of streams to log to
        '''
        if streams is None:
            streams = []
        self.streams = streams
    def write(self, buffer_data):
        '''
        Write the input buffer to all child buffers
        @param buffer_data: buffer to write to all childs
        '''
        for stream in self.streams:
            stream.write(buffer_data)
    def flush(self):
        '''
        Flushes all streams
        '''
        for stream in self.streams:
            stream.flush()

class FPrimePrettyPrint(type):
    '''
    Python metaclass --
    This metaclass is used to help in the construction of the FPrimeTestCase
    in order to ensure that the "setUp" method of any test case inheriting
    from FPrimeTestCase is properly wrapped in order to ensure that a
    carriage return is printed, making the output look nice.
    '''
    def __new__(mcs, name, bases, attrs):
        '''
        Use this "new" function when creating a "new" class and make sure
        that there is always a setUp function and that the carriage_return
        flag is properly set.
        '''
        upfn = attrs.get("setUp", None)
        #If setup was defined, then wrap it, otherwise ignore
        if upfn is not None:
            def wrap_up(self):
                '''
                Wraps the setup function to set a variable before running it.
                '''
                self.carriage_return = True
                upfn(self)
            attrs["setUp"] = wrap_up
        return super(FPrimePrettyPrint, mcs).__new__(mcs, name, bases, attrs)

class TestCaseNotConfigured(Exception):
    '''
    Exception for unconfigured child
    '''
    def __init__(self, name):
        message = "TestCase '{0}' does not set 'gds_config' before __init__".format(name)
        super(TestCaseNotConfigured, self).__init__(message)

def get_verbosity():
    '''
    Searches up the stack for TestProgram, then grabs verbosity
    Borrowed from: stackoverflow.com/questions/13761697/
    '''
    frame = inspect.currentframe()
    while frame:
        slf = frame.f_locals.get('self')
        if isinstance(slf, unittest.TestProgram):
            return slf.verbosity
        frame = frame.f_back
    return 0

def get_testbed():
    '''
    Searches up the stack for TestProgram, then grabs testbed
    Borrowed from: stackoverflow.com/questions/13761697/
    '''
    frame = inspect.currentframe()
    while frame:
        slf = frame.f_locals.get('self')
        if hasattr(slf, "testbed"):
            return slf.testbed
        frame = frame.f_back
    return {"name": "default"}
