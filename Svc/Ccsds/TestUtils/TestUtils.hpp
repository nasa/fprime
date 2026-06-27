// ======================================================================
// \title  CcsdsTestUtils.hpp
// \author bocchino
// \brief  Test utilities for CCSDS
// ======================================================================

#ifndef Svc_Ccsds_TestUtils_TestUtils_HPP
#define Svc_Ccsds_TestUtils_TestUtils_HPP

#include "Fw/Types/Optional.hpp"
#include "config/ApidEnumAc.hpp"

namespace Svc {

namespace CcsdsTestUtils {

//! Get a random APID for CCSDS testing
//! If possible, randomly choose from among the set of configured constants for
//! ComCfg::APID that fit in 11 bits.
//! If not possible, return NONE.
//! \return SOME(apid) or NONE
Fw::Optional<ComCfg::Apid::T> getRandomApid();

}  // namespace CcsdsTestUtils

}  // namespace Svc

#endif
