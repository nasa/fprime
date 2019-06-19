"""
test_enum.py:

Checks that the enum xml is being correctly autocoded and imported into
components, porttypes, and serializables.

@author jishii
"""

import os

import subprocess
from subprocess import CalledProcessError
import pexpect
from pexpect import TIMEOUT, EOF

import logging

def test_enum():
    """
    Tests that enum xml is being generated correctly
    """
    try:
        ## Spawn executable
        p = pexpect.spawn("make")
        
        logging.debug('`g++` executed.')
        #logging.debug('Executable now running.')
        
        #for pout, psend in $test_case.ptuples:
        #logging.debug('Expecting: ${pout}')
        
        p.expect("1", timeout=3)
        p.sendline("2")
        
        #logging.debug('Sent: ${psend}')
        #end for
        
        logging.debug('No timeout. Asserting test as true.')
        ## If there was no timeout the pexpect test passed
        assert True
        
        ## A timeout occurs when pexpect cannot match the executable
        ## output with the designated expectation. In this case the
        ## key expectation is p.expect(expect_string, timeout=3)
        ## which tests what the method name describes
    except TIMEOUT as e:
        print("Timeout Error. Expected Value not returned.")
        print ("-------Program Output-------")
        print (p.before)
        print ("-------Expected Output-------")
        print (e.get_trace())
        assert False
