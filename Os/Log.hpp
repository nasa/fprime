/**
 * File: Os/Log.hpp
 * Description: this file provides an implementation of the Fw::Logger class that is backed by the
 * Os abstraction layer.
 */
#ifndef _Log_hpp_
#define _Log_hpp_

#include <FpConfig.hpp>
#include <Fw/Logger/Logger.hpp>

namespace Os {
    class Log : public Fw::Logger {
        public:
            /**
             * Constructor for the Os::Log object.
             */
            Log();

            /**
             * Function called on the logger to log a message. This is abstract virtual method and
             * must be supplied by the subclass. This logger object should be registered with the
             * Fw::Log::registerLogger function.
             * \param fmt: format string in which to place arguments
             * \param a0: zeroth argument. (Default: 0)
             * \param a1: first argument. (Default: 0)
             * \param a2: second argument. (Default: 0)
             * \param a3: third argument. (Default: 0)
             * \param a4: fourth argument. (Default: 0)
             * \param a5: fifth argument. (Default: 0)
             * \param a6: sixth argument. (Default: 0)
             * \param a7: seventh argument. (Default: 0)
             * \param a8: eighth argument. (Default: 0)
             * \param a9: ninth argument. (Default: 0)
             */
            void log(
                const char* fmt,
                POINTER_CAST a0 = 0,
                POINTER_CAST a1 = 0,
                POINTER_CAST a2 = 0,
                POINTER_CAST a3 = 0,
                POINTER_CAST a4 = 0,
                POINTER_CAST a5 = 0,
                POINTER_CAST a6 = 0,
                POINTER_CAST a7 = 0,
                POINTER_CAST a8 = 0,
                POINTER_CAST a9 = 0
            );
    };
}

#endif
