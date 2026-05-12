# ======================================================================
# \title  Svc/AssertFatalAdapter/test/ut/AcConstantsOverride.fpp
# \brief  Override constants for AssertFatalAdapter unit tests
#
# \copyright
# Copyright 2009-2015, by the California Institute of Technology.
# ALL RIGHTS RESERVED.  United States Government Sponsorship
# acknowledged.
# ======================================================================

@ Override for testing: Limit max string length to less than the max file length
constant AssertFatalAdapterEventFileSize = FileNameStringSize - 10
