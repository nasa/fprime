# ======================================================================
# FPP file for data products configuration
# ======================================================================

module Fw {

  module DpCfg {

    @ The size in bytes of the user-configurable data in the container
    @ packet header
    constant CONTAINER_USER_DATA_SIZE = 32;

    @ A bit mask for selecting the type of processing to perform on
    @ a container before writing it to disk.
    enum ProcType: U8 {
      @ Processing type 0
      PROC_TYPE_ZERO = 0x01
      @ Processing type 1
      PROC_TYPE_ONE = 0x02
      @ Processing type 2
      PROC_TYPE_TWO = 0x04
    }

  }

}
