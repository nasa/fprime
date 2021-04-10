// ======================================================================
// \title  FprimeProtocol.hpp
// \author mstarch
// \brief  hpp file for FprimeProtocol class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Svc/FramingProtocol/FramingProtocol.hpp>
#include <Svc/FramingProtocol/DeframingProtocol.hpp>
#ifndef FPRIMEPROTOCOL_HPP
#define FPRIMEPROTOCOL_HPP

#define FP_FRAME_TOKEN_TYPE U32
#define FP_FRAME_HEADER_SIZE (sizeof(FP_FRAME_TOKEN_TYPE) * 2)

namespace Svc {
/**
 * \brief class implementing the fprime serialization protocol
 */
class FprimeFraming: public FramingProtocol {
  public:
    static const FP_FRAME_TOKEN_TYPE START_WORD;
    FprimeFraming();

    void frame(const U8* const data, const U32 size, Fw::ComPacket::ComPacketType packet_type);
};

class FprimeDeframing : public DeframingProtocol {
  public:
    FprimeDeframing();

    bool validate(Types::CircularBuffer& buffer, U32 size);

    DeframingStatus deframe(Types::CircularBuffer& buffer, U32& needed);
};
};
#endif  // FPRIMEPROTOCOL_HPP
