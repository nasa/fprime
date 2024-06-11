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

#include <FpConfig.hpp>
#include <TlmPacketizerCfg.hpp>

namespace Svc {

struct TlmPacketizerChannelEntry {
    FwChanIdType id;        //!< Id of channel
    NATIVE_UINT_TYPE size;  //!< serialized size of channel in bytes
};

struct TlmPacketizerPacket {
    const TlmPacketizerChannelEntry* list;  //!< pointer to a channel entry
    FwTlmPacketizeIdType id;                //!< packet ID
    NATIVE_UINT_TYPE level;                 //!< packet level - used to select set of packets to send
    NATIVE_UINT_TYPE numEntries;            //!< number of channels in packet
};

struct TlmPacketizerPacketList {
    const TlmPacketizerPacket* list[MAX_PACKETIZER_PACKETS];  //!<
    NATIVE_UINT_TYPE numEntries;
};
}  // namespace Svc

#endif /* SVC_TLMPACKETIZER_TLMPACKETIZERTYPES_HPP_ */
