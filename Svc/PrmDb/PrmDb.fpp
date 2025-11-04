module Svc {

  @ A component for storing parameters
  active component PrmDb {

    # ----------------------------------------------------------------------
    # Types
    # ----------------------------------------------------------------------

    @ Parameter DB type
    enum PrmDbType : U8 {
      DB_ACTIVE,
      DB_STAGING
    }

    @ State of parameter DB file load operations
    enum PrmDbFileLoadState : U8 {
        IDLE,
        LOADING_FILE_UPDATES,
        FILE_UPDATES_STAGED,
    }


    @ Parameter read error
    enum PrmReadError : U8 {
      OPEN
      DELIMITER
      DELIMITER_SIZE
      DELIMITER_VALUE
      RECORD_SIZE
      RECORD_SIZE_SIZE
      RECORD_SIZE_VALUE
      PARAMETER_ID
      PARAMETER_ID_SIZE
      PARAMETER_VALUE
      PARAMETER_VALUE_SIZE
    }

    @ Parameter write error
    enum PrmWriteError : U8 {
      OPEN
      DELIMITER
      DELIMITER_SIZE
      RECORD_SIZE
      RECORD_SIZE_SIZE
      PARAMETER_ID
      PARAMETER_ID_SIZE
      PARAMETER_VALUE
      PARAMETER_VALUE_SIZE
    }

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Port to get parameter values
    guarded input port getPrm: Fw.PrmGet

    @ Port to update parameters
    async input port setPrm: Fw.PrmSet

    @ Ping input port
    async input port pingIn: Svc.Ping

    @ Ping output port
    output port pingOut: Svc.Ping

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Command receive port
    command recv port CmdDisp

    @ Command registration port
    command reg port CmdReg

    @ Command response port
    command resp port CmdStatus

    @ Event port
    event port Log

    @ Text event port
    text event port LogText

    @ Time get port
    time get port Time

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    include "PrmDbCmdDict.fppi"

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    include "PrmDbEventDict.fppi"

  }

}
