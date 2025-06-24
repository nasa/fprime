#ifndef CDHCORE_PINGENTRIES_HPP
#define CDHCORE_PINGENTRIES_HPP

  namespace PingEntries {
    struct CDHCore_cmdDisp    { enum { WARN=3, FATAL=5 }; };
    struct CDHCore_events     { enum { WARN=3, FATAL=5 }; };
    struct CDHCore_tlmSend    { enum { WARN=3, FATAL=5 }; };
  }

#endif
