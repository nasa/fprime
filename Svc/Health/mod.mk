#
#   Copyright 2004-2015, by the California Institute of Technology.
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

SRC = HealthComponentAi.xml HealthComponentImpl.cpp

HDR = HealthComponentImpl.hpp

SUBDIRS = test

SRC_LINUX = Stub/HealthComponentStubChecks.cpp	

SRC_CYGWIN = Stub/HealthComponentStubChecks.cpp	

SRC_DARWIN = Stub/HealthComponentStubChecks.cpp

SRC_BAERAD750 = VxWorks/HealthComponentVxWorksChecks.cpp

SRC_SPHINX = VxWorks/HealthComponentVxWorksChecks.cpp
