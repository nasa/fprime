telemetry ChannelEnum: FormalParamEnum

telemetry ChannelArray: FormalParamArray \
  id 0x10

telemetry ChannelStruct: FormalParamStruct \
  update always

telemetry ChannelU32: U32 \
  format "{x}" \
  low { red 0, orange 1, yellow 2 }

telemetry ChannelF32: F32 \
  update always \
  format "{.3f}" \
  low { red -3, orange -2, yellow -1 } \
  high { red 3, orange 2, yellow 1 }

telemetry ChannelString: string \
  update on change \
  format "{}"
