// ======================================================================
// \title Os/Posix/DefaultRawTime.cpp
// \brief sets default Os::RawTime Posix implementation via linker
// ======================================================================
#include "Os/Posix/RawTime.hpp"
#include "Os/Delegate.hpp"

namespace Os {

//! \brief get a delegate for RawTimeInterface that intercepts calls for Posix
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
RawTimeInterface *RawTimeInterface::getDelegate(RawTimeHandleStorage& aligned_new_memory, const RawTimeInterface* to_copy) {
    return Os::Delegate::makeDelegate<RawTimeInterface, Os::Posix::RawTime::PosixRawTime, RawTimeHandleStorage>(
            aligned_new_memory, to_copy
    );
}

}  // namespace Os
