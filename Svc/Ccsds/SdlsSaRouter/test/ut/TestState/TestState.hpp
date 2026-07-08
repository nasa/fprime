// ======================================================================
// \title  TestState.hpp
// \author lestarch-autobot
// \brief  Shadow state model for SdlsSaRouter rule-based testing
//
// The shadow mirrors the component's table of outstanding decrypted data
// buffers (data pointer -> originating downstream port index).  Rule
// preconditions read from it; rule actions update it in lockstep with the
// component.
// ======================================================================

#ifndef Svc_Ccsds_SdlsSaRouter_TestState_HPP
#define Svc_Ccsds_SdlsSaRouter_TestState_HPP

#include <map>

#include "Fw/FPrimeBasicTypes.hpp"

namespace Svc {

namespace Ccsds {

class SdlsSaRouterTestState {
  public:
    // ----------------------------------------------------------------------
    // Component "shadow" test state
    // ----------------------------------------------------------------------

    //! Outstanding decrypted data buffers to their originating port index.
    //! Mirrors the component's m_outstanding map.
    std::map<const U8*, FwIndexType> shadow_outstanding;

  public:
    // ----------------------------------------------------------------------
    // Helpers querying the shadow state
    // ----------------------------------------------------------------------

    //! Return a uniformly random outstanding buffer pointer
    const U8* shadow_getRandomOutstanding() const;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
