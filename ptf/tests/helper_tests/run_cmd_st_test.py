#!/dsw/Python-2.4.3r1_32/bin/python

import scripts.helpers.target_helper_factory
import scripts.helpers.master_helper_factory
import scripts.helpers.simulation_helper_factory
import scripts.helpers.results_helpers.file_searcher
import os
import sys
import time

def exitAll():
    master_helper.exit()
    simulation_helper.exit()
    target_helper.exit()


console_output = scripts.helpers.results_helpers.file_searcher.FileSearcher()

master_helper = scripts.helpers.master_helper_factory.MasterHelperFactory().getMasterHelper()
master_helper.start_master()

simulation_helper = scripts.helpers.simulation_helper_factory.SimulationHelperFactory().getSimulationHelper()
simulation_helper.start_simulation()

target_helper = scripts.helpers.target_helper_factory.TargetHelperFactory().getTargetHelper()

target_helper.genStdScript()
target_helper.start_target()

#Give the target a little time to load FSW
time.sleep(3)

target_helper.run_cmd("i")

#The run_cmd() routine's second argument is an optional timeout.  By default, the routine will wait
#forever until the vxworks prompt (->) is return before logging the output.  If you set a timeout, the
#output will be logged when the prompt is returned or if the timeout is reached.
target_helper.run_cmd("</proj/msl/fsw/dev/hanry/test_script", 5)

# Start FSW execution
done = False
maxTime = 5000
currentTime = 0
while ((done == False) and (currentTime < maxTime)):
    master_helper.run(500)
    if (console_output.find(os.environ["TARGET_CONSOLE_STDOUT"],"Initialization Complete:") != 0):
        done = True
    currentTime += 500

if (done == False):
     print("Didn't find FSW initialization message.")
     exitAll() 
     sys.exit(-1) 
     
#Flush out current output buffer (messages from FSW initialization)
target_helper.run_cmd()

target_helper.run_cmd("pwd")

target_helper.run_cmd("</proj/msl/fsw/dev/hanry/test_script")

target_helper.run_cmd("printf(\"%f\\n\",tim_get_sc_time_f64())")

target_helper.run_cmd("printf(\"%i\\n\",tim_get_elapsed_time_1hz)")

target_helper.run_cmd("ld < " + os.environ["BUILD_ROOT"] + "/osal/sim/" + os.environ["COMPILER_dir"] + "/osal_dependent_sim.o")

target_helper.run_cmd("moduleShow")

target_helper.run_cmd("osal_print(\"hello, world!\\n\")")

target_helper.run_cmd("osal_print(\"   val =  2 something\\n\")")

  
print("vxsim_st test completed successfully.")

exitAll()
sys.exit(0)

