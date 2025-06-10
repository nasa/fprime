#ifndef CDHCORESUBTOPOLOGY_DEFS_HPP
#define CDHCORESUBTOPOLOGY_DEFS_HPP

namespace CDHCore {

    // State for topology construction
    struct TopologyState {
    };
}

  namespace PingEntries {
    struct CDHCore_cmdDisp    { enum { WARN=3, FATAL=5 }; };
    struct CDHCore_events     { enum { WARN=3, FATAL=5 }; };
    struct CDHCore_tlmSend    { enum { WARN=3, FATAL=5 }; };
  }

#endif