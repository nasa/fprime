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
//! Maximum number of packets that the packetizer can handle.
static const FwChanIdType MAX_PACKETIZER_PACKETS = 50;

//! Maximum number of channels that the packetizer can handle. Must be >= number of non-omitted channels
static const FwChanIdType MAX_PACKETIZER_CHANNELS = 200;

//! Maximum number of missing channels to track and report
static const FwChanIdType TLMPACKETIZER_MAX_MISSING_TLM_CHECK = 25;

}  // namespace Svc

#endif /* SVC_TLMPACKETIZER_TLMPACKETIZERCOMPONENTIMPLCFG_HPP_ */
