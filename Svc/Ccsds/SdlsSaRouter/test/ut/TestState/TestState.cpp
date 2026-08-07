// ======================================================================
// \title  TestState.cpp
// \author lestarch-autobot
// \brief  Shadow state model for SdlsSaRouter rule-based testing
// ======================================================================

#include "Svc/Ccsds/SdlsSaRouter/test/ut/TestState/TestState.hpp"

#include "Fw/Types/Assert.hpp"
#include "STest/Pick/Pick.hpp"

namespace Svc {

namespace Ccsds {

const U8* SdlsSaRouterTestState ::shadow_getRandomOutstanding() const {
    FW_ASSERT(!this->shadow_outstanding.empty());
    U32 pick = STest::Pick::lowerUpper(0, static_cast<U32>(this->shadow_outstanding.size() - 1));
    auto it = this->shadow_outstanding.begin();
    std::advance(it, pick);
    return it->first;
}

}  // namespace Ccsds

}  // namespace Svc
