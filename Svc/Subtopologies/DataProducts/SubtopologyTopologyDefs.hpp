#ifndef DATAPRODUCTSSUBTOPOLOGY_DEFS_HPP
#define DATAPRODUCTSSUBTOPOLOGY_DEFS_HPP 

#include <Fw/Types/MallocAllocator.hpp>
#include <Svc/BufferManager/BufferManager.hpp>
#include <Os/FileSystem.hpp>
#include "Svc/Subtopologies/DataProducts/DataProductsConfig/FppConstantsAc.hpp"
#include "DataProductsConfig/DataProductsSubtopologyConfig.hpp"

namespace DataProducts {
    // State for topology construction
    struct SubtopologyState {
        // Empty - no external state needed for DataProducts subtopology
    };

    struct TopologyState {
        SubtopologyState dataProducts;
    };
}

#endif
