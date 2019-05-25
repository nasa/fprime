#!/tps/bin/python

import sys
import os

filename = "%s"%sys.argv[1]
hash_val = hash(filename)% 0xFFFFFFFF # force to 32-bits
hash_arg = "0x%08X"%(hash_val) 

# open local hash file
open("%s/mk/hash/%s"%(os.environ["BUILD_ROOT"],filename.replace(".","_")+".hash"),"w").write("%s: %s"%(hash_arg,filename))
# open fragment file
#open("%s/mk/xml/%s"%(os.environ["BUILD_ROOT"],filename.replace(".","_")+".xml_frag"),"w").write("                    <value fsw_value=\"%s\" numeric_value=\"%s\" comment=\"hash value for file %s\" dict_value=\"%s\"/>\n"%(filename.replace(".","_").replace("-","_").replace("+","_"),hash_val,filename,filename))

print(("%s"%hash_arg))
#sys.stderr.write("file: %s hash: %d\n"%(sys.argv[1],hash(sys.argv[1])))