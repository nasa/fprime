#
#   Copyright 2004-2008, by the California Institute of Technology.
#   ALL RIGHTS RESERVED. United States Government Sponsorship
#   acknowledged. Any commercial use must be negotiated with the Office
#   of Technology Transfer at the California Institute of Technology.
#
#   Information included herein is controlled under the International
#   Traffic in Arms Regulations ("ITAR") by the U.S. Department of State.
#   Export or transfer of this information to a Foreign Person or foreign
#   entity requires an export license issued by the U.S. State Department
#   or an ITAR exemption prior to the export or transfer.
#

# This is a template for the mod.mk file that goes in each module
# and each module's subdirectories.
# With a fresh checkout, "make gen_make" should be invoked. It should also be
# run if any of the variables are updated. Any unused variables can 
# be deleted from the file.

# There are some standard files that are included for reference

SRC =  Simple_ActiveComponentAi.xml \
 	   Simple_Active_TesterComponentAi.xml \
	   Simple_ActiveImpl.cpp \
	   Simple_Active_TesterImpl.cpp \
	   Active_TesterTopologyAppAi.xml \
	   F32PortAi.xml \
	   U32PortAi.xml
	   	   
HDR = Simple_ActiveImpl.hpp \
	  Simple_Active_TesterImpl.hpp



SUBDIRS = test
