#
#   Copyright 2004-2015, by the California Institute of Technology.
#   ALL RIGHTS RESERVED. United States Government Sponsorship
#   acknowledged.
#
#

SRC = HealthComponentAi.xml HealthComponentImpl.cpp

HDR = HealthComponentImpl.hpp

SUBDIRS = test

SRC_LINUX = Stub/HealthComponentStubChecks.cpp	

SRC_CYGWIN = Stub/HealthComponentStubChecks.cpp	

SRC_DARWIN = Stub/HealthComponentStubChecks.cpp

SRC_RASPIAN = Stub/HealthComponentStubChecks.cpp	
