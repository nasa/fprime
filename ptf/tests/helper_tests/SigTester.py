#!/dsw/Python-2.4.3r1_32/bin/python

import scripts.helpers.target_helper_factory
import scripts.helpers.master_helper_factory
import scripts.helpers.simulation_helper_factory
import scripts.helpers.gds_helper_factory
import time
import os


master_helper = scripts.helpers.master_helper_factory.MasterHelperFactory().getMasterHelper()
simulation_helper = scripts.helpers.simulation_helper_factory.SimulationHelperFactory().getSimulationHelper()
gds_helper = scripts.helpers.gds_helper_factory.GdsHelperFactory().getGdsHelper()
target_helper = scripts.helpers.target_helper_factory.TargetHelperFactory().getTargetHelper()

time.sleep(5)

master_helper.exit()
target_helper.exit()
simulation_helper.exit()
gds_helper.exit()

print("Interactive session finished.")


