// ======================================================================
// \title  LocklessQueueCfg.hpp
// \brief  configuration for Os::Generic::LocklessPriorityQueue
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#ifndef CONFIG_LOCKLESSQUEUECFG_HPP
#define CONFIG_LOCKLESSQUEUECFG_HPP

#include "Fw/FPrimeBasicTypes.hpp"

namespace Os {
namespace Generic {

//! Unsigned integral type of the lockless queue slot state-tag word. The low STATE_BITS hold the
//! slot state; the remaining bits hold the ABA epoch tag. `std::atomic` of this type must be
//! lock-free on the target platform (statically asserted where the platform guarantees it and
//! runtime-asserted at create()). Platforms without lock-free 64-bit atomics may configure U32,
//! narrowing the ABA tag (see SDD sections 4 and 15 for the wrap consequence).
using LocklessStateTagType = U64;

}  // namespace Generic
}  // namespace Os

#endif  // CONFIG_LOCKLESSQUEUECFG_HPP
