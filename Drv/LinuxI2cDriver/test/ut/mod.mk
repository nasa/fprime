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

TEST_SRC = 	TesterBase.cpp 	\
			GTestBase.cpp \
			Tester.cpp \
			main.cpp

TEST_MODS = Drv/LinuxI2cDriver \
			Drv/I2cDriverPorts \
			gtest \
			Fw/Comp \
			Fw/Port \
			Fw/Types \
			Fw/Obj \
			Os \
			Fw/Buffer
