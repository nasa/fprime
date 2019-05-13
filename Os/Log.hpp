#ifndef _Log_hpp_
#define _Log_hpp_

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Log/Log.hpp>
#include <Fw/Obj/ObjBase.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Os {
    class Log : public Fw::Log {
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
        
            static void logMsg(const char* fmt, POINTER_CAST a1, POINTER_CAST a2, POINTER_CAST a3, POINTER_CAST a4, POINTER_CAST a5, POINTER_CAST a6); //!< write a message to the system log
            
    };
}

#endif
