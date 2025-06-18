#ifndef COMFPRIMESUBTOPOLOGY_DEFS_HPP
#define COMFPRIMESUBTOPOLOGY_DEFS_HPP

#include <Svc/FrameAccumulator/FrameDetector/FprimeFrameDetector.hpp>
#include <Fw/Types/MallocAllocator.hpp>
#include <Svc/BufferManager/BufferManager.hpp>

namespace ComFprime {
    namespace Allocation {
        // Malloc allocator for topology construction
        extern Fw::MallocAllocator mallocator;
    }

    namespace BufferManagerBins {
        // Buffer manager bins for ComFprime
        extern Svc::BufferManager::BufferBins bins;
    }

    namespace Detector {
        // Frame detector for ComFprime
        extern Svc::FrameDetectors::FprimeFrameDetector frameDetector;
    }

    // State for topology construction
    struct TopologyState {
      const char* hostname;
      U16 port;
    };

}

#endif