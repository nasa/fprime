module Svc {
module Ccsds {
    @ CcsdsSdlsFramer is a passive component supporting the Svc.Framer interface used to frame buffers into
    @ an SDLS frame. This component follows these steps to frame the SDLS frame:
    @ 1. Determine the security association index from the frame context, falling back to the SA_INDEX parameter
    @ 2. Call the SDLS encryption component to encrypt the data
    @ 3. Check the status passed forward with the encrypted data and raise an event if it indicates failure
    @ 4. Allocate a frame buffer and prepend the security association index (16 bits) to the encrypted data
    @ 5. Pass the SDLS frame to the next component in the pipeline
    passive component CcsdsSdlsFramer {

        @ Security association index used when the incoming frame context does not specify one
        param SA_INDEX: U16 default 0

        @ Event raised when the encryption helper fails
        event EncryptionFailed(status: Svc.Ccsds.SdlsStatus) severity warning high format "Failed to encrypt frame: {}"

        @ Event raised when the allocated frame buffer is too small for the security association index and encrypted data
        event BufferAllocationFailed(allocationSize: FwSizeType) severity warning high format "Failed to allocate frame buffer of size {}"

        @ Port to allocate a buffer for the SDLS frame
        output port bufferAllocate: Fw.BufferGet

        @ Port to deallocate the frame buffer once the frame is sent
        output port bufferDeallocate: Fw.BufferSend

        ##############################################################################
        #### Interfaces used by the component                                     ####
        ##############################################################################

        # This component is primarily used as a framer in the framing pipeline
        import Svc.Framer

        # Client of a SDLS encryption component/set of components used as helpers
        import Svc.Ccsds.CcsdsSdlsEncryptClient

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################

        import Fw.Event

        @ Port for requesting the current time
        time get port timeCaller

        import Fw.Command

        @ Port to return the value of a parameter
        param get port prmGetOut

        @ Port to set the value of a parameter
        param set port prmSetOut

    }
}
}
