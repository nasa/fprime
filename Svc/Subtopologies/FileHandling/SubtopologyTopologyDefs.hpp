#ifndef FILEHANDLINGSUBTOPOLOGY_DEFS_HPP
#define FILEHANDLINGSUBTOPOLOGY_DEFS_HPP

namespace FileHandling {
    // State for topology construction
    struct TopologyState {
    };
}

  namespace PingEntries {
    namespace FileHandling_fileDownlink   {enum { WARN = 3, FATAL = 5 };}
    namespace FileHandling_fileManager    {enum { WARN = 3, FATAL = 5 };}
    namespace FileHandling_fileUplink     {enum { WARN = 3, FATAL = 5 };}
    namespace FileHandling_prmDb          {enum { WARN = 3, FATAL = 5 };}
  }

#endif