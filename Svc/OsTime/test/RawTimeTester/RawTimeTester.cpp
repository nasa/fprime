
#include "Os/RawTime.hpp"
#include "Os/Delegate.hpp"
#include "Fw/Time/Time.hpp"
#include "RawTimeTester.hpp"

namespace Os {
    RawTimeInterface *RawTimeInterface::getDelegate(RawTimeHandleStorage& aligned_new_memory, const RawTimeInterface* to_copy) {
        return Os::Delegate::makeDelegate<RawTimeInterface, Svc::RawTimeTester, RawTimeHandleStorage>(
                aligned_new_memory, to_copy
        );
    }
}


namespace Svc {
    Fw::Time RawTimeTester::s_now_time = Fw::Time();
}

