/*
 * TlmPacketizerTypes.hpp
 *
 *  Created on: Dec 10, 2017
 *      Author: tim
 */

// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef SVC_TLMPACKETIZER_TLMPACKETIZERTYPES_HPP_
#define SVC_TLMPACKETIZER_TLMPACKETIZERTYPES_HPP_

#include <Fw/FPrimeBasicTypes.hpp>
#include <config/TlmPacketizerCfg.hpp>

namespace Svc {

struct TlmPacketizerChannelEntry {
    FwChanIdType id;  //!< Id of channel
    FwSizeType size;  //!< serialized size of channel in bytes
};

struct TlmPacketizerPacket {
    const TlmPacketizerChannelEntry* list;  //!< pointer to a channel entry
    FwTlmPacketizeIdType id;                //!< packet ID
    FwChanIdType level;                     //!< packet level - used to select set of packets to send
    FwChanIdType numEntries;                //!< number of channels in packet
};

struct TlmPacketizerPacketList {
    const TlmPacketizerPacket* list[MAX_PACKETIZER_PACKETS];  //!<
    FwChanIdType numEntries;
};
}  // namespace Svc

#endif /* SVC_TLMPACKETIZER_TLMPACKETIZERTYPES_HPP_ */
