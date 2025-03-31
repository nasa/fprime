@ Port to receive data to frame, in a Fw::Buffer with optional context
guarded input port dataIn: Fw.DataWithContext

@ Port to output framed data, with optional context, for follow-up framing
output port framedDataOut: Fw.DataWithContext

@ Port to output framed data to a ByteStream interface
output port framedStreamOut: Drv.ByteStreamSend
