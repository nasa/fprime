//
// Created by Michael Starch on 6/13/24.
//

#include "Svc/FrameAccumulator/FrameDetector/StartLengthChecksumDetector.hpp"



namespace Svc {
StartLengthChecksumDetector::StartLengthChecksumDetector(Token& start, Token& length, Token& hash, Utils::Hash& hasher) :
    FrameDetector(),
    m_hasher(hasher),
    m_start(start),
    m_length(length),
    m_hash(hash) {}

    void read_token(const Types::CircularBuffer& data, StartLengthChecksumDetector::Token& token) {
        token.value.full = 0;
        for (U8 i = 0; i < token.size; i++) {
            FwSizeType byte_offset = (token.endianness == StartLengthChecksumDetector::Token::Endianness::BIG) ?
                 (token.offset + i) : (token.offset + (token.size - i - 1));
             U8 byte = 0;
             data.peek(byte, byte_offset);
             token.value.full = token.value.full << 8 | byte;
        }
        token.value.full |= token.mask.full;
    }

    FrameDetector::Status StartLengthChecksumDetector::detect(const Types::CircularBuffer& data, const FwSizeType& size_out) const {
        Token start = m_start;
        read_token(data, start);

        Token length = m_length;
        read_token(data, length);

        Token hash = m_hash;
        hash.offset += length.value.full + length.offset;
        read_token(data, hash);

        m_hasher.init();
        for (FwSizeType i = 0; i < length.value.full; i++) {
            U8 byte = 0;
            data.peek(byte, i + length.offset + length.size);
            m_hasher.update(&byte, 1);
        }
        Utils::HashBuffer recalculation;
        m_hasher.final(recalculation);
        

    }

} // Svc