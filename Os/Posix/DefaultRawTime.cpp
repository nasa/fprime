// ======================================================================
// \title Os/Posix/DefaultRawTime.cpp
// \brief sets default Os::RawTime Posix implementation via linker
// ======================================================================
#include "Os/Posix/RawTime.hpp"
#include "Os/Delegate.hpp"
#include "Svc/Cycle/TimerVal.hpp"
namespace Os {

//! \brief get a delegate for RawTimeInterface that intercepts calls for Posix
//! \param aligned_new_memory: aligned memory to fill
//! \param to_copy: pointer to copy-constructor input
//! \return: pointer to delegate
RawTimeInterface *RawTimeInterface::getDelegate(HandleStorage& aligned_new_memory, const RawTimeInterface* to_copy) {
    return Os::Delegate::makeDelegate<RawTimeInterface, Os::Posix::RawTime::PosixRawTime>(
            aligned_new_memory, to_copy
    );
}

const FwSizeType RawTimeInterface::SERIALIZED_SIZE = sizeof(U32) * 2; // TODO: document why
}  // namespace Os

// const FwSizeType Svc::TimerVal::SERIALIZED_SIZE = sizeof(U32) * 2; // TODO: document why
