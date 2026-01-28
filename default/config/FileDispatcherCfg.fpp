# ======================================================================
# FPP file for data products configuration
# ======================================================================

module Svc {

  module FileDispatcherCfg {

    @ An enumeration that maps file types to ports in the FileDispatcher component.
    @ Projects can add custom dispatcher ports by extending this enumeration.

    enum FileDispatchPort: U8 {
      @ Sequence file port
      SEQUENCE_FILE_PORT = 0
      @ Bundle file port
      BUNDLE_FILE_PORT = 1
      @ User file port (make up your own file types starting here)
      USER_FILE_PORT = 2
      @ Maximum number of file dispatch ports
      MAX_FILE_DISPATCH_PORTS = 3
    }

    constant FILE_DISPATCHER_MAX_TABLE_SIZE = 10;

  }

}
