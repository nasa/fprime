module Svc {
module Ccsds {
    @ CcsdsSdlsDeframer is a passive component supporting the Svc.Deframer interface used to deframe buffers containing
    @ an SDLS frame. This component follows these steps to deframe the SDLS frame:
    @ 1. Check that the frame is long enough to contain the security association index (16 bits)
    @ 2. Extract the security association index from the frame
    @ 3. Call the SDLS decryption component to decrypt the frame
    @ 4. Checking the status of the decryption and raise an event if it fails
    @ 5. Passing the decrypted frame to the next component in the pipeline
    passive component CcsdsSdlsDeframer {

        @ Event raised when the frame is not long enough for the security association index of 16bits
        event InsufficientLength() severity warning high format "Failed to deframe security association index: insufficient length"

        @ Event raised when the decryption helper fails
        event DecryptionFailed(status: Svc.Ccsds.SdlsStatus) severity warning high format "Failed to decrypt frame: {}"

        @ Notify on error
        output port errorNotify: Svc.Ccsds.ErrorNotify

        ##############################################################################
        #### Interfaces used by the component                                     ####
        ##############################################################################

        # This component is primarily used as a deframer in the deframing pipeline
        import Svc.Deframer

        # Client of a SDLS decryption component/set of components used as helpers
        import Svc.Ccsds.CcsdsSdlsDecryptClient

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################

        import Fw.Event

        @ Port for requesting the current time
        time get port timeCaller

    }
}
}