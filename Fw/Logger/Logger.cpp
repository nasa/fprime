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
        Logger::s_current_logger->writeMessage(string);
    }
}

void Logger::registerLogger(Logger* logger) {
    Logger::s_current_logger = logger;
}

}  // End namespace Fw
