#ifndef RefTopologyDefs_HPP
#define RefTopologyDefs_HPP

#include <Fw/Types/Assert.hpp>
#include <Os/Task.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Os/Log.hpp>
#include <Fw/Types/MallocAllocator.hpp>

#include <Svc/FramingProtocol/FprimeProtocol.hpp>

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>
#endif

namespace Ref {

  namespace Allocation {

    extern Fw::MallocAllocator mallocator;
    
  }

  // TODO: Redefine TopologyState
  typedef int TopologyState;

  namespace PingEntries {
    namespace blockDrv { enum { WARN = 3, FATAL = 5 }; }
    namespace chanTlm { enum { WARN = 3, FATAL = 5 }; }
    namespace cmdDisp { enum { WARN = 3, FATAL = 5 }; }
    namespace cmdSeq { enum { WARN = 3, FATAL = 5 }; }
    namespace eventLogger { enum { WARN = 3, FATAL = 5 }; }
    namespace fileDownlink { enum { WARN = 3, FATAL = 5 }; }
    namespace fileManager { enum { WARN = 3, FATAL = 5 }; }
    namespace fileUplink { enum { WARN = 3, FATAL = 5 }; }
    namespace pingRcvr { enum { WARN = 3, FATAL = 5 }; }
    namespace prmDb { enum { WARN = 3, FATAL = 5 }; }
    namespace rateGroup1Comp { enum { WARN = 3, FATAL = 5 }; }
    namespace rateGroup2Comp { enum { WARN = 3, FATAL = 5 }; }
    namespace rateGroup3Comp { enum { WARN = 3, FATAL = 5 }; }
  }

}

#endif
