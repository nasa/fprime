

#ifndef RefPackets_header_h
#define RefPackets_header_h

#include <Svc/TlmPacketizer/TlmPacketizerTypes.hpp>

namespace Ref {

    // set of packets to send
    extern const Svc::TlmPacketizerPacketList RefPacketsPkts;
    // set of channels to ignore
    extern const Svc::TlmPacketizerPacket RefPacketsIgnore;

}

#endif // RefPackets_header_h

