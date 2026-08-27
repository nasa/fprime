import unittest

from telemetry_bridge import normalize_frame, to_ontology_record


class TelemetryBridgeTest(unittest.TestCase):
    def test_normalizes_simulation_telemetry(self):
        record = to_ontology_record({
            "kind": "TELEMETRY",
            "component": "Demo.Health",
            "channel": "temperature_c",
            "value": 21.5,
            "timestamp_utc": "2026-08-27T19:30:00Z",
        })
        self.assertEqual(record["object_type"], "FPrimeTelemetryFrame")
        self.assertTrue(record["simulation_only"])
        self.assertEqual(len(record["provenance_hash"]), 64)

    def test_blocks_flight_command(self):
        with self.assertRaisesRegex(ValueError, "REAL_WORLD_FLIGHT_CONTROL_DISABLED"):
            normalize_frame({"kind": "FLIGHT_COMMAND", "component": "Demo", "channel": "cmd", "value": 1})


if __name__ == "__main__":
    unittest.main()
