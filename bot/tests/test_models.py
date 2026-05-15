import pytest

from bot.models import Contest, Problem, State, TrackedEntry


def test_problem_defaults():
    p = Problem(title="A", url="https://example.com/a")
    assert p.difficulty is None
    assert p.tags == ()


def test_contest_key_is_source_qualified():
    c = Contest(
        source="itsa",
        source_id="itsa-2026-05",
        title="ITSA May",
        url="https://itsa.org.tw/c/1",
        start_time=None,
        end_time=None,
        registration_url=None,
        problems=[],
    )
    assert c.key == "itsa:itsa-2026-05"


def test_state_roundtrip_through_dict():
    s = State(
        version=1,
        entries={
            "itsa:itsa-2026-05": TrackedEntry(issue_number=42, tracked_at="2026-05-14T02:00:00Z"),
        },
    )
    d = s.to_dict()
    s2 = State.from_dict(d)
    assert s2 == s


def test_state_from_dict_rejects_unknown_version():
    with pytest.raises(ValueError, match="unsupported state version"):
        State.from_dict({"version": 99, "entries": {}})
