from pathlib import Path
from unittest.mock import MagicMock

from bot.main import run
from bot.models import Contest, Problem, State, TrackedEntry


def make_contest(*, source_id: str, problems: list[Problem]) -> Contest:
    return Contest(
        source="fake",
        source_id=source_id,
        title=f"Fake {source_id}",
        url=f"https://example.com/{source_id}",
        start_time=None,
        end_time=None,
        registration_url=None,
        problems=problems,
    )


class FakeScraper:
    source = "fake"

    def __init__(self, contests: list[Contest], exc: Exception | None = None):
        self._contests = contests
        self._exc = exc

    def fetch(self) -> list[Contest]:
        if self._exc:
            raise self._exc
        return self._contests


def test_run_creates_issue_for_new_contest_with_problems(tmp_path: Path) -> None:
    state_path = tmp_path / "state.json"
    contest = make_contest(
        source_id="c1",
        problems=[Problem(title="A", url="https://example.com/p")],
    )
    create_issue = MagicMock(return_value=42)
    summary = run(
        scrapers=[FakeScraper([contest])],
        state_path=state_path,
        create_issue=create_issue,
        repo="owner/repo",
        token="t",
        now_iso="2026-05-14T02:00:00Z",
    )
    assert summary.new_issues == [("fake:c1", 42)]
    assert summary.failures == []
    create_issue.assert_called_once()
    _, kwargs = create_issue.call_args
    assert kwargs["labels"] == ["source:fake", "auto-tracked"]
    # State persisted
    from bot.state import load_state
    s = load_state(state_path)
    assert s.entries["fake:c1"] == TrackedEntry(42, "2026-05-14T02:00:00Z")


def test_run_skips_already_tracked(tmp_path: Path) -> None:
    state_path = tmp_path / "state.json"
    from bot.state import save_state
    save_state(state_path, State(
        version=1,
        entries={"fake:c1": TrackedEntry(7, "2026-05-13T00:00:00Z")},
    ))
    contest = make_contest(source_id="c1", problems=[Problem("A", "u")])
    create_issue = MagicMock()
    summary = run(
        scrapers=[FakeScraper([contest])],
        state_path=state_path,
        create_issue=create_issue,
        repo="owner/repo", token="t", now_iso="2026-05-14T02:00:00Z",
    )
    assert summary.new_issues == []
    create_issue.assert_not_called()


def test_run_skips_contest_with_no_problems(tmp_path: Path) -> None:
    state_path = tmp_path / "state.json"
    contest = make_contest(source_id="c2", problems=[])
    create_issue = MagicMock()
    summary = run(
        scrapers=[FakeScraper([contest])],
        state_path=state_path,
        create_issue=create_issue,
        repo="o/r", token="t", now_iso="2026-05-14T02:00:00Z",
    )
    assert summary.new_issues == []
    create_issue.assert_not_called()


def test_run_records_per_source_failure_and_continues(tmp_path: Path) -> None:
    state_path = tmp_path / "state.json"
    good = FakeScraper([make_contest(source_id="c3", problems=[Problem("A", "u")])])
    bad = FakeScraper([], exc=RuntimeError("boom"))
    create_issue = MagicMock(return_value=99)
    summary = run(
        scrapers=[bad, good],
        state_path=state_path,
        create_issue=create_issue,
        repo="o/r", token="t", now_iso="2026-05-14T02:00:00Z",
    )
    assert summary.new_issues == [("fake:c3", 99)]
    assert summary.failures == [("fake", "boom")]


def test_run_does_not_persist_state_if_create_issue_raises(tmp_path: Path) -> None:
    state_path = tmp_path / "state.json"
    contest = make_contest(source_id="c4", problems=[Problem("A", "u")])

    def boom(**kw: object) -> int:
        raise RuntimeError("api down")

    summary = run(
        scrapers=[FakeScraper([contest])],
        state_path=state_path,
        create_issue=boom,
        repo="o/r", token="t", now_iso="2026-05-14T02:00:00Z",
    )
    assert summary.new_issues == []
    assert ("fake", "api down") in [(s, m) for s, m in summary.failures]
    from bot.state import load_state
    assert load_state(state_path).entries == {}
