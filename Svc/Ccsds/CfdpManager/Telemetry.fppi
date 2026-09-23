@ Array of per-channel telemetry counters
@ Each element contains receive counters, sent counters, fault counters,
@ queue depths, and activity counters for one CFDP channel.
telemetry ChannelTelemetry: Cfdp.ChannelTelemetryArray

# Parameter mimics: report the current value of each parameter
telemetry LOCAL_EID: Cfdp.EntityId
telemetry OUTGOING_FILE_CHUNK_SIZE: U32
telemetry RX_CRC_CALC_BYTES_PER_CYCLE: U32
telemetry POST_INACTIVITY_SEND_RETRIES: U8
telemetry FILE_IN_DEFAULT_CHANNEL: U8
telemetry FILE_IN_DEFAULT_DEST_ENTITY_ID: Cfdp.EntityId
telemetry FILE_IN_DEFAULT_CLASS: Cfdp.Class
telemetry FILE_IN_DEFAULT_KEEP: Cfdp.Keep
telemetry FILE_IN_DEFAULT_PRIORITY: U8
telemetry CHANNEL_CONFIG: Cfdp.ChannelArrayParams
