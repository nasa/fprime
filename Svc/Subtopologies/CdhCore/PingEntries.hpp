#ifndef CDHCORE_PINGENTRIES_HPP
#define CDHCORE_PINGENTRIES_HPP

namespace PingEntries {
struct CdhCore_cmdDisp {
    enum { WARN = 3, FATAL = 5 };
};
struct CdhCore_events {
    enum { WARN = 3, FATAL = 5 };
};
struct CdhCore_tlmSend {
    enum { WARN = 3, FATAL = 5 };
};
}  // namespace PingEntries

#endif
