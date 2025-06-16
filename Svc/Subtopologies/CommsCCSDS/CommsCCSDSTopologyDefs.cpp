#include "SubtopologyTopologyDefs.hpp"

namespace Comms {
    namespace Allocation {
        Fw::MallocAllocator mallocator;
    }

    namespace BufferManagerBins {
        Svc::BufferManager::BufferBins bins;
    }

    namespace Detector {
        Svc::FrameDetectors::CcsdsTcFrameDetector frameDetector;
    }
}