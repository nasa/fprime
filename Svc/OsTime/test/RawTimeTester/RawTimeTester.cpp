
#include "RawTimeTester.hpp"
#include "Fw/Time/Time.hpp"
#include "Os/Delegate.hpp"
#include "Os/RawTime.hpp"
#include "Os/RawTimeSource.hpp"

namespace Os {
RawTimeInterface* RawTimeInterface::getDelegate(RawTimeHandleStorage& aligned_new_memory,
                                                const RawTimeInterface* to_copy,
                                                RawTimeSource source) {
    (void)source;  // Test implementation ignores timer source
    return Os::Delegate::makeDelegate<RawTimeInterface, Svc::RawTimeTester, RawTimeHandleStorage>(aligned_new_memory,
                                                                                                  to_copy);
}
}  // namespace Os

namespace Svc {
Fw::Time RawTimeTester::s_now_time = Fw::Time();
}
