/*
 * TlmPacketizerComponentImplCfg.hpp
 *
 *  Created on: Dec 10, 2017
 *      Author: tim
 */

// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef SVC_TLMPACKETIZER_TLMPACKETIZERCOMPONENTIMPLCFG_HPP_
#define SVC_TLMPACKETIZER_TLMPACKETIZERCOMPONENTIMPLCFG_HPP_

#include <Fw/FPrimeBasicTypes.hpp>

namespace Svc {
static const FwChanIdType MAX_PACKETIZER_PACKETS = 200;

// Works best when set to about twice the number of components producing telemetry
static const FwChanIdType TLMPACKETIZER_NUM_TLM_HASH_SLOTS = 15;  // !< Number of slots in the hash table.

// Should be set to a little below the ID gaps to spread the entries around
static const FwChanIdType TLMPACKETIZER_HASH_MOD_VALUE = 99;  // !< The modulo value of the hashing function.

// Buckets must be >= number of telemetry channels in system
static const FwChanIdType TLMPACKETIZER_HASH_BUCKETS = 1000;  // !< Buckets assignable to a hash slot.

static const FwChanIdType TLMPACKETIZER_MAX_MISSING_TLM_CHECK = 25;  // !< Max number of missing channel checks

// packet update mode
enum PacketUpdateMode {
    PACKET_UPDATE_ALWAYS,              // Always send packets, even if no changes to channel data
    PACKET_UPDATE_ON_CHANGE,           // Only send packets if any of the channels updates
    PACKET_UPDATE_AFTER_FIRST_CHANGE,  // Always send packets, but only after first channel has been updated
};

static const PacketUpdateMode PACKET_UPDATE_MODE = PACKET_UPDATE_ON_CHANGE;
}  // namespace Svc

#endif /* SVC_TLMPACKETIZER_TLMPACKETIZERCOMPONENTIMPLCFG_HPP_ */
