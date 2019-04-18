#
#   Copyright 2004-20015, by the California Institute of Technology.
#   ALL RIGHTS RESERVED. United States Government Sponsorship
#   acknowledged. Any commercial use must be negotiated with the Office
#   of Technology Transfer at the California Institute of Technology.
#
#

TEST_SRC = LogTest.cpp

TEST_MODS = Fw/Log Fw/Com Fw/Types Fw/Obj Fw/Port Fw/Time gtest


COMPARGS = -I$(CURDIR)/test/ut/Handcode
