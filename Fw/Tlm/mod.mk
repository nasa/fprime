#
#   Copyright 2004-2015, by the California Institute of Technology.
#   ALL RIGHTS RESERVED. United States Government Sponsorship
#   acknowledged.
#
#

SRC = \	
	TlmPortAi.xml \
	TlmBuffer.cpp \
	TlmPacket.cpp \
	TlmString.cpp
	
HDR = TlmBuffer.hpp \
	TlmPacket.hpp \
	TlmString.hpp

SUBDIRS = test
