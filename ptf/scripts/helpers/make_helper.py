# make file helper
from __future__ import absolute_import
import subprocess
import os
import sys
import scripts.helpers.process_helper

class MakeHelper:
    
    def make_deployment(self, target, deployment, comp="DEFAULT"):
        
        # Build make command
        
        curdir = os.getcwd()
        os.chdir(os.environ["BUILD_ROOT"] + "/" + deployment)    
        cmd = os.environ["HOST_MAKE"] + " Makefile " + target
        
        print("cmd: %s" % cmd)
        
        (status, output) = scripts.helpers.process_helper.getstatusoutput(cmd)
        os.chdir(curdir)
        
        open("make." + deployment.replace("/","_") + ".log", "w").write(cmd + "\n\n" + output)
        print(output)
        
        if status != 0:
            print("Error building deployment %s:\n%s" % (deployment, output)) 
            sys.exit((status & 0xFF00) >> 8)
        
    def make_module(self, module, target="all"):
        
        # Build make command
        curdir = os.getcwd()
        os.chdir(os.environ["BUILD_ROOT"] + "/" + module)    
        cmd = os.environ["HOST_MAKE"] + " -f " + os.environ["BUILD_ROOT"] + "/" + module + "/Makefile " + target

        print("cmd: %s\npwd: %s" % (cmd, os.getcwd()))
        
        (status, output) = scripts.helpers.process_helper.getstatusoutput(cmd)
        os.chdir(curdir)
        
        open(curdir + "/module_make." + module.replace("/","_") + ".log", "w").write(cmd + "\n\n" + output)
        
        if status != 0:
            print("Error building module %s target %s:\n%s" % (module, target, output)) 
            sys.exit((status & 0xFF00) >> 8)
    
    def clean_module(self, module, target="clean_all"):
        
        # Build make command
        curdir = os.getcwd()
        os.chdir(os.environ["BUILD_ROOT"] + "/" + module)    
        cmd = os.environ["HOST_MAKE"] + " -f " + os.environ["BUILD_ROOT"] + "/" + module + "/Makefile " + target

        print("cmd: %s\npwd: %s" % (cmd, os.getcwd()))
        
        (status, output) = scripts.helpers.process_helper.getstatusoutput(cmd)
        os.chdir(curdir)
        
        open(curdir + "/module_make." + module.replace("/","_") + ".log", "w").write(cmd + "\n\n" + output)
        
        if status != 0:
            print("Error building module %s target %s:\n%s" % (module, target, output)) 
            sys.exit((status & 0xFF00) >> 8)
    
    def make_unit_test(self, module, rebuild=False):
        
        curdir = os.getcwd()
        os.chdir(os.environ["BUILD_ROOT"] + "/" + module)    
        cmd = os.environ["HOST_MAKE"] + " -f " + os.environ["BUILD_ROOT"] + "/" + module + "/Makefile "
        if rebuild:
            cmd += "ut_clean "
        cmd += "ut"

        print("cmd: %s\npwd: %s" % (cmd, os.getcwd()))
        
        (status, output) = scripts.helpers.process_helper.getstatusoutput(cmd)
        os.chdir(curdir)
            
        open(curdir + "/unit_test_build." + module.replace("/","_") + ".log", "w").write(cmd + "\n\n" + output)
        
        if status != 0:
            print("Error building module %s unit test:\n%s" % (module,output)) 
            sys.exit((status & 0xFF00) >> 8)

    def run_unit_test(self, module):
        
        curdir = os.getcwd()
        os.chdir(os.environ["BUILD_ROOT"] + "/" + module)    
        cmd = os.environ["HOST_MAKE"] + " -f " + os.environ["BUILD_ROOT"] + "/" + module + "/Makefile run_ut"

        print("cmd: %s\npwd: %s" % (cmd, os.getcwd()))
        
        (status, output) = scripts.helpers.process_helper.getstatusoutput(cmd)
        os.chdir(curdir)
            
        open(curdir + "/unit_test_run." + module.replace("/","_") + ".log", "w").write(cmd + "\n\n" + output)
        print(output)
        
        if status != 0:
            print("Error building module %s unit test:\n%s" % (module,output)) 
            sys.exit((status & 0xFF00) >> 8)
            
    # if tester = true, it will copy Tester.hpp/.cpp
    def gen_unit_test(self, module,tester):
        curdir = os.getcwd()
        os.chdir(os.environ["BUILD_ROOT"] + "/" + module)    
        cmd = os.environ["HOST_MAKE"] + " -f " + os.environ["BUILD_ROOT"] + "/" + module + "/Makefile testcomp"

        print("cmd: %s\npwd: %s" % (cmd, os.getcwd()))
        
        (status, output) = scripts.helpers.process_helper.getstatusoutput(cmd)
            
        if status != 0:
            print("Error generating module %s unit test code:\n%s" % (module,output)) 
            sys.exit((status & 0xFF00) >> 8)
        
        cmd = os.environ["HOST_MAKE"] + " -f " + os.environ["BUILD_ROOT"] + "/" + module + "/Makefile testcomp"
        if tester:
            tester = "Tester.*"
        else:
            tester = ""
        cmd = "cp GTestBase.* TesterBase.* %s test/ut" % tester

        print("cmd: %s\npwd: %s" % (cmd, os.getcwd()))
        
        (status, output) = scripts.helpers.process_helper.getstatusoutput(cmd)
        os.chdir(curdir)
            
        if status != 0:
            print("Error copying module %s unit test code:\n%s" % (module,output)) 
            sys.exit((status & 0xFF00) >> 8)
        
