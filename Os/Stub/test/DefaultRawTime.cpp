// ======================================================================
// \title Os/Stub/test/DefaultRawTime.cpp
// \brief sets default Os::RawTime to test stub implementation via linker
// ======================================================================
#include "Os/Stub/test/RawTime.hpp"
#include "Os/Delegate.hpp"

namespace Os {

//! \brief get a delegate for RawTimeInterface that intercepts calls for  for stub test file usage
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
RawTimeInterface *RawTimeInterface::getDelegate(RawTimeHandleStorage& aligned_placement_new_memory, const RawTimeInterface* to_copy) {
    return Os::Delegate::makeDelegate<RawTimeInterface, Os::Stub::RawTime::Test::TestRawTime, RawTimeHandleStorage>(
            aligned_placement_new_memory, to_copy
    );
}

}  // namespace Os
