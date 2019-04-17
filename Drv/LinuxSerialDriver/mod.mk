#
#   Copyright 2004-2008, by the California Institute of Technology.
#   ALL RIGHTS RESERVED. United States Government Sponsorship
#   acknowledged. Any commercial use must be negotiated with the Office
#   of Technology Transfer at the California Institute of Technology.
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

