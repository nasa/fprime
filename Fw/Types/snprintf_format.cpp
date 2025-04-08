// ======================================================================
// \title  format.cpp
// \author mstarch
// \brief  cpp file for c-string format function as a implementation using snprintf
// ======================================================================
#include <Fw/Types/format.hpp>
#include <limits>
#include <cstdio>

Fw::FormatStatus Fw::stringFormat(char* destination, const FwSizeType maximumSize, const char* formatString, ...) {
    va_list args;
    va_start(args, formatString);
    FormatStatus status = Fw::stringFormat(destination, maximumSize, formatString, args);
    va_end(args);
    return status;
}

Fw::FormatStatus Fw::stringFormat(char* destination, const FwSizeType maximumSize, const char* formatString, va_list args) {
    Fw::FormatStatus formatStatus = Fw::FormatStatus::SUCCESS;
    // Force null termination in error cases
    destination[0] = 0;
    // Check format string
    if (formatString == nullptr) {
        formatStatus = Fw::FormatStatus::INVALID_FORMAT_STRING;
    }
    // Must allow the compiler to choose the correct type for comparison
    else if (maximumSize > std::numeric_limits<size_t>::max()) {
        formatStatus = Fw::FormatStatus::SIZE_OVERFLOW;
    } else {
        PlatformIntType needed_size = vsnprintf(destination, static_cast<size_t>(maximumSize), formatString, args);
        destination[maximumSize - 1] = 0; // Force null-termination
        if (needed_size < 0) {
            formatStatus = Fw::FormatStatus::OTHER_ERROR;
        } else if (static_cast<FwSizeType>(needed_size) >= maximumSize) {
            formatStatus = Fw::FormatStatus::OVERFLOWED;
        }
    }
    return formatStatus;
}
