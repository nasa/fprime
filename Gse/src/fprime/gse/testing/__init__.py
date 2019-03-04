'''
F' GDS Testing package

This package allows the use of python's unittest package to run integration testing against a
standard F' deployment via the F' GDS (previously GSE).

@author mstarch
'''
import os
import datetime

#The log directory to be used by the testing package
TESTING_LOG_DIR = os.path.join(os.environ.get("BUILD_ROOT", "/tmp"), "logging", "testing",
                               datetime.datetime.strftime(datetime.datetime.now(),
                                                          "%Y-%m-%dT%H.%M.%S"))