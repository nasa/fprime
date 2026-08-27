"""XUNIA Mission Telemetry Twin bridge.

This module accepts telemetry/simulation records and emits provenance-bearing ontology
frames. It intentionally rejects command/actuation payloads and is not part of a
flight-control path.
"""

from __future__ import annotations

import json
from dataclasses import dataclass, asdict
from datetime import datetime, timezone
from hashlib import sha256
from typing import Any, Mapping

BLOCKED_KINDS = {"FLIGHT_COMMAND", "VEHICLE_COMMAND", "TELECOMMAND", "CONTROL", "ACTUATE"}
ALLOWED_KINDS = {"TELEMETRY", "EVENT", "LOG", "SIMULATION", "HEALTH"}


@dataclass(frozen=True)
class TelemetryEnvelope:
    object_type: str
    component: str
    channel: str
    value: Any
    timestamp_utc: str
    source: str
    provenance_hash: str
    simulation_only: bool = True


def _hash_payload(payload: Mapping[str, Any]) -> str:
    encoded = json.dumps(payload, sort_keys=True, separators=(",", ":"), default=str).encode("utf-8")
    return sha256(encoded).hexdigest()


def normalize_frame(frame: Mapping[str, Any], source: str = "fprimeXUNIA-simulation") -> TelemetryEnvelope:
    kind = str(frame.get("kind", "TELEMETRY")).upper()
    if kind in BLOCKED_KINDS:
        raise ValueError("REAL_WORLD_FLIGHT_CONTROL_DISABLED")
    if kind not in ALLOWED_KINDS:
        raise ValueError("UNSUPPORTED_FRAME_KIND")

    component = str(frame.get("component", "unknown"))
    channel = str(frame.get("channel", "unknown"))
    timestamp = str(frame.get("timestamp_utc") or datetime.now(timezone.utc).isoformat())
    value = frame.get("value")
    canonical = {
        "kind": kind,
        "component": component,
        "channel": channel,
        "value": value,
        "timestamp_utc": timestamp,
        "source": source,
    }
    return TelemetryEnvelope(
        object_type="FPrimeTelemetryFrame",
        component=component,
        channel=channel,
        value=value,
        timestamp_utc=timestamp,
        source=source,
        provenance_hash=_hash_payload(canonical),
    )


def to_ontology_record(frame: Mapping[str, Any], source: str = "fprimeXUNIA-simulation") -> dict[str, Any]:
    envelope = normalize_frame(frame, source)
    return asdict(envelope)
