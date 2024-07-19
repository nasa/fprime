// ======================================================================
// \title  CCSDSFrameDetector.hpp
// \author mstarch
// \brief  hpp file for CCSDS frame detector definitions
// ======================================================================
#ifndef SVC_FRAME_ACCUMULATOR_FRAME_DETECTOR_CCSDS_FRAME_DETECTOR
#define SVC_FRAME_ACCUMULATOR_FRAME_DETECTOR_CCSDS_FRAME_DETECTOR
#include "FpConfig.h"
#include "Svc/FrameAccumulator/FrameDetector/StartLengthCrcDetector.hpp"
namespace Svc {
namespace FrameDetectors {

//! \brief CRC16 CCITT implementation
//!
//! CCSDS uses a CRC16 (CCITT) implementation with polynomial 0x11021, initial value of 0xFFFF, and XOR of 0x0000.
//!
class CRC16_CCITT : public CRCWrapper<U16> {
  public:
    // Initial value is 0xFFFF
    CRC16_CCITT() : CRCWrapper<U16>(std::numeric_limits<U16>::max()) {}

    //! \brief update CRC with one new byte
    //!
    //! Update function for CRC taking previous value from member variable and updating it.
    //!
    //! \param new_byte: new byte to add to calculation
    void update(U8 new_byte) override {
        this->m_crc =  static_cast<U16>(update_crc_ccitt(m_crc, new_byte));
    };

    //! \brief finalize and return CRC value
    U16 finalize() override {
        // Specified XOR value is 0x0000
        return this->m_crc ^ static_cast<U16>(0);
    };
};


//! CCSDS framing start word is 2 bits of version number "00" at the head of the first 2 octets
using CCSDSStartWord = StartToken<U16, static_cast<U16>(0), static_cast<U8>(0xC000)>;
//! CCSDS length is the last 10 bits of the 3rd and 4th octet
using CCSDSLength = LengthToken<U16, sizeof(U16), static_cast<U16>(0x03FF)>;
//! CCSDS checksum is a 16bit CRC with data starting at the 6th octet and the crc following directly
using CCSDSChecksum = CRC<U16, 5, 0, CRC16_CCITT>;

//! CCSDS frame detector is a start/length/crc detector using the configured fprime tokens
using CCSDSFrameDetector = StartLengthCrcDetector<CCSDSStartWord, CCSDSLength, CCSDSChecksum>;
}  // namespace FrameDetectors
}  // namespace Svc
#endif  // SVC_FRAME_ACCUMULATOR_FRAME_DETECTOR_CCSDS_FRAME_DETECTOR
