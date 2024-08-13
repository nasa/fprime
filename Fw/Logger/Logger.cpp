/**
 * File: Logger.cpp
 * Description: Framework logging implementation
 * Author: mstarch
 *
 * This file adds in support to the core 'Fw' package, to separate it from Os and other loggers, and
 * allow the architect of the system to select which core framework logging should be used.
 */
#include <Fw/Logger/Logger.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/String.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <cstdarg>
#include <limits>

namespace Fw {

// Initial logger is NULL
Logger* Logger::s_current_logger = nullptr;

void Logger::log(const char* format, ...) {
    Fw::String formatted_string;
    // Forward the variable arguments to the vformat format implementation
    va_list args;
    va_start(args, format);
    formatted_string.vformat(format, args);
    va_end(args);
    Logger::log(formatted_string);
}

void Logger::log(const StringBase& string) {
    if (Logger::s_current_logger != nullptr) {
        Logger::s_current_logger->write(string.toChar(), string.length());
    }
}

void Logger::logMsg(const char* fmt,
                    POINTER_CAST a0,
                    POINTER_CAST a1,
                    POINTER_CAST a2,
                    POINTER_CAST a3,
                    POINTER_CAST a4,
                    POINTER_CAST a5,
                    POINTER_CAST a6,
                    POINTER_CAST a7,
                    POINTER_CAST a8,
                    POINTER_CAST a9) {
    // Ensure this method can handle all integers
    static_assert((sizeof(POINTER_CAST) >= sizeof(void*)) and
                      (sizeof(POINTER_CAST) >= sizeof(U16) or (not FW_HAS_16_BIT)) and
                      (sizeof(POINTER_CAST) >= sizeof(U32) or (not FW_HAS_32_BIT)) and
                      (sizeof(POINTER_CAST) >= sizeof(U64) or (not FW_HAS_64_BIT)),
                  "POINTER_CAST type is not large enough to support logMsg");

    // Check that there are no use of floats
    FW_ASSERT(
        Fw::StringUtils::substring_find(
            fmt, Fw::StringUtils::string_length(fmt, std::numeric_limits<FwSizeType>::max()), "%f", sizeof "%f") == -1);
    FW_ASSERT(
        Fw::StringUtils::substring_find(
            fmt, Fw::StringUtils::string_length(fmt, std::numeric_limits<FwSizeType>::max()), "%g", sizeof "%g") == -1);
    Logger::log(fmt, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
}

void Logger::registerLogger(Logger* logger) {
    Logger::s_current_logger = logger;
}

}  // End namespace Fw
