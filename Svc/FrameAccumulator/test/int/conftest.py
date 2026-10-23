"""Raw uplink hook supplied by the deployment's local test transport."""

import json
import os
import socket

import pytest


@pytest.fixture
def frame_crc_injector():
    """Inject bytes before framing validation, not through a command handler."""
    path = os.environ.get("FPRIME_CRC_INJECTOR_SOCKET")
    if not path:
        pytest.skip(
            "Provide a local raw-uplink injection socket; see test/int/README.md"
        )

    def inject(payload):
        with socket.socket(socket.AF_UNIX) as client:
            client.settimeout(5)
            client.connect(path)
            client.sendall(json.dumps({"hex": payload.hex()}).encode() + b"\n")
            with client.makefile("rb") as response:
                result = json.loads(response.readline(4096))
            assert result == {"ok": True, "bytes": len(payload)}

    return inject
