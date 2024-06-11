/**
 * File: Logger.cpp
 * Description: Framework logging implementation
 * Author: mstarch
 *
 * This file adds in support to the core 'Fw' package, to separate it from Os and other loggers, and
 * allow the architect of the system to select which core framework logging should be used. 
 */

#include <Fw/Logger/Logger.hpp>

namespace Fw {

//Initial logger is NULL
    Logger* Logger::s_current_logger = nullptr;

// Basic log implementation
    void Logger::logMsg(const char* fmt, POINTER_CAST a0, POINTER_CAST a1,
            POINTER_CAST a2, POINTER_CAST a3, POINTER_CAST a4, POINTER_CAST a5,
            POINTER_CAST a6, POINTER_CAST a7, POINTER_CAST a8, POINTER_CAST a9) {
        // Log if capable, otherwise drop
        if (Logger::s_current_logger != nullptr) {
            Logger::s_current_logger->log(fmt, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
        }
    }
// Register the logger
    void Logger::registerLogger(Logger* logger) {
        Logger::s_current_logger = logger;
    }

    Logger::~Logger() {
    }

} //End namespace Fw
