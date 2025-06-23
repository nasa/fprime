#ifndef DATAPRODUCTSSUBTOPOLOGY_DEFS_HPP
#define DATAPRODUCTSSUBTOPOLOGY_DEFS_HPP 

#include <Fw/Types/MallocAllocator.hpp>
#include <Svc/BufferManager/BufferManager.hpp>
#include <Os/FileSystem.hpp>
#include "Svc/Subtopologies/DataProducts/DataProductsConfig/FppConstantsAc.hpp"

namespace DataProducts {
    namespace Allocation {
        // Malloc allocator for topology construction
        extern Fw::MallocAllocator mallocator;
    }

    namespace BufferManagerBins{
        // Buffer manager bins for Data Products
        extern Svc::BufferManager::BufferBins bins;
    }
    
    // State for topology construction
    struct TopologyState {
    };
}

#endif
