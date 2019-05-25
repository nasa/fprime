#
#   Copyright 2004-2008, by the California Institute of Technology.
#   ALL RIGHTS RESERVED. United States Government Sponsorship
#   acknowledged.
#
#

# This is a template for the mod.mk file that goes in each module
# and each module's subdirectories.
# With a fresh checkout, "make gen_make" should be invoked. It should also be
# run if any of the variables are updated. Any unused variables can 
# be deleted from the file.

# There are some standard files that are included for reference

SRC = \	
	LogPortAi.xml \
	LogTextPortAi.xml \
	LogBuffer.cpp \
	LogPacket.cpp \
	LogString.cpp \
	TextLogString.cpp \
	AmpcsEvrLogPacket.cpp
	
HDR = LogBuffer.hpp \
	LogPacket.hpp \
	LogString.hpp \
	TextLogString.hpp \
	AmpcsEvrLogPacket.hpp

SUBDIRS = test
