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

#ifndef SVC_FPRIME_PROTOCOL_HPP
#define SVC_FPRIME_PROTOCOL_HPP

#include <Svc/FramingProtocol/FramingProtocol.hpp>
#include <Svc/FramingProtocol/DeframingProtocol.hpp>

namespace Svc {

  // Definitions for the F Prime frame header
  namespace FpFrameHeader {

    //! Token type for F Prime frame header
    using TokenType = U32;

    enum {
      //! Header size for F Prime frame header
      SIZE = sizeof(TokenType) * 2
    };

    //! The start word for F Prime framing
    const TokenType START_WORD = static_cast<TokenType>(0xdeadbeef);

  }

  //! \brief Implements the F Prime framing protocol
  class FprimeFraming: public FramingProtocol {
    public:

      FprimeFraming();

      void frame(
          const U8* const data,
          const U32 size,
          Fw::ComPacket::ComPacketType packet_type
      );

  };

  //! \brief Implements the F Prime deframing protocol
  class FprimeDeframing : public DeframingProtocol {
    public:

      FprimeDeframing();

      bool validate(Types::CircularBuffer& buffer, U32 size);

      DeframingStatus deframe(Types::CircularBuffer& buffer, U32& needed);

  };

};
#endif  // SVC_FPRIME_PROTOCOL_HPP
