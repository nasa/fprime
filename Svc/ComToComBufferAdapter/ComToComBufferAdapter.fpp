module Svc {

  @ Backwards-compatibility adapter from the legacy Fw.Com interface to the
  @ Fw.ComBufferSend interface.
  @
  @ The legacy Fw.Com format prepends a FwPacketDescriptorType to the front of
  @ the ComBuffer. This component reads that descriptor off the front of the
  @ buffer, strips it, and forwards the remaining payload on a Fw.ComBufferSend
  @ port with the descriptor passed as the explicit packetType (APID) argument.
  passive component ComToComBufferAdapter {

    @ Legacy Com input port: ComBuffer with a FwPacketDescriptorType prepended
    sync input port comIn: Fw.Com

    @ Output port carrying the payload (descriptor stripped) with the descriptor
    @ value passed as the explicit packetType argument
    output port comBufferSendOut: Fw.ComBufferSend

  }

}
