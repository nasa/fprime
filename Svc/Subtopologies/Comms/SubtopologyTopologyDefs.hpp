#ifndef COMMSSUBTOPOLOGY_DEFS_HPP
#define COMMSSUBTOPOLOGY_DEFS_HPP

#include <Svc/FrameAccumulator/FrameDetector/CcsdsTcFrameDetector.hpp>
#include <Fw/Types/MallocAllocator.hpp>
#include <Svc/BufferManager/BufferManager.hpp>

namespace Comms {
    namespace Allocation {
        // Malloc allocator for topology construction
        extern Fw::MallocAllocator mallocator;
    }

    namespace BufferManagerBins {
        // Buffer manager bins for Comms
        extern Svc::BufferManager::BufferBins bins;
    }

    namespace Detector {
        // The subtopology uses the CCSDS packet protocol when communicating with the ground and therefore uses the CCSDS
        // framing and deframing implementations.
        Svc::FrameDetectors::CcsdsTcFrameDetector frameDetector;
    }

    // State for topology construction
    struct TopologyState {
      const char* hostname;
      U16 port;
    };

}

#endif