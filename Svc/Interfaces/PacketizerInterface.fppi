# ---------- Data In ----------
@ Port for receiving data in Fw::Com buffers
guarded input port comBufferIn: Fw.Com

@ Port for receiving file data stored in Fw::Buffer
guarded input port fileBufferIn: Fw.BufferSend

@ Port to receive data to packetize in Fw::Buffer, with optional context
guarded input port rawDataIn: Fw.DataWithContext

# ---------- Packet Out ----------
@ Port to output packets, with optional context
output port packetOut: Fw.DataWithContext

# ---------- Memory ----------
@ Port for returning the fileBuffer Fw::Buffer to the send for deallocation
output port fileBufferReturn: Fw.BufferSend
