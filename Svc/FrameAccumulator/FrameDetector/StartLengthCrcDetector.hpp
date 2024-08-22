// ======================================================================
// \title  StartLengthCrcDetector.hpp
// \author mstarch
// \brief  hpp file for start/length/crc frame detector definitions
// ======================================================================
#ifndef SVC_FRAMEACCUCULATOR_FRAME_DETECTOR_STARTLENGTHCHECKSUMDETECTOR_HPP
#define SVC_FRAMEACCUCULATOR_FRAME_DETECTOR_STARTLENGTHCHECKSUMDETECTOR_HPP
#include "Fw/Types/PolyType.hpp"
#include "Svc/FrameAccumulator/FrameDetector.hpp"
#include "Utils/Hash/libcrc/lib_crc.h"
#include <type_traits>

namespace Svc {
namespace FrameDetectors {

//! \brief endianness of the token word
enum Endianness {
    BIG,    //!< Token word is big-endian
    LITTLE  //!< Token word is little-endian
};

// CRC uses library defined with non fixed size types.  Check for compliance with correct fixed size types.
static_assert(sizeof(unsigned long) >= sizeof(U32), "CRC32 cannot fit in CRC32 library chosen types");
static_assert(sizeof(unsigned short) >= sizeof(U16), "CRC16 cannot fit in CRC16 library chosen types");

//! \breif base template definition of a "token"
//!
//! A token is a field that can be read from the circular buffer in order to detect a frame. These tokens could be start
//! words, lengths, crcs, etc. Tokens are static with respect to a given framing protocol, but are parameterized across
//! several concepts:
//!   1. Token type: type of the token's containing word (e.g. U32, U16, U8)
//!   2. Token mask: mask used to pull the token from the containing word. Used for bit fields.
//!   3. Token endianness: endianness of the stored token word
//! \tparam TokenType: template parameter setting token word's type. Must be unsigned.
//! \tparam TokenMask: template parameter storing mask for token word. Expressed in type "TokenType"
//! \tparam TokenEndianness: template parameter setting endianness of token word. Endianness::BIG or Endianness::LITTLE.
template <typename TokenType,
          TokenType TokenMask = std::numeric_limits<TokenType>::max(),
          Endianness TokenEndianness = Endianness::BIG>
class Token {
    // Checks to ensure token parameters are well-formed
    static_assert(!std::numeric_limits<TokenType>::is_signed, "Tokens must be unsigned");
    static_assert(sizeof(TokenType) < std::numeric_limits<U8>::max(), "Token sizes must fit in a unsigned byte");
    static_assert(TokenEndianness == Endianness::BIG || TokenEndianness == Endianness::LITTLE, "Invalid endianness");

  public:
    // \brief zero out m_value
    Token(): m_value(0) {}

    //! \brief read token from circular buffer without consuming data
    //!
    //! Reads the token from the circular buffer by reading the token's containing word, correcting for endianness, and
    //! applying the token mask. The read data is set in the member variable: m_value. `size_out` parameter is updated
    //! to add in the size of the token read regardless of any errors. Will return "MORE_DATA_NEEDED" on error or
    //! "FRAME_DETECTED" on success.
    //! \param data: circular buffer to peek into
    //! \param offset: offset into the circular buffer to read
    //! \param size_out: updated to add in token size
    //! \return: status of the success/failure of the read
    FrameDetector::Status read(const Types::CircularBuffer& data, FwSizeType offset, FwSizeType& size_out) {
        // Update size_out
        constexpr FwSizeType token_size = sizeof(TokenType);
        size_out += token_size;

        // Read token byte by byte correcting for endianness
        TokenType token = 0;
        for (U8 i = 0; i < static_cast<U8>(token_size); i++) {
            // Read most significant remaining byte regardless of endianness
            FwSizeType byte_offset =
                (TokenEndianness == Endianness::BIG) ? (offset + i) : (offset + (token_size - i - 1));
            U8 byte = 0;
            Fw::SerializeStatus status = data.peek(byte, byte_offset);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->m_value = 0;
                return FrameDetector::Status::MORE_DATA_NEEDED;
            }
            // Shift in most significant remaining byte
            token = token << 8 | byte;
        }
        // Mask and set token value
        token &= TokenMask;
        this->m_value = token;
        return FrameDetector::FRAME_DETECTED;
    }

    //! \brief get value of token after read, or 0 before
    const TokenType& getValue() { return m_value; }

  protected:
    TokenType m_value;
};

//! \brief subclass of Token representing the start token/start word of the frame
//!
//! Most frames have a start word or start token indicating the beginning of a frame. This token subclass represents
//! this concept in a parameterizable way. Start tokens are parameterized across the following:
//!   1. TokenType: same as token type from parent "Token" class.
//!   2. StartExpected: value of token indicating start of frame as TokenType. e.g. 0xdeadbeef(U32) for fprime
//!   3. TokenMask: same as token mask from parent class.  Also applied to StartExpected
//!   4. TokenEndianness: same as token endianness from parent class.
//!
//! Start words read the token from the circular buffer, and then compare against the expected value after each is
//! masked. If the comparison passes, frame detection continues with "FRAME_DETECTED" otherwise "NO_FRAME_DETECTED" is
//! returned to indicate the current position in the buffer is *not* a frame.
//! \tparam TokenType: template parameter setting token word's type. Must be unsigned.
//! \tparam StartExpected: template parameter setting the expected start word pre-mask.
//! \tparam TokenMask: template parameter storing mask for token word. Expressed in type "TokenType"
//! \tparam TokenEndianness: template parameter setting endianness of token word. Endianness::BIG or Endianness::LITTLE.
template <typename TokenType,
          TokenType StartExpected,
          TokenType TokenMask = std::numeric_limits<TokenType>::max(),
          Endianness TokenEndianness = Endianness::BIG>
class StartToken : public Token<TokenType, TokenMask, TokenEndianness> {
  public:
    //! \breif read start token and determine if match with expected start token
    //!
    //! This will read the start token from the circular buffer. It then compares to expected to determine if there is
    //! a frame start or no. This will return FRAME_DETECTED if the start word is detected. It will return
    //! NO_FRAME_DETECTED if there was no match, and MORE_DATA_NEEDED if there is not enough data in underlying read.
    //! \param data: circular buffer to read
    //! \param size_out: size returned updated to include size of start token.
    //! \return: FRAME_DETECTED, NO_FRAME_DETECTED, or MORE_DATA_NEEDED.
    FrameDetector::Status read(const Types::CircularBuffer& data, FwSizeType& size_out) {
        constexpr TokenType EXPECTED_VALUE = (StartExpected & TokenMask);

        FrameDetector::Status status = this->Token<TokenType, TokenMask, TokenEndianness>::read(data, 0, size_out);
        if ((status == FrameDetector::FRAME_DETECTED) && (this->m_value != EXPECTED_VALUE)) {
            status = FrameDetector::NO_FRAME_DETECTED;
        }
        return status;
    }
};

//! \breif token representing data length
//!
//! Length of the data field is found somewhere in the data payload at a specified offset. This template has the
//! standard Token functions and an additional template parameter "Offset" used to show the offset of the length token.
//! \tparam TokenType: template parameter setting token word's type. Must be unsigned.
//! \tparam Offset: template parameter setting offset of length word
//! \tparam StartExpected: template parameter setting the expected start word pre-mask.
//! \tparam TokenMask: template parameter storing mask for token word. Expressed in type "TokenType"
//! \tparam TokenEndianness: template parameter setting endianness of token word. Endianness::BIG or Endianness::LITTLE.
template <typename TokenType,
          FwSizeType Offset = 0,
          FwSizeType MaximumLength=1024,
          TokenType TokenMask = std::numeric_limits<TokenType>::max(),
          Endianness TokenEndianness = Endianness::BIG>
class LengthToken : public Token<TokenType, TokenMask, TokenEndianness> {
  public:
    //! \breif read length token and return total size of packet through length token
    //!
    //! This reads the length token from the buffer and returns the total needed size up through the length token. If
    //! the length exceeds the maximum, then NO_FRAME_DETECTED is returned. size_out does not include the length of the
    //! data as that is up to the caller. MORE_DATA_NEEDED is returned when insufficient data is available to read the
    //! length token.  FRAME_DETECTED is return when a valid size token was read
    //! \param data: buffer to read
    //! \param size_out: total packet length up through end of length token
    //! \return: FRAME_DETECTED, or MORE_DATA_NEEDED.
    FrameDetector::Status read(const Types::CircularBuffer& data, FwSizeType& size_out) {
        size_out = Offset;
        FrameDetector::Status status = this->Token<TokenType, TokenMask, TokenEndianness>::read(data, size_out, size_out);
        if (this->m_value > MaximumLength) {
            return FrameDetector::Status::NO_FRAME_DETECTED;
        }
        return status;
    }
};
template <typename TokenType>
class CRCWrapper {
  protected:
    //! \brief constructor setting initial value
    CRCWrapper(TokenType initial) : m_crc(initial) {}
  public:
    //! \brief update CRC with one new byte
    //!
    //! Update function for CRC taking previous value from member variable and updating it.
    //!
    //! \param new_byte: new byte to add to calculation
    virtual void update(U8 new_byte) = 0;

    //! \brief finalize and return CRC value
    virtual TokenType finalize() = 0;

  protected:
    TokenType m_crc;
};
//! \brief standard CRC32 implementation
class CRC32 : public CRCWrapper<U32> {
  public:
    CRC32() : CRCWrapper<U32>(std::numeric_limits<U32>::max()) {}

    //! \brief update CRC with one new byte
    //!
    //! Update function for CRC taking previous value from member variable and updating it.
    //!
    //! \param new_byte: new byte to add to calculation
    void update(U8 new_byte) override {
        this->m_crc =  static_cast<U32>(update_crc_32(static_cast<U32>(m_crc), new_byte));
    };

    //! \brief finalize and return CRC value
    U32 finalize() override {
        // Bitwise not also works, but the CRC standard requests XOR calculation instead
        return this->m_crc ^ std::numeric_limits<U32>::max();
    };
};

//! \brief token representing the CRC
//!
//! CRC checksum template used to calculate the CRC of the *entire* packet except the stored CRC value itself. This is
//! parameterized on the following:
//!   1. TokenType (same as Token class)
//!   2. DataOffset: offset of data block. Used to calculate the full packet length including the data.
//!   3. RelativeTokenOffset: offset from end of data (length token) to where the CRC stored value
//!   4 CRCFn: function used to update CRC for each byte. Must match the TokenType.
//! This token will update size_out to include the whole packet (data block and CRC). It allows calculating the CRC on
//! the packet data and reading the sent CRC from the buffer.
//! \tparam TokenType: template parameter setting token word's type. Must be unsigned.
//! \tparam DataOffset: offset of variable data block in packet
//! \tparam RelativeTokenOffset: offset relative to end of data to where CRC is stored
//! \tparam CRCFn: function for updating CRC with new byte
template <typename TokenType,
          FwSizeType DataOffset,
          FwSizeType RelativeTokenOffset = 0,
          class CRCHandler = CRC32>
class CRC : public Token<TokenType, std::numeric_limits<TokenType>::max(), BIG> {
  public:
    // Check CRC token and CRC handler match
    static_assert(std::is_base_of<CRCWrapper<TokenType>, CRCHandler>::value, "Invalid CRC wrapper supplied");

    CRC() : m_stored_offset(0), m_expected(0) {}
    //! \brief calculate CRC across whole packet
    //!
    //! This will read in the whole packet (start word through but not including stored CRC) and calculate the CRC using
    //! the parameterized function. It then negates (1's complements) the result per the CRC guidance. The initial
    //! value is 0xffffffff converted to the appropriate size for the CRC. This will return FRAME_DETECTED
    //! if there is data or MORE_DATA_NEEDED if the whole packet is not available.  This will set m_stored_offset and
    //! m_expected in preparation for a read call.  m_stored_offset will be 0 if not called, or error occurred.
    //! \param data: buffer to read
    //! \param length: variable data length read from LengthToken
    //! \param size_out: updated to include full length of packet (less stored CRC)
    //! \return: FRAME_DETECTED, or MORE_DATA_NEEDED.
    FrameDetector::Status calculate(const Types::CircularBuffer& data, FwSizeType length, FwSizeType& size_out) {
        const FwSizeType checksum_length = DataOffset + length + RelativeTokenOffset;
        size_out = checksum_length;
        CRCHandler crc;
        // Loop byte by byte
        for (FwSizeType i = 0; i < checksum_length; i++) {
            U8 byte = 0;
            Fw::SerializeStatus status = data.peek(byte, i);
            if (status != Fw::SerializeStatus::FW_SERIALIZE_OK) {
                this->m_stored_offset = 0;
                return FrameDetector::Status::MORE_DATA_NEEDED;
            }
            crc.update(byte);
        }
        // CRC finalization ends in bit-wise negation
        this->m_stored_offset = checksum_length;
        this->m_expected = crc.finalize();
        return FrameDetector::FRAME_DETECTED;
    }
    //! \brief read stored CRC and check against recalculation
    //!
    //! `calculate` must be called first. This will read the stored CRC and check against the value calculated during
    //! the `calculate` step. size_out will be updated to the full packet size (including CRC at the end). This will
    //! return FRAME_DETECTED if the CRC is valid (as is the whole frame) or MORE_DATA_NEEDED if the whole packet
    //! is not including the trailing CRC, and NO_FRAME_DETECTED if the CRC fails.
    //! \param data: buffer to read for data
    //! \param size_out: size of the whole packet
    //! \return: FRAME_DETECTED, NO_FRAME_DETECTED, or MORE_DATA_NEEDED.
    FrameDetector::Status read(const Types::CircularBuffer& data, FwSizeType& size_out) {
        FW_ASSERT(this->m_stored_offset != 0); // Must have called calculate before calling read
        size_out = this->m_stored_offset;
        FrameDetector::Status status =
            this->Token<TokenType, std::numeric_limits<TokenType>::max(), BIG>::read(data, size_out, size_out);
        if ((status == FrameDetector::FRAME_DETECTED) && (this->m_value != this->m_expected)) {
            status = FrameDetector::NO_FRAME_DETECTED;
        }
        return status;
    }

  protected:
    FwSizeType m_stored_offset;
    TokenType m_expected;
};
//! \breif start/length/crc detector template
//!
//! Most packets are of the form start token, length token, data, and a trailing CRC. This template is used to quickly
//! implement detectors that follow the above pattern by supplying configured tokens for each concept. In shor, a dev
//! should create a protocol specific StartToken, LengthToken, and Checksum and then template this class using those
//! specified tokens.  That fully specified class can be supplied to FrameAccumulator for detection.
//!
//! For example see: FprimeFrameDetector.hpp
//! \tparam StartToken: start token specification
//! \tparam LengthToken: length token specification
//! \tparam Checksum: checksum token specification
template <class StartToken, class LengthToken, class Checksum>
class StartLengthCrcDetector : public FrameDetector {
  public:
    //! \brief detect if a frame is available in circular buffer
    //!
    //! Detects if the frame is available by checking start word, length, and checksum in order. Returns FRAME_DETECTED
    //! when the whole frame is available, NO_FRAME_DETECTED when there is no frame at the head of the buffer, and
    //! MORE_DATA_NEEDED if the frame is valid (so far) but incomplete. size_out specifies the size needed to perform
    //! the next read of the next token.
    //! \param data: circular buffer
    //! \param size_out: set to needed size. Note: may not be full frame size, just to read the next token.
    //! \return: FRAME_DETECTED, NO_FRAME_DETECTED, or MORE_DATA_NEEDED.
    Status detect(const Types::CircularBuffer& data, FwSizeType& size_out) const override {
        // Read start word
        StartToken startWord;
        Status status = startWord.read(data, size_out);
        if (status != Status::FRAME_DETECTED) {
            return status;
        }

        // Read length
        LengthToken lengthWord;
        status = lengthWord.read(data, size_out);
        if (status != Status::FRAME_DETECTED) {
            return status;
        }
        FwSizeType length = lengthWord.getValue();

        // Recalculate CRC
        Checksum crc;
        status = crc.calculate(data, length, size_out);
        if (status != Status::FRAME_DETECTED) {
            return status;
        }
        // Read CRC
        status = crc.read(data, size_out);
        return status;
    };
};
}  // namespace FrameDetectors
}  // namespace Svc

#endif  // SVC_FRAMEACCUCULATOR_FRAME_DETECTOR_STARTLENGTHCHECKSUMDETECTOR_HPP
