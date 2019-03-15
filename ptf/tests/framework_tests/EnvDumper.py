#!/dsw/Python-2.4.3r1_32/bin/python

import os

for var in list(os.environ.keys()):
    print("%s = %s" % (var,os.environ[var]))