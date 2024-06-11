module Svc {

  @ A component for storing parameters
  active component PrmDb {

    # ----------------------------------------------------------------------
    # Types
    # ----------------------------------------------------------------------

    @ Parameter read error
    enum PrmReadError {
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
    enum PrmWriteError {
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
      format "Parameter ID 0x{x} not found" \
      throttle 5

    @ Parameter ID updated in database
    event PrmIdUpdated(
                        Id: U32 @< The parameter ID
                      ) \
      severity activity high \
      id 1 \
      format "Parameter ID 0x{x} updated"

    @ Parameter database is full
    event PrmDbFull(
                     Id: U32 @< The parameter ID
                   ) \
      severity fatal \
      id 2 \
      format "Parameter DB full when adding ID 0x{x} "


    @ Parameter ID added to database
    event PrmIdAdded(
                      Id: U32 @< The parameter ID
                    ) \
      severity activity high \
      id 3 \
      format "Parameter ID 0x{x} added"

    @ Failed to write parameter file
    event PrmFileWriteError(
                             stage: PrmWriteError @< The write stage
                             $record: I32 @< The record that had the failure
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

    @ Failed to read parameter file
    event PrmFileReadError(
                            stage: PrmReadError @< The read stage
                            $record: I32 @< The record that had the failure
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
