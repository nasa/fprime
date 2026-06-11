// ======================================================================
// \title  CcsdsTestUtils.hpp
// \author bocchino
// \brief  Test utilities for CCSDS
// ======================================================================

#include "TestUtils/Option.hpp"
#include "config/ApidEnumAc.hpp"

namespace Svc {

namespace CcsdsTestUtils {

//! Get a random APID for CCSDS testing
//! If possible, randomly choose from among the set of configured constants for
//! ComCfg::APID that fit in 11 bits.
//! If not possible, return NONE.
//! \return SOME(apid) or NONE
TestUtils::Option<ComCfg::Apid::SerialType> getRandomApid();

}  // namespace CcsdsTestUtils

}  // namespace Svc
