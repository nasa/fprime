#!/dsw/Python-2.4.3r1_32/bin/python

import scripts.helpers.target_helper_factory
import scripts.helpers.results_helpers.file_searcher
import scripts.helpers.make_helper
import time
import os
import sys
import optparse

module = os.environ["MODULE"]

print("Module %s unit test"%module)

#parser = optparse.OptionParser()

#parser.add_option("-r",action="store_true",default="false",dest="no_run",help="Build but don't run")

# build module
scripts.helpers.make_helper.MakeHelper().make_module(module)
# make test binary
scripts.helpers.make_helper.MakeHelper().make_unit_test(module)
# run test script
scripts.helpers.make_helper.MakeHelper().run_unit_test(module)
