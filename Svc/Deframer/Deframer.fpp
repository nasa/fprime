module Svc {

  @ A component for deframing input received from the ground
  @ via a byte stream driver, which may be active or passive
  passive component Deframer {

    # ----------------------------------------------------------------------
    # Receiving framed data via push
    # ----------------------------------------------------------------------

    @ Port for receiving frame buffers FB pushed from the byte stream driver.
    @ After using a buffer FB received on this port, Deframer deallocates it
    @ by invoking framedDeallocate.
    guarded input port framedIn: Drv.ByteStreamRecv

    @ Port for deallocating buffers received on framedIn.
    output port framedDeallocate: Fw.BufferSend

    # ----------------------------------------------------------------------
    # Receiving framed data via poll
    # ----------------------------------------------------------------------

    @ Schedule in port, driven by a rate group.
    guarded input port schedIn: Svc.Sched

    @ Port that polls for data from the byte stream driver.
    @ Deframer invokes this port on its schedIn cycle, if it is connected.
    @ No allocation or occurs when invoking this port.
    @ The data transfer uses a pre-allocated frame buffer
    @ owned by Deframer.
    output port framedPoll: Drv.ByteStreamPoll

    # ----------------------------------------------------------------------
    # Memory management for deframing and for sending file packets
    # ----------------------------------------------------------------------

    @ Port for allocating Fw::Buffer objects from a buffer manager.
    @ When Deframer invokes this port, it receives a packet buffer PB and
    @ takes ownership of it. It uses PB internally for deframing.
    @ Then one of two things happens:
    @
    @ 1. PB contains a file packet, which Deframer sends on bufferOut.
    @    In this case ownership of PB passes to the receiver.
    @
    @ 2. PB does not contain a file packet, or bufferOut is unconnected.
    @    In this case Deframer deallocates PB on bufferDeallocate.
    output port bufferAllocate: Fw.BufferGet

    @ Port for sending file packets (case 1 above).
    @ The file packets are wrapped in Fw::Buffer objects allocated with
    @ bufferAllocate.
    @ Ownership of the Fw::Buffer passes to the receiver, which is
    @ responsible for the deallocation.
    output port bufferOut: Fw.BufferSend

    @ Port for deallocating temporary buffers allocated with
    @ bufferAllocate (case 2 above). Deallocation occurs here
    @ when there is nothing to send on bufferOut.
    output port bufferDeallocate: Fw.BufferSend

    # ----------------------------------------------------------------------
    # Sending command packets and receiving command responses
    # ----------------------------------------------------------------------

    @ Port for sending command packets as Com buffers.
    output port comOut: Fw.Com

    @ Port for receiving command responses from a command dispatcher.
    @ Invoking this port does nothing. The port exists to allow the matching
    @ connection in the topology.
    sync input port cmdResponseIn: Fw.CmdResponse

  }

}
