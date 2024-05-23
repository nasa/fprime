/**
 * File: Logger.cpp
 * Description: Framework logging implementation
 * Author: mstarch
 *
 * This file adds in support to the core 'Fw' package, to separate it from Os and other loggers, and
 * allow the architect of the system to select which core framework logging should be used. 
 */

#include <Fw/Logger/Logger.hpp>
#include <Fw/Types/String.hpp>

namespace Fw {

//Initial logger is NULL
Logger* Logger::s_current_logger = nullptr;


void Logger::log(const char *format, ...) {
    Fw::String formatted_string;
    // Forward the variable arguments to the vformat format implementation
    va_list args;
    va_start(args, format);
    formatted_string.vformat(format, args);
    va_end(args);
    this->log(formatted_string);
}

void Logger::log(const StringBase& string) {
    this->write(string.toChar(), string.length());
}

void Logger::logMsg(const char* fmt, POINTER_CAST a0, POINTER_CAST a1,
        POINTER_CAST a2, POINTER_CAST a3, POINTER_CAST a4, POINTER_CAST a5,
        POINTER_CAST a6, POINTER_CAST a7, POINTER_CAST a8, POINTER_CAST a9) {
    // Log if capable, otherwise drop

}

void Logger::registerLogger(Logger* logger) {
    Logger::s_current_logger = logger;
}

Logger::~Logger() {
}

} //End namespace Fw
