#!python

import sys
import subprocess
import os
import time
import signal
from optparse import OptionParser

#TODO remove, debugging
from pprint import pprint

def main(argv=None):

    # Get a path to the python binary we are using to launch processes
    python_bin = "python"

    # Get a path to the root of the reference application build directory
    build_root = os.environ["BUILD_ROOT"]

    # Parse options on the command line
    parser = OptionParser()
    parser.add_option("-p", "--port", dest="port", action="store", type="int", help="Set the threaded TCP socket server port [default: 50000]", default=50000)
    parser.add_option("-a", "--addr", dest="addr", action="store", type="string", help="set the threaded TCP socket server address [default: %default]", default="0.0.0.0")
    parser.add_option("-n", "--nobin", dest="nobin", action="store_true", help="Disables the binary app from starting [default: %default]", default=False)
    parser.add_option("-t", "--twin", dest="twin", action="store_true", help="Runs Threaed TCP Server in window, otherwise backgrounds [default: %default]", default=False)

    (opts, args) = parser.parse_args(argv)
    used_port = opts.port
    nobin = opts.nobin
    addr = opts.addr
    twin = opts.twin


    ################## Run the ThreadedTCPServer.py #################################
    if twin:
        TTS_args = [python_bin,"%s/Gds/bin/wxgui/pexpect_runner.py"%build_root,
                    "ThreadedTCP.log","Threaded TCP Server",
                    python_bin,"%s/Gds/bin/ThreadedTCPServer.py"%build_root,
                    "--port","%d"%used_port, "--host",addr]
        TTS = subprocess.Popen(TTS_args)
    else:
        tts_log = open("ThreadedTCP.log",'w')
        TTS_args = [python_bin, "-u",
                    "%s/Gds/bin/ThreadedTCPServer.py"%build_root,
                    "--port","%d"%used_port, "--host",addr]
        TTS = subprocess.Popen(TTS_args,stdout=tts_log,stderr=subprocess.STDOUT)

    #################################################################################

    # Wait for TCP Server to start
    time.sleep(2)



    ########################### Run the GUI for the GDS #############################

    GUI_args = [python_bin,"%s/Gds/bin/wxgui/gds.py"%build_root,
                "--port","%d"%used_port,
                "--dictionary","%s/Ref/py_dict"%build_root,
                "--addr",addr,"-L","%s/Ref/logs"%build_root,
                "--config","%s/Ref/gds.ini"%build_root]


    #print ("GUI: %s"%" ".join(GUI_args))
    GUI = subprocess.Popen(GUI_args)

    #################################################################################


    ################################ Run Ref app ####################################

    ref_bin = "%s/Ref/%s/Ref"%(build_root,os.environ["OUTPUT_DIR"])

    if not nobin:
        #print("running ref app\n\n\n\n")
        REF_args = [python_bin,"%s/Gds/bin/wxgui/pexpect_runner.py"%build_root,
                    "Ref.log","Ref Application",ref_bin,"-p","%d"%used_port,
                    "-a",addr]
        REF = subprocess.Popen(REF_args)

    #################################################################################

    # Wait for GUI process to close
    #print("Waiting for GUI")
    GUI.wait()
    #print("GUI Closed")


    if not nobin:
        #print("Waiting for App")
        try:
            REF.send_signal(signal.SIGTERM)
        except:
            pass

        try:
            REF.wait()
        except:
            pass
        #print("App Closed")


    #print("Waiting for TTS")

    try:
        TTS.send_signal(signal.SIGINT)
    except:
        pass

    try:
        TTS.wait()
    except:
        pass

    #print("TTS Closed")

if __name__ == "__main__":
    sys.exit(main())
