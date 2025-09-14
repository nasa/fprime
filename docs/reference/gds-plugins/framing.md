# Framing Plugin

Communication with an F Prime deployment happens via a communication protocol that passes byte streams, buffers, or packets. The framing plugin is used to assemble packets from the byte data, and then extract the F Prime data units from within the assembled payload. This plugin allows users to adapt the framing logic for different communication protocols (e.g., CCSDS). It does not decode F Prime data—only extracts F Prime packets from a protocol-specific frame.

The [F Prime framing plugin](https://github.com/nasa/fprime-gds/blob/0b749b54b8ff8c6b5a379a6e0adb5acacc7a3d30/src/fprime_gds/common/communication/framing.py#L94) is an example of the framing plugin implementing the F Prime framing protocol.

Framing is a `SELECTION` type plugin meaning only one framing plugin will run. It will be selected by the user.


## Usage

The Framing GDS Plugin allows users to customize how raw byte streams are framed and deframed during communication with an F Prime deployment over a direct link. This is particularly useful when integrating with hardware or protocols that require a specific framing strategy.

## Considerations

- The framing plugin runs inside the GDS communication thread.
- Poorly performing or blocking implementations can delay all communications.

## Required Interface

To create a custom framing plugin, subclass the [`FramerDeframer`](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/common/communication/framing.py#L24) class and implement the following methods:

- `def deframe(self, data: bytes, no_copy=False) -> Tuple[bytes, bytes, bytes]`:  
  Parses incoming byte data into complete packets. Returns a tuple of:
  (1) the extracted packet (or `None` if incomplete), 
  (2) remaining bytes to be parsed, and 
  (3) any discarded data.


- `def frame(self, data: bytes) -> bytes`:  
  Prepares an outgoing packet for transmission. This may include adding headers, checksums, or escape sequences.

```python title="null_framing.py"
from typing import Tuple
from fprime_gds.common.communication.framing import FramerDeframer
from fprime_gds.plugin.definitions import gds_plugin

@gds_plugin(FramerDeframer)
class NullFramer(FramerDeframer):
    """ No-op framer/deframer plugin """
    def frame(self, data: bytes) -> bytes:
        return data

    def deframe(self, data: bytes) -> Tuple[bytes, bytes, bytes]:
        return data, b"", b""
```