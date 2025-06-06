#ifndef COMMSSUBTOPOLOGY_DEFS_HPP
#define COMMSSUBTOPOLOGY_DEFS_HPP

namespace Comms {
    struct CommsState {
        /* include any variables that are needed for 
        configuring/starting/tearing down the topology */
    };
}

  namespace PingEntries {
    namespace Comms_cmdSeq         {enum { WARN = 3, FATAL = 5 };}
  }

#endif