# implements a helper to start and interact with a linux instance

import scripts.helpers.target_helper
import scripts.helpers.process_helper
import subprocess
import os
import signal
import time

class IsfLinuxHelper(scripts.helpers.target_helper.TargetHelper):
    def __init__(self,instance_id):
        scripts.helpers.helper.Helper.__init__(self)
        self.instance_id = instance_id
        self.started = False
        if "DUAL_RCE" in os.environ:
            self.max_session_id_char = 24
        else:
            self.max_session_id_char = 30


    def start_target(self):
        #@todo: Make this start ISF reference applicaiton Ref...
        
        #@todo: Create both linux and mac term commands here..
        if (self.instance_id == None):
            xterm_name = os.environ["session_id"][0:self.max_session_id_char]
        else:
            xterm_name = self.instance_id  + "-" + os.environ["session_id"][0:self.max_session_id_char] 

        spawn_cmd = os.environ["XTERM_LOG"]
        arg_list = "-l","-lf"
        if (self.instance_id == None):
            arg_list += os.path.abspath(os.curdir+"/linux.log"),
        else:
            arg_list += os.path.abspath(os.curdir+"/linux." + self.instance_id + ".log"),

        title = xterm_name
        arg_list += "-sl","20000","-title",title,"-n",title
            
        if "vxsim_geometry" in os.environ:
            arg_list += "-geometry",os.environ["vxsim_geometry"]

        #@todo: new isf Ref execution line here
        #TBD fsw_file = os.environ["BUILD_ROOT"] + "/fsw/" + os.environ["COMPILER_dir"]+"/fsw"
        # LJR
        #fsw_file = os.environ["BUILD_ROOT"] + "/fsw/" + os.environ["COMPILER_dir"] + "/fsw"
        fsw_file = os.environ["BUILD_ROOT"] + os.sep + os.environ['DEPLOYMENT'] + os.sep + os.environ["COMPILER_dir"] + os.sep + os.environ['DEPLOYMENT']

        # Start 
        self.linux_helper = scripts.helpers.process_helper.BackgroundProcess()
        self.linux_helper.start(spawn_cmd, arg_list, os.path.abspath(os.curdir+"/linux.stdout"), os.path.abspath(os.curdir+"/linux.stderr"))
        os.environ["TARGET_CONSOLE_STDOUT"]=os.path.abspath(os.curdir+"/linux.stdout")
        os.environ["CMD_STDOUT"] = os.path.abspath(os.curdir+"/cmds.stdouterr")
        self.started = True

    def exit(self):
        if self.started:
            self.linux_helper.kill(signal.SIGKILL)

    def wait(self):
        print("base wait")

