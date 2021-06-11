@ The total buffers allocated
telemetry TotalBuffs: U32 id 0x00 update on change

@ The current number of allocated buffers
telemetry CurrBuffs: U32 id 0x01 update on change

@ The high water mark of allocated buffers
telemetry HiBuffs: U32 id 0x02 update on change

@ The number of requests that couldn't return a buffer
telemetry NoBuffs: U32 id 0x03 update on change \
  high {
    red 1
  }

@ The number of empty buffers returned
telemetry EmptyBuffs: U32 id 0x04 update on change \
  high {
    red 1
  }
