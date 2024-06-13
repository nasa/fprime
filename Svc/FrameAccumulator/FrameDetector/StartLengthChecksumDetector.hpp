//
// Created by Michael Starch on 6/13/24.
//

#ifndef SVC_FRAMEACCUCULATOR_FRAME_DETECTOR_STARTLENGTHCHECKSUMDETECTOR_HPP
#define SVC_FRAMEACCUCULATOR_FRAME_DETECTOR_STARTLENGTHCHECKSUMDETECTOR_HPP
#include "Fw/Types/PolyType.hpp"
#include "Utils/Hash/Hash.hpp"
#include "Svc/FrameAccumulator/FrameDetector.hpp"


namespace Svc {
    class StartLengthChecksumDetector : public FrameDetector {
       public:
        struct Token {
            enum Endianness {
                BIG,
                LITTLE
            };
            union Type  {
                U8 u8;
                U16 u16;
                U32 u32;
                U64 u64;
                U64 full;
            };
            Endianness endianness;
            FwSizeType size;
            FwSizeType offset;
            Type value;
            Type mask;
        };

        StartLengthChecksumDetector(Token& start, Token& length, Token& hash, Utils::Hash& hasher);

        FrameDetector::Status detect(const Types::CircularBuffer& data, const FwSizeType& size_out) const;

        Utils::Hash& m_hasher;
        Token& m_start;
        Token& m_length;
        Token& m_hash;
        FwSizeType m_length_offset;

    };

} // Svc

#endif //SVC_FRAMEACCUCULATOR_FRAME_DETECTOR_STARTLENGTHCHECKSUMDETECTOR_HPP
