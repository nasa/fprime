module Svc {

    active component FileWorker {

        # ----------------------------------------------------------------------
        # Ports
        # ----------------------------------------------------------------------

        async input port writeIn: Svc.FileWrite
        output port writeDoneOut: Svc.SignalDone

        async input port readIn: Svc.FileRead
        output port readDoneOut: Svc.SignalDone

        guarded input port cancelIn: Svc.CancelStatus

        async input port verifyIn: Svc.VerifyStatus
        output port verifyDoneOut: Svc.SignalDone

        # ----------------------------------------------------------------------
        # Standard ports
        # ----------------------------------------------------------------------
        
        @ Port for requesting the current time
        time get port timeCaller

        @ Port for sending command registrations
        command reg port cmdRegOut

        @ Port for receiving commands
        command recv port cmdIn

        @ Port for sending command responses
        command resp port cmdResponseOut

        @ Port for sending textual representation of events
        text event port logTextOut

        @ Port for sending events to downlink
        event port logOut

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

        @ Port to return the value of a parameter
        param get port prmGetOut

        @ Port to set the value of a parameter
        param set port prmSetOut

        # ----------------------------------------------------------------------
        # Commands
        # ----------------------------------------------------------------------

        # ----------------------------------------------------------------------
        # Telemetry
        # ----------------------------------------------------------------------

        # ----------------------------------------------------------------------
        # Parameters
        # ----------------------------------------------------------------------

        # ----------------------------------------------------------------------
        # Events
        # ----------------------------------------------------------------------

        @ Notify we are not in IDLE state
        event NotInIdle(
            currState: U32
        ) severity warning high \
        format "Not in IDLE state, currently in state: {}" \
        throttle 2 \
        every {seconds=10}

        @ Failed to verify file
        event CrcFailed(
            crcStat: U32
        ) severity warning high \
        format "Failed CRC check with {} status" \
        throttle 2 \
        every {seconds=10}

        @ Failed CRC verification
        event CrcVerificationError(
            crcExp: U32
            crcCalculated: U32
        ) severity warning low \
        format "Failed to verify file. Expected CRC {}. Actual CRC {}"

        @ Getting file size failed
        event ReadFailedFileSize(
            fsStat: U32
        ) severity warning high \
        format "Failed to get filesize with stat {} in read handler" \
        throttle 2 \
        every {seconds=10}

        @ Getting file open failed
        event OpenFileError(
            fileName: string size MAX_STRING_BYTES
            fsStat: U32
        ) severity warning high \
        format "Failed to open file {} with stat {}" \
        throttle 2 \
        every {seconds=10}

        @ Inform we are reading from file
        event ReadBegin(
            fileSize: U64
            fileName: string size MAX_STRING_BYTES
        ) severity activity low \
        format "Reading {} bytes from {}"

        @ Inform we are done reading from file
        event ReadCompleted(
            fileSize: U64
            fileName: string size MAX_STRING_BYTES
        ) severity activity low \
        format "Finished reading {} bytes from {}"

        @ Error encountered reading to file
        event ReadError(
            bytesRead: U64
            readSize: U64
            fileName: string size MAX_STRING_BYTES
        ) severity warning high \
        format "Failed after {} of {} bytes read to {}"

        @ Completed the read operation and will return buffer
        event ReadAborted(
            bytesRead: U64
            readSize: U64
            fileName: string size MAX_STRING_BYTES
        ) severity warning low \
        format "Aborted after {} of {} bytes read to {}"

        @ Reading to file was too slow
        event ReadTimeout(
            bytesRead: U64
            readSize: U64
            fileName: string size MAX_STRING_BYTES
            timeout: U64
        ) severity warning high \
        format "Failed after {} of {} bytes read to {} after exceeding timeout of {} microseconds"

        @ Received a buffer and initiating write operation
        event WriteBegin(
            writeSize: U64
            fileName: string size MAX_STRING_BYTES
        ) severity activity low \
        format "Beginning write of size {} to {}"

        @ Completed the write operation and will return buffer
        event WriteCompleted(
            writeSize: U64
            fileName: string size MAX_STRING_BYTES
        ) severity activity low \
        format "Completed write of size {} to {}"

        @ Error encountered writing to file
        event WriteFileError(
            bytesWritten: U64
            writeSize: U64
            fileName: string size MAX_STRING_BYTES
            status: I32
        ) severity warning high \
        format "Failed after {} of {} bytes written to {} with write status {}" \
        throttle 2 \
        every {seconds=10}

        @ Error encountered while opening validation file
        event WriteValidationOpenError(
            hashFileName: string size MAX_STRING_BYTES
            status: I32
        ) severity warning high \
        format "Failed to open validation file {} with status {}" \
        throttle 2 \
        every {seconds=10}

        @ Error encountered while reading validation file
        event WriteValidationReadError(
            hashFileName: string size MAX_STRING_BYTES
            status: I32
        ) severity warning high \
        format "Failed to read validation file {} with status {}" \
        throttle 2 \
        every {seconds=10}

        @ Error encountered while creating validation file
        event WriteValidationError(
            hashFileName: string size MAX_STRING_BYTES
            bytesWritten: U64
            hashSize: U32
        ) severity warning low \
        format "Failed to create hash file {}. Wrote {} bytes when expected to write {} bytes to hash file"

        @ Writing to file was too slow
        event WriteTimeout(
            bytesWritten: U64
            writeSize: U64
            fileName: string size MAX_STRING_BYTES
            timeout: U64
        ) severity warning high \
        format "Failed after {} of {} bytes written to {} after exceeding timeout of {} microseconds"

        @ Completed the write operation and will return buffer
        event WriteAborted(
            bytesWritten: U64
            writeSize: U64
            fileName: string size MAX_STRING_BYTES
        ) severity warning low \
        format "Aborted after {} of {} bytes written to {}"
    }

}
