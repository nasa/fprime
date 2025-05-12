
// ======================================================================
// \title  TestFPrimeHeaderOverride.cpp
// \author mstarch
// \brief  cpp file for config override testing
//
// \description this file will fail to compile if the DpCfg.hpp header was
//    not overridden using the F Prime configuration build system
//
// \copyright
// Copyright 2025, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include "config/DpCfg.hpp"
static_assert(OVERRIDE_TEST, "OVERRIDE_TEST is not set, header override failed");
              