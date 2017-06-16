#ifndef _Log_hpp_
#define _Log_hpp_

#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Obj/ObjBase.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Os {
    class Log {
        public:
        
            static void logMsg(const char* fmt, POINTER_CAST a1, POINTER_CAST a2, POINTER_CAST a3, POINTER_CAST a4, POINTER_CAST a5, POINTER_CAST a6); //!< write a message to the system log
            
    };
}

#endif
