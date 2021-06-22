module Svc {

  @ A component for storing parameters
  active component PrmDb {

    # ----------------------------------------------------------------------
    # General ports 
    # ----------------------------------------------------------------------

    @ Port to get parameter values
    guarded input port getPrm: Fw.PrmGet

    @ Port to update parameters
    async input port setPrm: Fw.PrmSet

    @ Ping input port
    async input port pingIn: [1] Svc.Ping

    @ Ping output port
    output port pingOut: [1] Svc.Ping

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

    @ Command to save parameter image to file. Uses file name passed to constructor
    async command PRM_SAVE_FILE \
      opcode 0

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Parameter ID not found in database.
    event PrmIdNotFound(
                         Id: U32 @< The parameter ID
                       ) \
      severity warning low \
      id 0 \
      format "Parameter ID {} not found" \
      throttle 5

    @ Parameter ID updated in database
    event PrmIdUpdated(
                        Id: U32 @< The parameter ID
                      ) \
      severity activity high \
      id 1 \
      format "Parameter ID {} updated"

    @ Parameter database is full
    event PrmDbFull(
                     Id: U32 @< The parameter ID
                   ) \
      severity fatal \
      id 2 \
      format "Parameter DB full when adding ID {} "


    @ Parameter ID added to database
    event PrmIdAdded(
                      Id: U32 @< The parameter ID
                    ) \
      severity activity high \
      id 3 \
      format "Parameter ID {} added"

    enum PrmWriteError {
      PRM_WRITE_OPEN = 0
      PRM_WRITE_DELIMITER = 1
      PRM_WRITE_DELIMITER_SIZE = 2
      PRM_WRITE_RECORD_SIZE = 3
      PRM_WRITE_RECORD_SIZE_SIZE = 4
      PRM_WRITE_PARAMETER_ID = 5
      PRM_WRITE_PARAMETER_ID_SIZE = 6
      PRM_WRITE_PARAMETER_VALUE = 7
      PRM_WRITE_PARAMETER_VALUE_SIZE = 8
    }

    @ Failed to write parameter file
    event PrmFileWriteError(
                             stage: PrmWriteError @< The write stage
                             record: I32 @< The record that had the failure
                             error: I32 @< The error code
                           ) \
      severity warning high \
      id 4 \
      format "Parameter write failed in stage {} with record {} and error {}"

    @ Save of parameter file completed
    event PrmFileSaveComplete(
                               records: U32 @< The number of records saved
                             ) \
      severity activity high \
      id 5 \
      format "Parameter file save completed. Wrote {} records."

    enum PrmReadError {
      PRM_READ_OPEN = 0
      PRM_READ_DELIMITER = 1
      PRM_READ_DELIMITER_SIZE = 2
      PRM_READ_DELIMITER_VALUE = 3
      PRM_READ_RECORD_SIZE = 4
      PRM_READ_RECORD_SIZE_SIZE = 5
      PRM_READ_RECORD_SIZE_VALUE = 6
      PRM_READ_PARAMETER_ID = 7
      PRM_READ_PARAMETER_ID_SIZE = 8
      PRM_READ_PARAMETER_VALUE = 9
      PRM_READ_PARAMETER_VALUE_SIZE = 10
    }

    @ Failed to read parameter file
    event PrmFileReadError(
                            stage: PrmReadError @< The write stage
                            record: I32 @< The record that had the failure
                            error: I32 @< The error code
                          ) \
      severity warning high \
      id 6 \
      format "Parameter file read failed in stage {} with record {} and error {}"

    @ Load of parameter file completed
    event PrmFileLoadComplete(
                               records: U32 @< The number of records loaded
                             ) \
      severity activity high \
      id 7 \
      format "Parameter file load completed. Read {} records."

  }

}
