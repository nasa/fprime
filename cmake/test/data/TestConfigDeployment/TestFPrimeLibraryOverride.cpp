
// ======================================================================
// \title  TestFprimeLibraryOverride.cpp
// \author mstarch
// \brief  cpp file for config override testing
//
// \description this file will fail to compile if the library was unable to
//     override a file from the F Prime configuration build system support.
//
// \copyright
// Copyright 2025, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include "config/StaticMemoryConfig.hpp"
static_assert(MY_LIBRARY_OVERRIDE, "MY_LIBRARY_OVERRIDE is not set, library override failed");