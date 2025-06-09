#ifndef FILEHANDLINGSUBTOPOLOGY_DEFS_HPP
#define FILEHANDLINGSUBTOPOLOGY_DEFS_HPP

namespace FileHandling {
    struct FileHandlingState {
        /* include any variables that are needed for 
        configuring/starting/tearing down the topology */
    };
}

  namespace PingEntries {
    namespace FileHandling_fileDownlink   {enum { WARN = 3, FATAL = 5 };}
    namespace FileHandling_fileManager    {enum { WARN = 3, FATAL = 5 };}
    namespace FileHandling_fileUplink     {enum { WARN = 3, FATAL = 5 };}
  }

#endif