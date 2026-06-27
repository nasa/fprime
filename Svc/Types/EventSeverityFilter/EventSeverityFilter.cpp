#include <Fw/Types/Assert.hpp>
#include <Svc/Types/EventSeverityFilter/EventSeverityFilter.hpp>

namespace Svc {

EventSeverityFilter::EventSeverityFilter() {
    for (FwSizeType i = 0; i < NUM_FILTER_LEVELS; i++) {
        this->m_enabled[i] = true;
    }
}

void EventSeverityFilter::setFilter(Fw::LogSeverity severity, bool enabled) {
    FwSizeType index = 0;
    if (toIndex(severity, index)) {
        this->m_enabled[index] = enabled;
    }
}

bool EventSeverityFilter::isFiltered(Fw::LogSeverity severity) const {
    FwSizeType index = 0;
    if (!toIndex(severity, index)) {
        // FATAL is never filtered
        return false;
    }
    return !this->m_enabled[index];
}

bool EventSeverityFilter::isEnabled(Fw::LogSeverity severity) const {
    FwSizeType index = 0;
    if (!toIndex(severity, index)) {
        // FATAL is always enabled
        return true;
    }
    return this->m_enabled[index];
}

bool EventSeverityFilter::toIndex(Fw::LogSeverity severity, FwSizeType& index) {
    switch (severity.e) {
        case Fw::LogSeverity::WARNING_HI:
            index = 0;
            return true;
        case Fw::LogSeverity::WARNING_LO:
            index = 1;
            return true;
        case Fw::LogSeverity::COMMAND:
            index = 2;
            return true;
        case Fw::LogSeverity::ACTIVITY_HI:
            index = 3;
            return true;
        case Fw::LogSeverity::ACTIVITY_LO:
            index = 4;
            return true;
        case Fw::LogSeverity::DIAGNOSTIC:
            index = 5;
            return true;
        default:
            return false;
    }
}

}  // namespace Svc
