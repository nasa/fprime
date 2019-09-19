/**
 * File: Os/Log.hpp
 * Description: this file provides an implementation of the Fw::Logger class that is backed by the
 * Os abstraction layer.
 */
#ifndef _Log_hpp_
#define _Log_hpp_

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Logger/Logger.hpp>

namespace Os {
    class Log : public Fw::Logger {
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
            void log(
                const char* fmt,
                POINTER_CAST a1 = 0,
                POINTER_CAST a2 = 0,
                POINTER_CAST a3 = 0,
                POINTER_CAST a4 = 0,
                POINTER_CAST a5 = 0,
                POINTER_CAST a6 = 0
            );
    };
}

#endif
