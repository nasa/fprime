@ The BufferManager was unable to allocate a requested buffer
event NoBuffsAvailable(
                        $size: U32 @< The requested size
                      ) \
  severity warning high \
  id 0x00 \
  format "No available buffers of size {}" \
  throttle 10

@ The buffer manager received a zero-sized buffer as a return. Probably undetected empty buffer allocation
event ZeroSizeBuffer \
  severity warning high \
  id 0x01 \
  format "Received zero size buffer" \
  throttle 10
