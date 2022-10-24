#include <Os/WatchdogTimer.hpp>
#include <Fw/Types/Assert.hpp>

namespace Os {

    WatchdogTimer::WatchdogTimer()
    : m_handle(0),m_cb(nullptr),m_parameter(nullptr),m_timerTicks(0),m_timerMs(0)
    {
    }

    WatchdogTimer::~WatchdogTimer() {
    }

    WatchdogTimer::WatchdogStatus WatchdogTimer::startTicks( I32 delayInTicks, WatchdogCb p_callback, void* parameter) {
        return WATCHDOG_START_ERROR;
    }

    WatchdogTimer::WatchdogStatus WatchdogTimer::startMs( I32 delayInMs, WatchdogCb p_callback, void* parameter) {
        return WATCHDOG_START_ERROR;
    }

    WatchdogTimer::WatchdogStatus WatchdogTimer::restart() {
        return WATCHDOG_START_ERROR;
    }

    WatchdogTimer::WatchdogStatus WatchdogTimer::cancel() {
        return WATCHDOG_CANCEL_ERROR;
    }

    void WatchdogTimer::expire() {
        FW_ASSERT(m_cb != nullptr);
        m_cb(m_parameter);
    }

}


