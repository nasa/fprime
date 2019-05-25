#!/dsw/Python-2.4.3r1_32/bin/python

import scripts.helpers.target_helper_factory
import scripts.helpers.results_helpers.file_searcher
import scripts.helpers.make_helper
import time
import os
import sys
import optparse

module = os.environ["MODULE"]

print("Build Module %s"%module)

parser = optparse.OptionParser()

#parser.add_option("-r",action="store_true",dest="no_run",help="Build but don't run")
#parser.add_option("-o",default=None, dest="output_dir", help="Set test output directory. If not specified, will be generated.")


# make test binary
scripts.helpers.make_helper.MakeHelper().make_module(module,target="rebuild")

