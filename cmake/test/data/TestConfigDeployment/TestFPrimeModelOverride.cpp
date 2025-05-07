
// ======================================================================
// \title  TestFPrimeModelOverride.cpp
// \author mstarch
// \brief  cpp file for config override testing
//
// \description this file will fail to compile if the model (FpConfig.fpp)
//    was not overridden using the F Prime configuration build system
//
// \copyright
// Copyright 2025, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#include <type_traits>
#include <cstdint>
#include "config/FwChanIdTypeAliasAc.h"
static_assert(std::is_same<FwChanIdType,uint64_t>::value, "FwChanIdType not a U64, model override failed");