// ======================================================================
// \title  SdlsIvSequence.hpp
// \brief  SDLS AES-GCM IV treated as a wrapping big-endian sequence number
// ======================================================================

#ifndef Svc_Ccsds_Utils_SdlsIvSequence_HPP
#define Svc_Ccsds_Utils_SdlsIvSequence_HPP

#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Types/Assert.hpp"
#include "Svc/Ccsds/Types/SdlsIvArrayAc.hpp"

namespace Svc {
namespace Ccsds {
namespace Utils {

//! Sequence-number arithmetic on Svc::Ccsds::SdlsIv, matching the ground segment's IvSeqNum:
//! the 12 bytes are one unsigned big-endian integer, incremented per frame modulo 2^96.
struct SdlsIvSequence final {
    //! Number of bytes in an IV
    static constexpr FwSizeType SIZE = static_cast<FwSizeType>(SdlsIv::SIZE);

    //! Copy SIZE raw frame bytes into an IV
    static void fromBytes(SdlsIv& iv, const U8* bytes) {
        FW_ASSERT(bytes != nullptr);
        for (FwSizeType i = 0; i < SIZE; i++) {
            iv[i] = bytes[i];
        }
    }

    //! Copy an IV out to SIZE raw frame bytes
    static void toBytes(const SdlsIv& iv, U8* bytes) {
        FW_ASSERT(bytes != nullptr);
        for (FwSizeType i = 0; i < SIZE; i++) {
            bytes[i] = iv[i];
        }
    }

    //! Advance the IV by one, wrapping from all-ones to zero
    static void increment(SdlsIv& iv) {
        for (FwSizeType i = SIZE; i > 0; i--) {
            iv[i - 1] = static_cast<U8>(iv[i - 1] + 1);
            if (iv[i - 1] != 0) {
                break;
            }
        }
    }

    //! True when received lies in (last, last + window] modulo 2^96, i.e. it is one of the next
    //! window sequence numbers after last. A reused IV (received == last) is never in the window.
    static bool isInWindow(const SdlsIv& last, const SdlsIv& received, U32 window) {
        // distance = received - last (mod 2^96), computed big-endian with a borrow
        U8 distance[SIZE];
        U16 borrow = 0;
        for (FwSizeType i = SIZE; i > 0; i--) {
            const U16 diff =
                static_cast<U16>(static_cast<U16>(received[i - 1]) - static_cast<U16>(last[i - 1]) - borrow);
            distance[i - 1] = static_cast<U8>(diff & 0xFF);
            borrow = static_cast<U16>((diff >> 8) & 0x01);
        }
        // Anything beyond the low 32 bits is farther away than any U32 window
        for (FwSizeType i = 0; i < SIZE - sizeof(U32); i++) {
            if (distance[i] != 0) {
                return false;
            }
        }
        U32 low = 0;
        for (FwSizeType i = SIZE - sizeof(U32); i < SIZE; i++) {
            low = static_cast<U32>((low << 8) | distance[i]);
        }
        return (low != 0) && (low <= window);
    }
};

}  // namespace Utils
}  // namespace Ccsds
}  // namespace Svc

#endif
