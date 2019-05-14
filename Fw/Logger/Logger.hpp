/**
 * File: Loger.hpp
 * Description: Framework logging support
 * Author: mstarch
 *
 * This file adds in support to the core 'Fw' package, to separate it from Os and other loggers, and
 * allow the architect of the system to select which core framework logging should be used. 
 */
#ifndef _Fw_Loger_hpp_
#define _Fw_Loger_hpp_

#include <Fw/Types/BasicTypes.hpp>

namespace Fw {
    class Logger {
        public:
            /**
             * Function called on the logger to log a message. This is abstract virtual method and
             * must be supplied by the subclass. This logger object should be registered with the
             * Fw::Log::registerLogger function.
             * \param const char* fmt: format string in which to place arguments
             * \param POINTER_CAST a1: first argument. (Default: 0)
             * \param POINTER_CAST a2: second argument. (Default: 0)
             * \param POINTER_CAST a3: third argument. (Default: 0)
             * \param POINTER_CAST a4: forth argument. (Default: 0)
             * \param POINTER_CAST a5: fifth argument. (Default: 0)
             * \param POINTER_CAST a6: sixth argument. (Default: 0)
             */
            virtual void log(
                const char* fmt,
                POINTER_CAST a1 = 0,
                POINTER_CAST a2 = 0,
                POINTER_CAST a3 = 0,
                POINTER_CAST a4 = 0,
                POINTER_CAST a5 = 0,
                POINTER_CAST a6 = 0
            ) = 0;

            /**
             * Logs a message using the currently specified static logger. If a logger is not
             * registered, then the log message is dropped.
             * \param const char* fmt: format string in which to place arguments
             * \param POINTER_CAST a1: first argument. (Default: 0)
             * \param POINTER_CAST a2: second argument. (Default: 0)
             * \param POINTER_CAST a3: third argument. (Default: 0)
             * \param POINTER_CAST a4: forth argument. (Default: 0)
             * \param POINTER_CAST a5: fifth argument. (Default: 0)
             * \param POINTER_CAST a6: sixth argument. (Default: 0)
             */
            static void logMsg(
                const char* fmt,
                POINTER_CAST a1 = 0,
                POINTER_CAST a2 = 0,
                POINTER_CAST a3 = 0,
                POINTER_CAST a4 = 0,
                POINTER_CAST a5 = 0,
                POINTER_CAST a6 = 0
            );

            /**
             * Registers the static logger for use with the Fw::Log::logMsg function. This must be
             * a subclass of Fw::Log.
             * Log* logger: logger to log to when Fw::Log::logMsg is called.
             */
            static void registerLogger(Logger* logger);

            //!< Static logger to use when calling the above 'logMsg' function
            static Logger* s_current_logger;
    };
}

#endif
