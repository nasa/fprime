import os

import subprocess
from subprocess import CalledProcessError
import pexpect
from pexpect import EOF, TIMEOUT

#TODO
## Write application name here
class Test_#APPLICATION_NAME#:

    def test_make(self):

        try:
            subprocess.check_output('make', stderr = subprocess.STDOUT, shell=True)
            assert True
        except CalledProcessError as e:
           print("MAKE ERROR")
           print("''''''''''")
           print(e.output)
           assert False

    def test_make_ut(self):

        try:
            subprocess.check_output('make ut', stderr = subprocess.STDOUT, shell=True)
            assert True
        except CalledProcessError as e:
            print("MAKE UT ERROR")
            print("'''''''''''''")
            print(e.output)
            assert False
    def test_run_ut(self):

        try:
            p = pexpect.spawn("make run_ut")

            #
            ## Write pexpect code here
            #

        except EOF: #EOF is thrown when program ends
            assert True
        except TIMEOUT as e:
            print("Timeout. Test Failed.")
            print(e.get_trace())
            assert False

    #TODO
    ## Set app path here
    @classmethod
    def setup_class(cls):
        os.chdir("{BUILD_ROOT}/Autocoders/Python/test/#APP_PATH_HERE#".format(BUILD_ROOT=os.environ.get('BUILD_ROOT')))

    @classmethod
    def teardown_class(cls):

        cleanCmds = ['make clean', 'make ut_clean']
        for cmd in cleanCmds:
            try:
                subprocess.check_output(cmd, stderr = subprocess.STDOUT, shell=True)
            except subprocess.CalledProcessError as e:
                print("MAKE CLEAN ERROR")
                print("''''''''''''''''")
                print(e.output)
        os.chdir("{BUILD_ROOT}/Autocoders/Python/test".format(BUILD_ROOT=os.environ.get('BUILD_ROOT')))

