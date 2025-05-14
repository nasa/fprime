// ======================================================================
// \title  TestLibraryNewConfig.cpp
// \author mstarch
// \brief  cpp file for new library config testing
//
// \description this file will fail to compile if libraries cannot add new
//     configuration options using the F Prime configuration build system
//
// \copyright
// Copyright 2025, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include "config/config.hpp"
static_assert(MY_LIBRARY_CONFIG, "MY_LIBRARY_CONFIG is not set, library new config failed");