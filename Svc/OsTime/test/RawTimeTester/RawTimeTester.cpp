
#include "RawTimeTester.hpp"
#include "Fw/Time/Time.hpp"
#include "Os/Delegate.hpp"
#include "Os/RawTime.hpp"

namespace Os {
RawTimeInterface* RawTimeInterface::getDelegate(RawTimeHandleStorage& aligned_new_memory,
                                                const RawTimeInterface* to_copy) {
    return Os::Delegate::makeDelegate<RawTimeInterface, Svc::RawTimeTester, RawTimeHandleStorage>(aligned_new_memory,
                                                                                                  to_copy);
}
}  // namespace Os

namespace Svc {
Fw::Time RawTimeTester::s_now_time = Fw::Time();
}
