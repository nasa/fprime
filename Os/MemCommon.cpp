#include <Os/Mem.hpp>

#include <string.h>

namespace Os {
    
    U32 Mem::virtToPhys(U32 virtAddr) {
        return virtAddr;
    }
    
    U32 Mem::physToVirt(U32 physAddr) {
        return physAddr;
    }
        
}
