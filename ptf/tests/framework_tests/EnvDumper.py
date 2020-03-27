import os

for var in list(os.environ.keys()):
    print("%s = %s" % (var,os.environ[var]))
