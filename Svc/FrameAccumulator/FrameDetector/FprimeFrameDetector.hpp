// ======================================================================
// \title  FprimeFrameDetector.hpp
// \author thomas-bc
// \brief  hpp file for fprime frame detector definitions
// ======================================================================
#ifndef SVC_FRAME_ACCUMULATOR_FRAME_DETECTOR_FPRIME_FRAME_DETECTOR
#define SVC_FRAME_ACCUMULATOR_FRAME_DETECTOR_FPRIME_FRAME_DETECTOR

#include "Svc/FrameAccumulator/FrameDetector.hpp"


#include "Fw/Buffer/Buffer.hpp"
#include "config/FpConfig.h"
#include "Utils/Hash/Hash.hpp"

#include "config/FrameHeaderSerializableAc.hpp"
#include "config/FrameFooterSerializableAc.hpp"

namespace Svc {
namespace FrameDetectors {

//! \brief interface class used to codify what must be supported to allow frame detection
class FprimeFrameDetector : public FrameDetector {
  public:
    //! \brief detect if a frame is available within the circular buffer
    //!
    //! Function implemented by sub classes used to determine if a frame is available at the current position of the
    //! circular buffer. Implementors should detect if a frame is available, set size_out, and return a status while
    //! following these expectations:
    //!
    //!  1. FRAME_DETECTED status implies a frame is available at the current offset of the circular buffer.
    //!     size_out must be set to the size of the frame from that location.
    //!
    //!  2. NO_FRAME_DETECTED status implies no frame is possible at the current offset of the circular buffer.
    //!     e.g. no start word is found at the current offset. size_out is ignored.
    //!
    //!  3. MORE_DATA_NEEDED status implies that a frame might be possible but more data is needed before a
    //!     determination is possible. size_out must be set to the total amount of data needed.
    //!
    //!     For example, if a frame start word is 4 bytes, and 3 bytes are available in the circular buffer then the
    //!     return status would be NO_FRAME_DETECTED and size_out must be set to 4 to ensure that at least the start
    //!     word is available.
    //!
    //! \param data: circular buffer with read-only access
    //! \param size_out: set as output to caller indicating size when appropriate
    //! \return status of the detection to be paired with size_out
    Status detect(const Types::CircularBuffer& data, FwSizeType& size_out) const override {
        FprimeProtocol::FrameHeader header;
        FprimeProtocol::FrameFooter footer;

        // decltype(header.m_length) length_value;
        U32 u32_data = 0; // TODO: don't hardcode U32; use the info from the FPP type instead

        // Issues:
        // 1. Can't retrieve the type info directly? Have to make something of size SERIALIZED_SIZE ?
        //        Not a huge deal, but not as efficient ?
        // 2. Can't use the serialization magic since CircularBuffer is not a SierializeBufferBase ???
        for (U32 i = 0; i < data.get_allocated_size(); i++) {
            // Read header
            Fw::SerializeStatus status = data.peek(u32_data, i);
            if (status != Fw::FW_SERIALIZE_OK) {
                return Status::NO_FRAME_DETECTED;
            }
            if (u32_data != header.getstart_word()) {
                continue;
            }
            status = data.peek(u32_data, i + sizeof(U32));
            if (status != Fw::FW_SERIALIZE_OK) {
                size_out = i + sizeof(U32); // can't really use the FPP type ? Or harvest info from nested data types ?
                // We detected the start word, but no length yet. Request more data
                return Status::MORE_DATA_NEEDED;
            }
            U32 packetLength = u32_data;
            Utils::Hash hash;
            Utils::HashBuffer hashBuffer;

            hash.init();
            data.peek(u32_data, packetLength + FprimeProtocol::FrameHeader::SERIALIZED_SIZE);
            U32 transmitted_crc = u32_data;

            // Read footer
            U8 byte;
            for (U32 i = 0; i < packetLength + FprimeProtocol::FrameHeader::SERIALIZED_SIZE; i++) {
                status = data.peek(byte, i);
                FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
                hash.update(&byte, 1);
            }
            hash.final(hashBuffer);
            U32 computed_crc = hashBuffer.asBigEndianU32();
            printf("transmitted_crc: %08X\n", transmitted_crc);
            printf("computed_crc: %08X\n", computed_crc);
            if (transmitted_crc != computed_crc) {
                return Status::MORE_DATA_NEEDED;
            }
            size_out = packetLength + FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameFooter::SERIALIZED_SIZE;
            return Status::FRAME_DETECTED;
        }
        return Status::NO_FRAME_DETECTED;

};  // namespace FrameDetectors
};  // namespace Svc
};
}
#endif  // SVC_FRAME_ACCUMULATOR_FRAME_DETECTOR_FPRIME_FRAME_DETECTOR


//! fprime framing start word is a configurable type and matched against 0xdeadbeef as cast into the appropriate type
// using FprimeStartWord = StartToken<FwFramingTokenType, static_cast<FwFramingTokenType>(0xdeadbeef)>;
// //! fprime framing length is a configurable type
// using FprimeLength = LengthToken<FwFramingTokenType, sizeof(FwFramingTokenType)>;
// //! fprime uses a CRC32 checksum anchored at the end of the data
// using FprimeChecksum = CRC<U32, 2 * sizeof(FwFramingTokenType), 0, CRC32>;

// //! fprime frame detector is a start/length/crc detector using the configured fprime tokens
// using FprimeFrameDetector = StartLengthCrcDetector<FprimeStartWord, FprimeLength, FprimeChecksum>;
