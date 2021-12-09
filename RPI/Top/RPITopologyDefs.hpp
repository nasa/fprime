#ifndef RPITopologyDefs_HPP
#define RPITopologyDefs_HPP

#include "Fw/Types/MallocAllocator.hpp"
#include "RPI/Top/FppConstantsAc.hpp"
#include "Svc/LinuxTimer/LinuxTimer.hpp"

namespace RPI {

  // Declare the Linux timer here so it is visible in main
  extern Svc::LinuxTimer linuxTimer;

  // A list of contexts for the rate groups
  namespace RGContext {

    enum {
      CONTEXT_1Hz = 10, // 1 Hz cycle
      CONTEXT_10Hz = 11 // 10 Hz cycle
    };

  }

  namespace Allocation {

    // Malloc allocator for topology construction
    extern Fw::MallocAllocator mallocator;

  }

  // State for topology construction
  struct TopologyState {
    TopologyState() :
      hostName(""),
      portNumber(0)
    {

    }
    TopologyState(
        const char *hostName,
        U32 portNumber
    ) :
      hostName(hostName),
      portNumber(portNumber)
    {

    }
    const char* hostName;
    U32 portNumber;
  };

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
