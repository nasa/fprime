# Communication Plugin

Communication Plugins allow users to swap out the mechanism used to send and receive raw bytes between the GDS and an F Prime deployment. These plugins provide a customizable interface for connecting to various communication layers, such as TCP/IP sockets, serial ports (UART), or custom radios.

By subclassing `BaseAdapter`, developers can integrate GDS with mission-specific or non-standard hardware interfaces while maintaining compatibility with the rest of the GDS infrastructure.

The F Prime [`IpAdapter`](https://github.com/nasa/fprime-gds/blob/0b749b54b8ff8c6b5a379a6e0adb5acacc7a3d30/src/fprime_gds/common/communication/adapters/ip.py#L46) is an example of a communication plugin supporting tcp.

Communication is a `SELECTION` type plugin meaning only one communication plugin will run. It will be selected by the user.

## Usage

The Communication Plugin is responsible for performing byte-level I/O between the GDS and the F Prime deployment. This includes:

- Reading incoming byte streams
- Writing outgoing data packets

This plugin runs in the communications process in the GDS.

## Considerations

- The plugin runs in the main GDS communication process.
- Blocking or slow I/O in `read()` or `write()` will delay the rest of the GDS.
- This plugin does **not** parse data—it only transmits raw bytes. Framing/parsing should be handled by a [Framing Plugin](./framing.md).

## Required Interface

To create a custom communication plugin, subclass the [`BaseAdapter`](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/common/communication/adapters/base.py#L16) class and implement the `read()` and `write()` methods.

```python
from fprime_gds.common.communication.adapters.base import BaseAdapter
from fprime_gds.plugin.definitions import gds_plugin

@gds_plugin(BaseAdapter)
class ExampleAdapter(BaseAdapter):
    """Example adapter that connects to a TCP socket."""

    def __init__(self):
        import socket
        self.sock = socket.create_connection(("localhost", 50000))

    def read(self) -> bytes:
        """Read raw bytes from the communication channel."""
        return self.sock.recv(4096)

    def write(self, frame: bytes):
        """Send raw bytes to the communication channel."""
        self.sock.sendall(frame)
```

> [!NOTE]
> These methods may be called frequently and must not block unnecessarily. Use non-blocking I/O or short timeouts to maintain system responsiveness.
