#
#   Copyright 2004-2015, by the California Institute of Technology.
#   ALL RIGHTS RESERVED. United States Government Sponsorship
#   acknowledged. Any commercial use must be negotiated with the Office
#   of Technology Transfer at the California Institute of Technology.
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
