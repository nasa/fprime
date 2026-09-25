@ Array of per-channel telemetry counters
@ Each element contains receive counters, sent counters, fault counters,
@ queue depths, and activity counters for one CFDP channel.
telemetry ChannelTelemetry: Cfdp.ChannelTelemetryArray

# Parameter mimics: report the current value of each parameter
telemetry PRM_LOCAL_EID: Cfdp.EntityId update on change
telemetry PRM_OUTGOING_FILE_CHUNK_SIZE: U32 update on change
telemetry PRM_RX_CRC_CALC_BYTES_PER_CYCLE: U32 update on change
telemetry PRM_POST_INACTIVITY_SEND_RETRIES: U8 update on change
telemetry PRM_FILE_IN_DEFAULT_CHANNEL: U8 update on change
telemetry PRM_FILE_IN_DEFAULT_DEST_ENTITY_ID: Cfdp.EntityId update on change
telemetry PRM_FILE_IN_DEFAULT_CLASS: Cfdp.Class update on change
telemetry PRM_FILE_IN_DEFAULT_KEEP: Cfdp.Keep update on change
telemetry PRM_FILE_IN_DEFAULT_PRIORITY: U8 update on change
telemetry PRM_CHANNEL_CONFIG: Cfdp.ChannelArrayParams update on change
