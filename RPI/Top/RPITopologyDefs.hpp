#ifndef RPITopologyDefs_HPP
#define RPITopologyDefs_HPP

#include "Fw/Types/MallocAllocator.hpp"

namespace RPI {

  namespace Allocation {

    // Malloc allocator for topology construction
    extern Fw::MallocAllocator mallocator;

  }

  // State for topology construction
  using TopologyState = int;

  // Health ping entries
  namespace PingEntries {
    namespace rateGroup10HzComp { enum { WARN = 3, FATAL = 5 }; }
    namespace rateGroup1HzComp { enum { WARN = 3, FATAL = 5 }; }
    namespace cmdDisp { enum { WARN = 3, FATAL = 5 }; }
    namespace cmdSeq { enum { WARN = 3, FATAL = 5 }; }
    namespace chanTlm { enum { WARN = 3, FATAL = 5 }; }
    namespace eventLogger { enum { WARN = 3, FATAL = 5 }; }
    namespace prmDb { enum { WARN = 3, FATAL = 5 }; }
    namespace fileDownlink { enum { WARN = 3, FATAL = 5 }; }
    namespace fileUplink { enum { WARN = 3, FATAL = 5 }; }
  }

}

#endif
