// ======================================================================
// \title  format.cpp
// \author mstarch
// \brief  cpp file for c-string format function as a implementation using snprintf
// ======================================================================
#include <Fw/Types/StringUtils.hpp>
#include <Fw/Types/scan.hpp>
#include <cstdio>
#include <limits>

Fw::ScanStatus Fw::stringScan(FwSizeType& count,
                              const char* source,
                              FwSizeType maximumSize,
                              const char* formatString,
                              ...) {
    va_list args;
    va_start(args, formatString);
    Fw::ScanStatus status = Fw::stringScan(count, source, maximumSize, formatString, args);
    va_end(args);
    return status;
}

Fw::ScanStatus Fw::stringScan(FwSizeType& count,
                              const char* source,
                              FwSizeType maximumSize,
                              const char* formatString,
                              va_list args) {
    Fw::ScanStatus scanStatus = Fw::ScanStatus::SUCCESS;
    count = 0;
    // Check format string
    if (formatString == nullptr) {
        scanStatus = Fw::ScanStatus::INVALID_FORMAT_STRING;
    }
    // Must allow the compiler to choose the correct type for comparison
    else if (maximumSize > std::numeric_limits<size_t>::max()) {
        scanStatus = Fw::ScanStatus::SIZE_OVERFLOW;
    }
    // Check for null-termination of the source string bounded by maximumSize
    else if (StringUtils::string_length(source, maximumSize) >= maximumSize) {
        scanStatus = Fw::ScanStatus::UNTERMINATED_SOURCE_STRING;
    } else {
        const int scannedFields = vsscanf(source, formatString, args);
        if (scannedFields < 0) {
            scanStatus = Fw::ScanStatus::OTHER_ERROR;
        } else {
            count = static_cast<FwSizeType>(scannedFields);
        }
    }
    return scanStatus;
}
