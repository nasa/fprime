#ifndef SVC_CCSDS_UTILS_CRC16_HPP
#define SVC_CCSDS_UTILS_CRC16_HPP

#include "Fw/Types/BasicTypes.hpp"
// Include the lic crc c library:
extern "C" {
#include <Utils/Hash/libcrc/lib_crc.h>
}

namespace Svc {
namespace Ccsds {
namespace Utils {

//! \brief CRC16 CCITT implementation
//!
//! CCSDS uses a CRC16 (CCITT) implementation with polynomial 0x1021, initial value of 0xFFFF, and XOR of 0x0000.
//!
class CRC16 {
  public:
    // Initial value is 0xFFFF
    CRC16() : m_crc(std::numeric_limits<U16>::max()) {}

    //! \brief update CRC with one new byte
    //!
    //! Update function for CRC taking previous value from member variable and updating it.
    //!
    //! \param new_byte: new byte to add to calculation
    void update(U8 new_byte) { this->m_crc = static_cast<U16>(update_crc_ccitt(m_crc, static_cast<char>(new_byte))); };

    //! \brief finalize and return CRC value
    U16 finalize() {
        // Specified XOR value is 0x0000
        return this->m_crc ^ static_cast<U16>(0);
    };

    //! \brief compute CRC16 for a buffer
    //!
    //! Compute the CRC16 for a given buffer and length.
    //!
    //! \param buffer: pointer to the data buffer
    //! \param length: length of the data buffer
    //! \return computed CRC16 value
    static U16 compute(const U8* buffer, U32 length) {
        U16 crc = std::numeric_limits<U16>::max();  // Initial value
        for (U32 i = 0; i < length; ++i) {
            crc = static_cast<U16>(update_crc_ccitt(crc, static_cast<char>(buffer[i])));
        }
        return crc ^ static_cast<U16>(0);  // Finalize with XOR value
    }

    U16 m_crc;
};

}  // namespace Utils
}  // namespace Ccsds
}  // namespace Svc

#endif  // SVC_CCSDS_UTILS_CRC16_HPP
