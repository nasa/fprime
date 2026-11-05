# ======================================================================
# FPP file for FileManager configuration
# ======================================================================

module Svc {

  module FileManagerCfg {

    @ The default priority of file data product containers. Projects can
    @ adjust this, and the GenerateDp command can override it per request.
    constant DEFAULT_DP_PRIORITY = 10;

  }

}
