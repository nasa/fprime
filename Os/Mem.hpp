#ifndef _Mem_hpp_
#define _Mem_hpp_

#include <FpConfig.hpp>
#include <Fw/Obj/ObjBase.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Os {
    class Mem {
        public:
            static U32 virtToPhys(U32 virtAddr); //!< Translate virtual to physical memory
            static U32 physToVirt(U32 physAddr); //!< Translate physical to virtual memory
    };
}

#endif
