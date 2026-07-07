// ======================================================================
// \title  CcsdsTestUtils.cpp
// \author bocchino
// \brief  Test utilities for CCSDS
// ======================================================================

#include "Svc/Ccsds/TestUtils/TestUtils.hpp"
#include "STest/Random/Random.hpp"

namespace Svc {

namespace CcsdsTestUtils {

using SerialType = ComCfg::Apid::SerialType;
using ApidOption = Fw::Optional<ComCfg::Apid::T>;

ApidOption getRandomApid() {
    const SerialType selectedIdx = static_cast<SerialType>(STest::Random::startLength(0, ComCfg::Apid::NUM_CONSTANTS));
    // Choose from within the bounds provided by CCSDS and the SerialType.
    // 1. We have to respect the CCSDS bound because we are testing CCSDS code
    // 2. We have to respect the SerialType bound because F Prime may not be configured
    // to use CCSDS, but we still want the test to be valid, if possible.
    constexpr auto ccsdsBound = static_cast<SerialType>((1 << 11) - 1);  // 11 bits
    constexpr auto serialTypeBound = std::numeric_limits<SerialType>::max();
    constexpr auto bound = std::min(ccsdsBound, serialTypeBound);
    // Search through the interval [0, maxApid] until we find a valid APID at the
    // selected index, or we run out of numbers
    SerialType idx = 0;
    SerialType apid = 0;
    for (SerialType candidateApid = 0; candidateApid <= bound; candidateApid++) {
        if (ComCfg::Apid::isValid(candidateApid)) {
            // Found a valid APID: store it
            apid = candidateApid;
            if (idx == selectedIdx) {
                // We are at the selected index: done
                break;
            }
            // Not yet at the selected index: keep going
            // We'll either go onto the next valid APID or use the current one
            // if we run off the end of the 11-bit range
            idx++;
        }
    }
    // If the APID we found is not valid, then return NONE
    // This can happen if all of the configured APIDs are out of the 11-bit range
    // required by CCSDS
    const auto result = ComCfg::Apid::isValid(apid) ? ApidOption(static_cast<ComCfg::Apid::T>(apid)) : Fw::NONE;
    return result;
}

}  // namespace CcsdsTestUtils

}  // namespace Svc
