// ======================================================================
// \title  Fw/FPrimeBasicTypes.h
// \author mstarch
// \brief  header file for basic types used in F Prime
//
// \copyright
// Copyright 2025, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// FPrime defines a number of basic types, platform configurable types,
// and project configurable types. This file provides a single header
// users can import for using these types.
//
// This header is intended to be C-compatible.
//
// ======================================================================
#ifndef FPRIME_BASIC_TYPES_H
#define FPRIME_BASIC_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif
#include <Fw/Types/BasicTypes.h>
#include <Platform/PlatformTypes.h>
#include <config/FpConfig.h>

// ----------------------------------------------------------------------
// Type aliases defined by FPP
// ----------------------------------------------------------------------

#include <config/FwAssertArgTypeAliasAc.h>
#include <config/FwIndexTypeAliasAc.h>
#include <config/FwQueuePriorityTypeAliasAc.h>
#include <config/FwSignedSizeTypeAliasAc.h>
#include <config/FwSizeTypeAliasAc.h>
#include <config/FwTaskIdTypeAliasAc.h>
#include <config/FwTaskPriorityTypeAliasAc.h>

#include <config/FwChanIdTypeAliasAc.h>
#include <config/FwDpIdTypeAliasAc.h>
#include <config/FwDpPriorityTypeAliasAc.h>
#include <config/FwEnumStoreTypeAliasAc.h>
#include <config/FwEventIdTypeAliasAc.h>
#include <config/FwIdTypeAliasAc.h>
#include <config/FwOpcodeTypeAliasAc.h>
#include <config/FwPacketDescriptorTypeAliasAc.h>
#include <config/FwPrmIdTypeAliasAc.h>
#include <config/FwSizeStoreTypeAliasAc.h>
#include <config/FwSizeTypeAliasAc.h>
#include <config/FwTimeBaseStoreTypeAliasAc.h>
#include <config/FwTimeContextStoreTypeAliasAc.h>
#include <config/FwTlmPacketizeIdTypeAliasAc.h>
#include <config/FwTraceIdTypeAliasAc.h>

// Backwards naming compatibility.
typedef FwSizeStoreType FwBuffSizeType;
#define PRI_FwBuffSizeType PRI_FwSizeStoreType

#ifdef __cplusplus
}
#endif
#endif  // FPRIME_BASIC_TYPES_H
