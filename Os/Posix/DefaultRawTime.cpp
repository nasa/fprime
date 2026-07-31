// ======================================================================
// \title Os/Posix/DefaultRawTime.cpp
// \brief sets default Os::RawTime Posix implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Posix/RawTime.hpp"
#include "Os/RawTimeSource.hpp"

namespace Os {

//! \brief get a delegate for RawTimeInterface that intercepts calls for Posix
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \param source: timer source selection (unused in Posix - only RAWTIME_DEFAULT supported)
//! \return: pointer to delegate
RawTimeInterface* RawTimeInterface::getDelegate(RawTimeHandleStorage& aligned_new_memory,
                                                const RawTimeInterface* to_copy,
                                                RawTimeSource source) {
    (void)source;  // Posix implementation only supports default timer source
    return Os::Delegate::makeDelegate<RawTimeInterface, Os::Posix::RawTime::PosixRawTime, RawTimeHandleStorage>(
        aligned_new_memory, to_copy);
}

}  // namespace Os
