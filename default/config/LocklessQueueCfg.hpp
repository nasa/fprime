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
//! lock-free on the target platform (never-lock-free widths are rejected at compile time; the
//! authoritative check is runtime-asserted at create()). Platforms without lock-free 64-bit atomics may configure a
//! narrower type (U32, U16, or U8), shrinking the ABA tag by the same amount (62, 30, 14, or 6
//! tag bits respectively; see SDD sections 4 and 15 for the wrap consequence and a WARNING on
//! the narrow-tag ordering risk).
using LocklessStateTagType = U64;

//! Backoff (microseconds) between bounded scans on the BLOCKING send/receive paths. Shorter
//! values lower message latency; longer values lower idle CPU use of blocked threads. Must be
//! greater than 0: a zero backoff could livelock on a strict-priority scheduler.
constexpr U32 LOCKLESS_QUEUE_BLOCKING_BACKOFF_US = 100;

//! Alignment (bytes) of each queue slot. Set to the target's cache-line size (typically 64) to
//! avoid false sharing between adjacent slots under contention; smaller power-of-two values
//! reduce memory use on constrained targets.
constexpr FwSizeType LOCKLESS_QUEUE_SLOT_ALIGNMENT = 64;

//! Retry passes through the slot array before a NONBLOCKING operation returns FULL/EMPTY.
//! Larger values reduce spurious FULL/EMPTY under contention at the cost of a larger bound
//! on non-blocking call time (`depth * passes`).
constexpr FwSizeType LOCKLESS_QUEUE_MAX_RETRY_PASSES = 4;

}  // namespace Generic
}  // namespace Os

#endif  // CONFIG_LOCKLESSQUEUECFG_HPP
