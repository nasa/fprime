#
#   Copyright 2004-2008, by the California Institute of Technology.
#   ALL RIGHTS RESERVED. United States Government Sponsorship
#   acknowledged.
#
#

SRC = LinuxSerialDriverComponentAi.xml LinuxSerialDriverComponentImplCommon.cpp

SRC_SDFLIGHT = LinuxSerialDriverComponentImpl.cpp

SRC_LINUX = LinuxSerialDriverComponentImpl.cpp

SRC_CYGWIN = LinuxSerialDriverComponentImpl.cpp

SRC_DARWIN = LinuxSerialDriverComponentImplStub.cpp

SRC_RASPIAN = LinuxSerialDriverComponentImpl.cpp

SRC_LINUXRT = LinuxSerialDriverComponentImpl.cpp

HDR = LinuxSerialDriverComponentImpl.hpp

SUBDIRS = test

