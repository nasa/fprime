enum PrmLoadAction : U8 {
    SET_PARAMETER,
    SAVE_FILE_COMMAND,
    LOAD_FILE_COMMAND,
    COMMIT_STAGED_COMMAND,
}

@ Parameter ID not found in database.
event PrmIdNotFound(
                        Id: FwPrmIdType @< The parameter ID
                    ) \
    severity warning low \
    id 0 \
    format "Parameter ID 0x{x} not found" \
    throttle 5

@ Parameter ID updated in database
event PrmIdUpdated(
                    Id: FwPrmIdType @< The parameter ID
                    ) \
    severity activity high \
    id 1 \
    format "Parameter ID 0x{x} updated"

@ Parameter database is full
event PrmDbFull(
                    Id: FwPrmIdType @< The parameter ID
                ) \
    severity warning high \
    id 2 \
    format "Parameter DB full when adding ID 0x{x} "


@ Parameter ID added to database
event PrmIdAdded(
                    Id: FwPrmIdType @< The parameter ID
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
                            databaseString: string @< The database string
                            recordsTotal: U32 @< The number of records total
                            recordsAdded: U32 @< The number of new records added
                            recordsUpdated: U32 @< The number of records updated
                            ) \
    severity activity high \
    id 7 \
    format "Parameter file load completed. Database: {}, Records: {} ({} added and {} updated)."

@ Committed staged parameter updates
event PrmDbCommitComplete() \
    severity activity high \
    id 8 \
    format "Parameter DB commit complete, staged updates are now active."

@ All parameters Copied from one DB to another
event PrmDbCopyAllComplete(
                            databaseStringSrc: string @< The src database string
                            databaseStringDest: string @< The dest database string
                            ) \
    severity activity high \
    id 9 \
    format "All parameters copied. Source database: {}, Destination database: {}."

@ Parameter file load failed, not staging any update
event PrmDbFileLoadFailed() \
    severity warning high \
    id 10 \
    format "Parameter file load failed. Clearing staging database and abandoning parameter file load."

@ Invalid Action during parameter file load
event PrmDbFileLoadInvalidAction(
                            currentState: PrmDbFileLoadState @< The current state
                            attemptedAction: PrmLoadAction @< The invalid action attempted
) \
    severity warning low \
    id 11 \
    format "Invalid action during parameter file load. Current state: {}, Action (Invalid for current state): {}."


@ parameter file failed CRC
event PrmFileBadCrc(
                       readCrc: U32  @< The read CRC
                       compCrc: U32  @< The computed CRC
                        ) \
    severity warning high \
    id 12 \
    format "Parameter file failed CRC. Read: 0x{x} Computed: 0x{x}"

@ Invalid parameter ID range requested for a data product snapshot
event PrmDpInvalidRange(
                            startId: FwPrmIdType @< The requested start ID
                            endId: FwPrmIdType @< The requested end ID
                        ) \
    severity warning low \
    id 13 \
    format "Invalid parameter DP range requested: start 0x{x}, end 0x{x}"

@ No active-database entries fell within the requested parameter DP range
event PrmDpNoEntriesInRange(
                                startId: FwPrmIdType @< The requested start ID
                                endId: FwPrmIdType @< The requested end ID
                            ) \
    severity activity low \
    id 14 \
    format "No parameter entries found in requested DP range: start 0x{x}, end 0x{x}"

@ Failed to allocate a data product buffer for a parameter DP snapshot
event PrmDpBufferFailed(
                            $size: U32 @< The requested buffer size
                        ) \
    severity warning high \
    id 15 \
    format "Failed to allocate parameter DP buffer of size {}"

@ A parameter DP snapshot was requested, but data product generation is not usable:
@ either the data product ports are not connected, or configureDp() was never called
event PrmDpNotConfigured() \
    severity warning high \
    id 18 \
    format "Parameter DP snapshot requested, but data product generation is not configured"

@ A parameter entry was too large to fit in a data product container at the configured
@ buffer size, and was dropped from the snapshot
event PrmDpRecordTooLarge(
                            Id: FwPrmIdType @< The parameter ID that was dropped
                          ) \
    severity warning low \
    id 16 \
    format "Parameter ID 0x{x} too large for configured DP container size; entry dropped" \
    throttle 5

@ Parameter DP snapshot generation complete
event PrmDpComplete(
                        startId: FwPrmIdType @< The requested start ID
                        endId: FwPrmIdType @< The requested end ID
                        numRecords: U32 @< The number of records in the snapshot
                        numContainers: U32 @< The number of DP containers sent for the snapshot
                    ) \
    severity activity high \
    id 17 \
    format "Parameter DP snapshot complete: range [0x{x}, 0x{x}], {} records in {} containers"
