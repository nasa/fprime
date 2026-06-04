module Svc {

  @ Backwards-compatibility adapter from the Fw.ComBufferSend interface to the
  @ legacy Fw.Com interface.
  @
  @ The legacy Fw.Com format prepends a FwPacketDescriptorType to the front of
  @ the ComBuffer. This component takes the explicit packetType (APID) argument
  @ from the Fw.ComBufferSend port, serializes it as a FwPacketDescriptorType
  @ at the front of the buffer, and forwards the result on a legacy Fw.Com port.
  passive component ComBufferToComAdapter {

    @ Input port carrying the payload with the descriptor as the explicit
    @ packetType (APID) argument
    sync input port comBufferSendIn: Fw.ComBufferSend

    @ Legacy Com output port: ComBuffer with a FwPacketDescriptorType prepended
    output port comOut: Fw.Com

  }

}
