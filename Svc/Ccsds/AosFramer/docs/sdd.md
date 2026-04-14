# Svc::Ccsds::AosFramer

The `Svc::Ccsds::AosFramer` is an implementation of the [FramerInterface](../../../Interfaces/docs/sdd.md) for the CCSDS [AOS Space Data Link Protocol](https://ccsds.org/wp-content/uploads/gravity_forms/5-448e85c647331d9cbaf66c096458bdd5/2025/10/732x0b5ec1.pdf).

It receives payload data (such as a Space Packet or Encapsulation Packet) on input and produces a AOS frame on its output port as a result. Please refer to the CCSDS [AOS specification (CCSDS 732.0-B-5)](https://ccsds.org/wp-content/uploads/gravity_forms/5-448e85c647331d9cbaf66c096458bdd5/2025/10/732x0b5ec1.pdf) for details on the frame format and protocol.

The `Svc::Ccsds::AosFramer` is designed to work in the common F Prime telemetry stack, receiving data from an upstream [`Svc::ComQueue`](../../../ComQueue/docs/sdd.md) and passing frames to a [Communications Adapter](../../../Interfaces/docs/sdd.md), such as a Radio manager component or [`Svc::ComStub`](../../../ComStub/docs/sdd.md), for transmission on the wire. It is commonly coupled with the [`Svc::Ccsds::SpacePacketFramer`](../../SpacePacketFramer/docs/sdd.md) to wrap CCSDS Space Packets into AOS frames.

## AOS Framing Support

The AOS Framer and Deframer support the following subset of CCSDS AOS SDL:

* Single, port context driven virtual channel
* Space Data Link Security (SDLS) disabled
* Frame Header Error Control Field is disabled
* Transfer Frame Insert Zone is disabled
* Transfer Frame Data Field is driven by the per-instance, compile time `fixed_frame_size` setting
* This component only supports the Virtual Channel Packet (VCP) service, using Multiplexing Protocol Data Units (M_PDU)
* Operational Control Field is disabled
* Frame Error Control Field is compile-time configurable
* This component supports packing multiple CCSDS Recognized packets (i.e. Space Packet Protocol or Encapsulation Packet Protocol) into one AOS SDL Frame. It also supports striping a single packet across multiple frames. Extra space is padded with idle packets.

## Internals

The AOS protocol specifies a fixed frame size. The maximum for all AOS framers can be configured in the `config/ComCfg.fpp` file. Individual AOS Framer instances can have their frame size overridden via the `configure` function.

The `Svc::Ccsds::AosFramer` uses an internal (member) buffer to hold the fixed size frame. The buffer **must** be returned to the AosFramer via the `dataReturnIn` port once it has been used or consumed. When the buffer returns to the AosFramer it will reuse the buffer for the next frame. Should a component want to use the frame data past the time it is returned to the AosFramer, data should be copied before the original buffer is returned to the AosFramer via the `dataReturnIn` port. 

## Port Descriptions

| Kind | Name | Port Type | Description |
|---|---|---|---|
| sync input | dataIn | Svc.ComDataWithContext | Receives data to frame, in a Fw::Buffer with optional context |
| output | dataOut | Svc.ComDataWithContext | Outputs framed data with optional context |
| output | dataReturnOut | Svc.ComDataWithContext | Returns ownership of the incoming Fw::Buffer to its sender once framing is handled |
| sync input | dataReturnIn | Svc.ComDataWithContext | Receives buffer from a deallocate call in a ComDriver component |
| sync input | comStatusIn | Fw.SuccessCondition | Receives status from downstream communication adapter per the [Communication Adapter Protocol](../../../../docs/reference/communication-adapter-interface.md#communication-adapter-protocol) |
| output | comStatusOut | Fw.SuccessCondition | Passes status through to upstream `Svc::ComQueue` per the [Framer Status Protocol](../../../../docs/reference/communication-adapter-interface.md#framer-status-protocol) |
