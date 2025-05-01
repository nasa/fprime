@ Receive raw bytes from a ComInterface (e.g. ComStub)
guarded input port dataIn: Svc.ComDataWithContext

@ Port for sending an extracted frame out
output port frameOut: Svc.ComDataWithContext

@ Port for returning ownership of buffers received on dataIn
output port bufferReturnOut: Fw.BufferSend

@ Port receiving back ownership of buffers sent on frameOut
sync input port dataReturnIn: Svc.ComDataWithContext
