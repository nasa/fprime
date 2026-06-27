#include <Fw/Types/Assert.hpp>
#include <Svc/Types/EventSeverityFilter/EventSeverityFilter.hpp>

namespace Svc {

EventSeverityFilter::EventSeverityFilter() {
    for (FwSizeType i = 0; i < NUM_FILTER_LEVELS; i++) {
        this->m_enabled[i] = Fw::Enabled::ENABLED;
    }
}

void EventSeverityFilter::setFilter(Fw::LogSeverity severity, Fw::Enabled enabled) {
    FwSizeType index = 0;
    if (toIndex(severity, index) == Fw::Success::SUCCESS) {
        this->m_enabled[index] = enabled;
    }
}

bool EventSeverityFilter::isFiltered(Fw::LogSeverity severity) const {
    FwSizeType index = 0;
    if (toIndex(severity, index) != Fw::Success::SUCCESS) {
        // FATAL / unknown are never filtered
        return false;
    }
    return this->m_enabled[index] == Fw::Enabled::DISABLED;
}

Fw::Enabled EventSeverityFilter::isEnabled(Fw::LogSeverity severity) const {
    FwSizeType index = 0;
    if (toIndex(severity, index) != Fw::Success::SUCCESS) {
        // FATAL is always enabled
        return Fw::Enabled::ENABLED;
    }
    return this->m_enabled[index];
}

const Fw::LogSeverity::t EventSeverityFilter::SEVERITY_ORDER[NUM_FILTER_LEVELS] = {
    Fw::LogSeverity::WARNING_HI,  Fw::LogSeverity::WARNING_LO,  Fw::LogSeverity::COMMAND,
    Fw::LogSeverity::ACTIVITY_HI, Fw::LogSeverity::ACTIVITY_LO, Fw::LogSeverity::DIAGNOSTIC,
};

Fw::Success EventSeverityFilter::fromIndex(FwSizeType index, Fw::LogSeverity& severity) {
    if (index >= NUM_FILTER_LEVELS) {
        return Fw::Success::FAILURE;
    }
    severity = SEVERITY_ORDER[index];
    return Fw::Success::SUCCESS;
}

Fw::Success EventSeverityFilter::toIndex(Fw::LogSeverity severity, FwSizeType& index) {
    switch (severity.e) {
        case Fw::LogSeverity::WARNING_HI:
            index = 0;
            return Fw::Success::SUCCESS;
        case Fw::LogSeverity::WARNING_LO:
            index = 1;
            return Fw::Success::SUCCESS;
        case Fw::LogSeverity::COMMAND:
            index = 2;
            return Fw::Success::SUCCESS;
        case Fw::LogSeverity::ACTIVITY_HI:
            index = 3;
            return Fw::Success::SUCCESS;
        case Fw::LogSeverity::ACTIVITY_LO:
            index = 4;
            return Fw::Success::SUCCESS;
        case Fw::LogSeverity::DIAGNOSTIC:
            index = 5;
            return Fw::Success::SUCCESS;
        default:
            return Fw::Success::FAILURE;
    }
}

}  // namespace Svc
