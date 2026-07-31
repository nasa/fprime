// ======================================================================
// \title Os/Stub/DefaultRawTime.cpp
// \brief sets default Os::RawTime to no-op stub implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/RawTimeSource.hpp"
#include "Os/Stub/RawTime.hpp"

namespace Os {

//! \brief get a delegate for RawTimeInterface that intercepts calls for stub RawTime usage
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \param source: timer source selection (unused in stub implementation)
//! \return: pointer to delegate
RawTimeInterface* RawTimeInterface::getDelegate(RawTimeHandleStorage& aligned_placement_new_memory,
                                                const RawTimeInterface* to_copy,
                                                RawTimeSource source) {
    (void)source;  // Stub implementation ignores timer source
    return Os::Delegate::makeDelegate<RawTimeInterface, Os::Stub::RawTime::StubRawTime, RawTimeHandleStorage>(
        aligned_placement_new_memory, to_copy);
}

}  // namespace Os
