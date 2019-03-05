#!/bin/python

import subprocess
import sys
import os
import getpass

if len(sys.argv) != 2:
    print("Should be %s <output version file>")
    sys.exit(-1)

output_file = sys.argv[1]

print("Writing version to %s"%output_file)

hash = subprocess.check_output(["git","log","--pretty=format:%h","-n","1","--abbrev=8"]).decode("ascii")
try:
    tag = subprocess.check_output(["git","describe","--abbrev=0","--tags"]).decode("ascii").strip()
except:
    tag = "NOTAG"
    
branch = subprocess.check_output(["git","rev-parse","--abbrev-ref","HEAD"]).decode("ascii").strip()
user = getpass.getuser()

version_file_contents = """
const unsigned int FSW_HASH=0x%s;
const char* FSW_TAG = "%s";
const char* FSW_BRANCH = "%s";
const char* FSW_USER = "%s";
"""

open(output_file,"w").write(version_file_contents % (hash,tag,branch,user))

