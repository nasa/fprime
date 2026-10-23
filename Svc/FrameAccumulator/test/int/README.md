# CRC diagnostic integration test

Run against a live GDS and a deployment using `FprimeFrameDetector`, with CRC
warnings enabled and the default quota of one per 60 seconds. Supply a deployment
configuration mapping `Svc.FrameAccumulator` and `Svc.CommandDispatcher` to their
instance mnemonics.

The test also needs a raw-byte uplink hook before frame validation. Set
`FPRIME_CRC_INJECTOR_SOCKET` to a local AF_UNIX socket provided by the test
transport. For each connection, the hook reads one newline-terminated JSON object
`{"hex": "<bytes>"}`, injects those bytes into the target's normal driver receive
stream, and replies `{"ok": true, "bytes": <count>}` followed by a newline. Serialize
injection with normal GDS uplink writes so fragments cannot interleave.

```bash
FPRIME_CRC_INJECTOR_SOCKET=/path/to/inject.sock \
python -m pytest Svc/FrameAccumulator/test/int -q \
  --dictionary /path/to/TopologyDictionary.json \
  --deployment-config /path/to/int_config.json
```

Without the hook, this test is explicitly skipped. It takes just over one minute.
It checks incomplete-candidate suppression, decoded warning severity/arguments,
default throttling, cumulative counts while throttled, actual resynchronization
byte accounting and successful ordinary commands after corruption. Disabled mode,
alternate configuration, saturation and allocation-exhaustion cases remain unit
tests; this test does not claim hardware or flight validation.
