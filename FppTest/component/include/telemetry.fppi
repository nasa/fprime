# ----------------------------------------------------------------------
# Telemetry
# ----------------------------------------------------------------------

@ A telemetry channel with U32 data and format string
telemetry ChannelU32Format: U32 \
  format "{x}"

@ A telemetry channel with F32 data and format string
telemetry ChannelF32Format: F32 \
  format "{.3f}"

@ A telemetry channel with string data with format string
telemetry ChannelStringFormat: string \
  format "{}"

@ A telemetry channel with enum data
telemetry ChannelEnum: E \
  id 0x10

@ A telemetry channel with array data and update frequency
telemetry ChannelArrayFreq: A

@ A telemetry channel with struct data
telemetry ChannelStructFreq: S \
  update always

@ A telemetry channel with U32 data and limits
telemetry ChannelU32Limits: U32 \
  low { red 0, orange 1, yellow 2 }

@ A telemetry channel with F32 data and limits
telemetry ChannelF32Limits: F32 \
  low { red -3, orange -2, yellow -1 } \
  high { red 3, orange 2, yellow 1 }

@ A telemetry channel F64 data, update frequency, format, and limits
telemetry ChannelF64: F64 \
  update always \
  format "{e}" \
  high { red 3, orange 2, yellow 1 }

@ A telemetry channel with U32 data and update on change frequency
telemetry ChannelU32OnChange: U32 \
  update on change

@ A telemetry channel with enum data and update on change frequency
telemetry ChannelEnumOnChange: E \
  update on change

