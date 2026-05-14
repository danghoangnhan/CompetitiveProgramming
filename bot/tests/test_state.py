import json
from pathlib import Path

import pytest

from bot.models import State, TrackedEntry
from bot.state import load_state, save_state


def test_load_state_missing_file_returns_empty(tmp_path: Path):
    state = load_state(tmp_path / "missing.json")
    assert state.version == 1
    assert state.entries == {}


def test_save_and_reload_roundtrip(tmp_path: Path):
    path = tmp_path / "state.json"
    original = State(
        version=1,
        entries={
            "itsa:itsa-2026-05": TrackedEntry(42, "2026-05-14T02:00:00Z"),
            "tioj:contest-98": TrackedEntry(43, "2026-05-14T02:00:01Z"),
        },
    )
    save_state(path, original)
    reloaded = load_state(path)
    assert reloaded == original


def test_save_writes_sorted_keys_and_trailing_newline(tmp_path: Path):
    path = tmp_path / "state.json"
    state = State(
        version=1,
        entries={
            "z:1": TrackedEntry(2, "2026-05-14T02:00:00Z"),
            "a:1": TrackedEntry(1, "2026-05-14T02:00:00Z"),
        },
    )
    save_state(path, state)
    text = path.read_text()
    assert text.endswith("\n"), "file must end with newline for clean diffs"
    parsed = json.loads(text)
    assert list(parsed["entries"].keys()) == ["a:1", "z:1"]


def test_load_corrupt_json_raises(tmp_path: Path):
    path = tmp_path / "state.json"
    path.write_text("{not json")
    with pytest.raises(ValueError, match="malformed state file"):
        load_state(path)
