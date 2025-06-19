#include "SubtopologyTopologyDefs.hpp"

namespace ComFprime {
    namespace Allocation {
        Fw::MallocAllocator mallocator;
    }

    namespace BufferManagerBins {
        Svc::BufferManager::BufferBins bins;
    }

    namespace Detector {
        Svc::FrameDetectors::FprimeFrameDetector frameDetector;
    }
}
