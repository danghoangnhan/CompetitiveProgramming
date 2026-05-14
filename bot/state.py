from __future__ import annotations

import json
from pathlib import Path

from .models import State


def load_state(path: Path) -> State:
    """Load state from JSON. Returns an empty State if the file does not exist."""
    if not path.exists():
        return State(version=State.SUPPORTED_VERSION, entries={})
    try:
        data = json.loads(path.read_text())
    except json.JSONDecodeError as e:
        raise ValueError(f"malformed state file at {path}: {e}") from e
    return State.from_dict(data)


def save_state(path: Path, state: State) -> None:
    """Write state to JSON with sorted keys and a trailing newline (clean git diffs)."""
    path.parent.mkdir(parents=True, exist_ok=True)
    payload = state.to_dict()
    payload["entries"] = dict(sorted(payload["entries"].items()))
    path.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n")
