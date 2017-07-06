#include <Os/WatchdogTimer.hpp>

namespace Os {
    
    WatchdogTimer::WatchdogTimer()
    : m_handle(0),m_cb(0),m_parameter(0),m_timerTicks(0),m_timerMs(0)
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

    WatchdogTimer::WatchdogStatus WatchdogTimer::restart(void) {
        return WATCHDOG_START_ERROR;
    }

    WatchdogTimer::WatchdogStatus WatchdogTimer::cancel(void) {
        return WATCHDOG_CANCEL_ERROR;
    }
    
}


