@ Receiving data (Fw::Buffer) to be routed with optional context to help with routing
sync input port dataIn: Fw.DataWithContext

@ Port for sending file packets as Fw::Buffer (ownership passed to receiver)
output port fileOut: Fw.BufferSend

@ Port for sending command packets as Fw::ComBuffers
output port commandOut: Fw.Com

@ Port for receiving command responses from a command dispatcher (can be a no-op)
sync input port cmdResponseIn: Fw.CmdResponse
