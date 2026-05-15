# Contest Tracker Bot Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Ship a GitHub Actions–hosted Python bot that auto-opens one GitHub issue per newly published Taiwanese programming contest (ITSA, TIOJ, ZeroJudge, TOI), with per-problem detail in each issue.

**Architecture:** Python 3.12 package under `bot/`, dataclass-based models, JSON state file in `data/`, one scraper module per source implementing a shared `Scraper` Protocol, orchestrator in `bot/main.py`, scheduled by `.github/workflows/contest-tracker.yml` (daily cron + manual dispatch). State is committed back to `main` by `stefanzweifel/git-auto-commit-action`. Issues are created via direct `httpx` calls to the GitHub REST API using the runner-provided `GITHUB_TOKEN`. No external services.

**Tech Stack:** Python 3.12 · `httpx` · `beautifulsoup4` + `lxml` · `pytest` · `ruff` · `mypy --strict` · GitHub Actions · `hatchling` build backend

**Spec:** `docs/superpowers/specs/2026-05-14-contest-tracker-bot-design.md`

**Incremental delivery:** Tasks 1–10 deliver a working bot that tracks **ITSA only** and can be merged + deployed for validation. Tasks 11–15 add the remaining sources (TIOJ, ZeroJudge, TOI) one at a time, plus polish.

**File responsibilities:**

| File | Purpose |
|---|---|
| `bot/pyproject.toml` | Package metadata + dependencies + build backend |
| `bot/__init__.py` | Package marker |
| `bot/models.py` | `Problem`, `Contest`, `TrackedEntry` dataclasses + `State` container |
| `bot/state.py` | Load/save `data/tracked_contests.json` |
| `bot/github_issue.py` | Render markdown body + REST call to create an issue |
| `bot/scrapers/__init__.py` | Registry: list of enabled scrapers |
| `bot/scrapers/base.py` | `Scraper` Protocol + retry helper |
| `bot/scrapers/itsa.py` | ITSA scraper |
| `bot/scrapers/tioj.py` | TIOJ scraper |
| `bot/scrapers/zerojudge.py` | ZeroJudge scraper |
| `bot/scrapers/toi.py` | TOI scraper |
| `bot/main.py` | Orchestrator: load state → scrape → dedup → create issues → save state |
| `bot/tests/...` | Pytest unit tests + HTML fixtures |
| `.github/workflows/contest-tracker.yml` | Daily cron + dispatch workflow |
| `.github/workflows/lint.yml` | Ruff/mypy/pytest on PRs touching `bot/` |
| `data/tracked_contests.json` | Bot-managed state, committed each run |

---

## Task 1: Scaffold the project

**Files:**
- Create: `bot/pyproject.toml`
- Create: `bot/__init__.py`
- Create: `bot/scrapers/__init__.py`
- Create: `bot/tests/__init__.py`
- Create: `bot/tests/fixtures/.gitkeep`
- Create: `data/tracked_contests.json`
- Modify: `.gitignore`

- [ ] **Step 1: Create `bot/pyproject.toml`**

```toml
[build-system]
requires = ["hatchling"]
build-backend = "hatchling.build"

[project]
name = "contest-tracker-bot"
version = "0.1.0"
description = "Auto-tracks Taiwanese competitive programming contests as GitHub issues"
requires-python = ">=3.12"
dependencies = [
    "httpx>=0.27",
    "beautifulsoup4>=4.12",
    "lxml>=5.2",
]

[project.optional-dependencies]
dev = [
    "pytest>=8",
    "ruff>=0.5",
    "mypy>=1.10",
]

[tool.hatch.build.targets.wheel]
packages = ["."]

[tool.ruff]
line-length = 100
target-version = "py312"

[tool.mypy]
strict = true
python_version = "3.12"

[[tool.mypy.overrides]]
module = ["bs4", "bs4.*", "lxml", "lxml.*"]
ignore_missing_imports = true
follow_imports = "silent"
```

The `bs4`/`lxml` override is required because BeautifulSoup4 does not ship type stubs; without it, `mypy --strict` would reject every scraper that imports `bs4`.

- [ ] **Step 2: Create empty package markers**

Create the following files with empty content:
- `bot/__init__.py`
- `bot/scrapers/__init__.py`
- `bot/tests/__init__.py`
- `bot/tests/fixtures/.gitkeep`

- [ ] **Step 3: Create initial state file**

Create `data/tracked_contests.json` with:

```json
{
  "version": 1,
  "entries": {}
}
```

- [ ] **Step 4: Update `.gitignore`**

Read current `.gitignore`, then add these entries (preserve existing lines):

```
__pycache__/
*.py[cod]
.pytest_cache/
.mypy_cache/
.ruff_cache/
*.egg-info/
.venv/
```

- [ ] **Step 5: Verify install works**

Run: `python -m venv .venv && source .venv/bin/activate && pip install -e bot/[dev]`
Expected: install succeeds with no errors; `pip show contest-tracker-bot` lists version 0.1.0.

- [ ] **Step 6: Commit**

```bash
git add bot/ data/tracked_contests.json .gitignore
git commit -m "feat(bot): scaffold contest-tracker package"
```

---

## Task 2: Data models with tests

**Files:**
- Create: `bot/models.py`
- Test: `bot/tests/test_models.py`

- [ ] **Step 1: Write failing tests for models**

Create `bot/tests/test_models.py`:

```python
from datetime import datetime, timezone
from bot.models import Contest, Problem, TrackedEntry, State


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
    import pytest
    with pytest.raises(ValueError, match="unsupported state version"):
        State.from_dict({"version": 99, "entries": {}})
```

- [ ] **Step 2: Run the test to verify it fails**

Run: `pytest bot/tests/test_models.py -v`
Expected: FAIL with `ModuleNotFoundError: No module named 'bot.models'`

- [ ] **Step 3: Implement `bot/models.py`**

```python
from __future__ import annotations

from dataclasses import dataclass, field
from datetime import datetime
from typing import Any


@dataclass(frozen=True)
class Problem:
    title: str
    url: str
    difficulty: str | None = None
    tags: tuple[str, ...] = ()


@dataclass(frozen=True)
class Contest:
    source: str
    source_id: str
    title: str
    url: str
    start_time: datetime | None
    end_time: datetime | None
    registration_url: str | None
    problems: list[Problem] = field(default_factory=list)

    @property
    def key(self) -> str:
        return f"{self.source}:{self.source_id}"


@dataclass(frozen=True)
class TrackedEntry:
    issue_number: int
    tracked_at: str

    def to_dict(self) -> dict[str, Any]:
        return {"issue_number": self.issue_number, "tracked_at": self.tracked_at}

    @classmethod
    def from_dict(cls, d: dict[str, Any]) -> "TrackedEntry":
        return cls(issue_number=int(d["issue_number"]), tracked_at=str(d["tracked_at"]))


@dataclass
class State:
    version: int
    entries: dict[str, TrackedEntry]

    SUPPORTED_VERSION = 1

    def to_dict(self) -> dict[str, Any]:
        return {
            "version": self.version,
            "entries": {k: v.to_dict() for k, v in self.entries.items()},
        }

    @classmethod
    def from_dict(cls, d: dict[str, Any]) -> "State":
        version = int(d.get("version", 0))
        if version != cls.SUPPORTED_VERSION:
            raise ValueError(f"unsupported state version: {version}")
        entries = {k: TrackedEntry.from_dict(v) for k, v in d.get("entries", {}).items()}
        return cls(version=version, entries=entries)
```

- [ ] **Step 4: Run tests to verify pass**

Run: `pytest bot/tests/test_models.py -v`
Expected: 4 passed.

- [ ] **Step 5: Commit**

```bash
git add bot/models.py bot/tests/test_models.py
git commit -m "feat(bot): add Contest/Problem/TrackedEntry/State models"
```

---

## Task 3: State persistence

**Files:**
- Create: `bot/state.py`
- Test: `bot/tests/test_state.py`

- [ ] **Step 1: Write failing tests**

Create `bot/tests/test_state.py`:

```python
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
```

- [ ] **Step 2: Run to confirm failure**

Run: `pytest bot/tests/test_state.py -v`
Expected: FAIL with `ModuleNotFoundError: No module named 'bot.state'`

- [ ] **Step 3: Implement `bot/state.py`**

```python
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
```

- [ ] **Step 4: Run tests**

Run: `pytest bot/tests/test_state.py -v`
Expected: 4 passed.

- [ ] **Step 5: Commit**

```bash
git add bot/state.py bot/tests/test_state.py
git commit -m "feat(bot): load/save JSON state with sorted-key output"
```

---

## Task 4: Issue body rendering

**Files:**
- Create: `bot/github_issue.py` (rendering only — REST call comes in Task 5)
- Test: `bot/tests/test_issue_render.py`

- [ ] **Step 1: Write failing tests**

Create `bot/tests/test_issue_render.py`:

```python
from datetime import datetime
from zoneinfo import ZoneInfo

from bot.github_issue import render_issue_body, render_issue_title
from bot.models import Contest, Problem


def make_contest(**overrides) -> Contest:
    tz = ZoneInfo("Asia/Taipei")
    defaults: dict = dict(
        source="itsa",
        source_id="itsa-2026-05",
        title="ITSA 2026-05 Monthly",
        url="https://itsa.org.tw/c/itsa-2026-05",
        start_time=datetime(2026, 5, 20, 19, 0, tzinfo=tz),
        end_time=datetime(2026, 5, 20, 22, 0, tzinfo=tz),
        registration_url="https://itsa.org.tw/c/itsa-2026-05/register",
        problems=[
            Problem(title="A. Sum", url="https://itsa.org.tw/p/1", difficulty="★★☆",
                    tags=("math",)),
            Problem(title="B. Tree DP", url="https://itsa.org.tw/p/2"),
        ],
    )
    defaults.update(overrides)
    return Contest(**defaults)


def test_title_includes_source_prefix():
    c = make_contest()
    assert render_issue_title(c) == "[itsa] ITSA 2026-05 Monthly"


def test_body_contains_contest_url_and_schedule():
    body = render_issue_body(make_contest(), now_iso="2026-05-14T02:00:00Z")
    assert "https://itsa.org.tw/c/itsa-2026-05" in body
    assert "2026-05-20 19:00" in body
    assert "Asia/Taipei" in body


def test_body_problem_table_links_each_problem():
    body = render_issue_body(make_contest(), now_iso="2026-05-14T02:00:00Z")
    assert "| A. Sum |" not in body, "should render as a markdown link, not plain text"
    assert "[A. Sum](https://itsa.org.tw/p/1)" in body
    assert "[B. Tree DP](https://itsa.org.tw/p/2)" in body


def test_body_omits_difficulty_and_tags_when_absent():
    body = render_issue_body(make_contest(), now_iso="2026-05-14T02:00:00Z")
    # B has no difficulty or tags — its row should show em-dashes
    assert "| B. Tree DP" in body
    assert "| — | — |" in body


def test_body_omits_registration_line_when_none():
    c = make_contest(registration_url=None)
    body = render_issue_body(c, now_iso="2026-05-14T02:00:00Z")
    assert "Registration" not in body


def test_body_omits_schedule_when_times_none():
    c = make_contest(start_time=None, end_time=None)
    body = render_issue_body(c, now_iso="2026-05-14T02:00:00Z")
    assert "Schedule" not in body
```

- [ ] **Step 2: Run to confirm failure**

Run: `pytest bot/tests/test_issue_render.py -v`
Expected: FAIL with `ModuleNotFoundError: No module named 'bot.github_issue'`

- [ ] **Step 3: Implement rendering in `bot/github_issue.py`**

```python
from __future__ import annotations

from datetime import datetime
from zoneinfo import ZoneInfo

from .models import Contest, Problem

TAIPEI = ZoneInfo("Asia/Taipei")


def render_issue_title(contest: Contest) -> str:
    return f"[{contest.source}] {contest.title}"


def _fmt_dt(dt: datetime) -> str:
    return dt.astimezone(TAIPEI).strftime("%Y-%m-%d %H:%M")


def _problem_row(p: Problem, idx: int) -> str:
    letter = chr(ord("A") + idx) if idx < 26 else str(idx + 1)
    link = f"[{p.title}]({p.url})"
    difficulty = p.difficulty if p.difficulty else "—"
    tags = ", ".join(p.tags) if p.tags else "—"
    return f"| {letter} | {link} | {difficulty} | {tags} |"


def render_issue_body(contest: Contest, *, now_iso: str) -> str:
    lines: list[str] = [
        f"# {contest.title}",
        "",
        f"**Source:** `{contest.source}` • [Contest page]({contest.url})",
    ]
    if contest.start_time and contest.end_time:
        lines.append(
            f"**Schedule:** {_fmt_dt(contest.start_time)} → "
            f"{_fmt_dt(contest.end_time)} (Asia/Taipei)"
        )
    if contest.registration_url:
        lines.append(f"**Registration:** [link]({contest.registration_url})")
    lines.extend(["", "## Problems", "", "| # | Problem | Difficulty | Tags |",
                  "|---|---------|-----------|------|"])
    for idx, p in enumerate(contest.problems):
        lines.append(_problem_row(p, idx))
    lines.extend([
        "",
        "---",
        f"*Auto-tracked by `contest-tracker` on {now_iso}. "
        "Edit freely — the bot only creates, it does not overwrite.*",
    ])
    return "\n".join(lines) + "\n"
```

- [ ] **Step 4: Run tests**

Run: `pytest bot/tests/test_issue_render.py -v`
Expected: 6 passed.

- [ ] **Step 5: Commit**

```bash
git add bot/github_issue.py bot/tests/test_issue_render.py
git commit -m "feat(bot): render issue title and body for a Contest"
```

---

## Task 5: GitHub REST client (`create_issue`)

**Files:**
- Modify: `bot/github_issue.py` (append `create_issue` function)
- Test: `bot/tests/test_github_client.py`

- [ ] **Step 1: Write failing tests**

Create `bot/tests/test_github_client.py`:

```python
from unittest.mock import MagicMock

import httpx
import pytest

from bot.github_issue import create_issue


class FakeTransport(httpx.BaseTransport):
    def __init__(self, response_factory):
        self._factory = response_factory
        self.calls: list[httpx.Request] = []

    def handle_request(self, request: httpx.Request) -> httpx.Response:
        self.calls.append(request)
        return self._factory(request, len(self.calls))


def test_create_issue_posts_to_correct_url_with_token():
    transport = FakeTransport(
        lambda req, n: httpx.Response(201, json={"number": 42}, request=req)
    )
    client = httpx.Client(transport=transport, base_url="https://api.github.com")
    n = create_issue(
        client,
        repo="owner/repo",
        token="t0ken",
        title="hello",
        body="world",
        labels=["a", "b"],
    )
    assert n == 42
    req = transport.calls[0]
    assert str(req.url) == "https://api.github.com/repos/owner/repo/issues"
    assert req.headers["Authorization"] == "Bearer t0ken"
    assert req.headers["Accept"] == "application/vnd.github+json"
    import json as _json
    payload = _json.loads(req.content)
    assert payload == {"title": "hello", "body": "world", "labels": ["a", "b"]}


def test_create_issue_retries_on_5xx_then_succeeds():
    def factory(req, n):
        if n < 3:
            return httpx.Response(503, request=req)
        return httpx.Response(201, json={"number": 99}, request=req)

    transport = FakeTransport(factory)
    client = httpx.Client(transport=transport, base_url="https://api.github.com")
    n = create_issue(client, "owner/repo", "t", "x", "y", labels=[], sleep=lambda s: None)
    assert n == 99
    assert len(transport.calls) == 3


def test_create_issue_raises_after_exhausting_retries():
    transport = FakeTransport(lambda req, n: httpx.Response(503, request=req))
    client = httpx.Client(transport=transport, base_url="https://api.github.com")
    with pytest.raises(RuntimeError, match="GitHub create_issue failed after 3 attempts"):
        create_issue(client, "owner/repo", "t", "x", "y", labels=[], sleep=lambda s: None)


def test_create_issue_does_not_retry_on_4xx():
    transport = FakeTransport(lambda req, n: httpx.Response(422, json={"message": "bad"},
                                                            request=req))
    client = httpx.Client(transport=transport, base_url="https://api.github.com")
    with pytest.raises(RuntimeError, match="GitHub create_issue failed: 422"):
        create_issue(client, "owner/repo", "t", "x", "y", labels=[], sleep=lambda s: None)
    assert len(transport.calls) == 1
```

- [ ] **Step 2: Run to confirm failure**

Run: `pytest bot/tests/test_github_client.py -v`
Expected: FAIL with `ImportError: cannot import name 'create_issue' from 'bot.github_issue'`

- [ ] **Step 3: Append `create_issue` to `bot/github_issue.py`**

Append the following to `bot/github_issue.py` (do not remove existing rendering functions):

```python
import time
from typing import Callable, Iterable

import httpx


def create_issue(
    client: httpx.Client,
    repo: str,
    token: str,
    title: str,
    body: str,
    labels: Iterable[str],
    *,
    max_attempts: int = 3,
    sleep: Callable[[float], None] = time.sleep,
) -> int:
    """POST a new issue. Retries on 5xx with exponential backoff. Returns issue number."""
    headers = {
        "Authorization": f"Bearer {token}",
        "Accept": "application/vnd.github+json",
        "X-GitHub-Api-Version": "2022-11-28",
    }
    payload = {"title": title, "body": body, "labels": list(labels)}
    last_status: int | None = None
    for attempt in range(1, max_attempts + 1):
        r = client.post(f"/repos/{repo}/issues", json=payload, headers=headers, timeout=30.0)
        last_status = r.status_code
        if r.status_code == 201:
            return int(r.json()["number"])
        if 400 <= r.status_code < 500:
            raise RuntimeError(f"GitHub create_issue failed: {r.status_code} {r.text}")
        if attempt < max_attempts:
            sleep(2.0 ** (attempt - 1))
    raise RuntimeError(f"GitHub create_issue failed after {max_attempts} attempts (last status {last_status})")
```

- [ ] **Step 4: Run all GitHub-client tests**

Run: `pytest bot/tests/test_github_client.py bot/tests/test_issue_render.py -v`
Expected: 10 passed (6 from render + 4 from client).

- [ ] **Step 5: Commit**

```bash
git add bot/github_issue.py bot/tests/test_github_client.py
git commit -m "feat(bot): add create_issue REST client with 5xx retry"
```

---

## Task 6: Scraper Protocol and registry skeleton

**Files:**
- Create: `bot/scrapers/base.py`
- Modify: `bot/scrapers/__init__.py`
- Test: `bot/tests/test_scraper_base.py`

- [ ] **Step 1: Write failing test for the Protocol contract**

Create `bot/tests/test_scraper_base.py`:

```python
from bot.models import Contest
from bot.scrapers.base import Scraper


class FakeScraper:
    source = "fake"

    def fetch(self) -> list[Contest]:
        return []


def test_fake_scraper_satisfies_protocol():
    s: Scraper = FakeScraper()   # type: ignore[assignment]
    assert s.source == "fake"
    assert s.fetch() == []


def test_registry_starts_empty_for_iteration():
    from bot.scrapers import SCRAPERS
    # SCRAPERS is a list; concrete scrapers append themselves over Tasks 7/11/12/13.
    assert isinstance(SCRAPERS, list)
```

- [ ] **Step 2: Run to confirm failure**

Run: `pytest bot/tests/test_scraper_base.py -v`
Expected: FAIL with `ModuleNotFoundError: No module named 'bot.scrapers.base'`

- [ ] **Step 3: Implement `bot/scrapers/base.py`**

```python
from __future__ import annotations

from typing import Protocol

from ..models import Contest


class Scraper(Protocol):
    """A read-only contest source.

    ``fetch()`` returns every contest the scraper can currently see. A contest with an empty
    ``problems`` list is treated as "not yet published" by the orchestrator and is silently
    skipped (no issue is created) until problems show up.
    """

    source: str

    def fetch(self) -> list[Contest]: ...
```

- [ ] **Step 4: Implement empty registry in `bot/scrapers/__init__.py`**

Overwrite `bot/scrapers/__init__.py` with:

```python
from __future__ import annotations

from .base import Scraper

# Concrete scrapers append themselves as they are added.
SCRAPERS: list[Scraper] = []

__all__ = ["Scraper", "SCRAPERS"]
```

- [ ] **Step 5: Run tests**

Run: `pytest bot/tests/test_scraper_base.py -v`
Expected: 2 passed.

- [ ] **Step 6: Commit**

```bash
git add bot/scrapers/base.py bot/scrapers/__init__.py bot/tests/test_scraper_base.py
git commit -m "feat(bot): add Scraper Protocol and empty registry"
```

---

## Task 7: ITSA scraper

> **Note on real-world HTML:** The unit test uses a synthetic minimal fixture that captures the structural pattern of an ITSA contest listing. After the unit test passes, the engineer MUST also capture a live HTML snapshot from `https://e-tutor.itsa.org.tw/e-Tutor/course/category.php?id=716` (or whichever ITSA listing matches the user's interest), save it as `bot/tests/fixtures/itsa_live_<YYYY-MM-DD>.html`, and update the parser's selectors if they differ. This is documented in step 7 below.

**Files:**
- Create: `bot/scrapers/itsa.py`
- Create: `bot/tests/fixtures/itsa_synthetic.html`
- Modify: `bot/scrapers/__init__.py`
- Test: `bot/tests/test_itsa.py`

- [ ] **Step 1: Create synthetic HTML fixture**

Create `bot/tests/fixtures/itsa_synthetic.html`:

```html
<!DOCTYPE html>
<html><body>
<div class="category">
  <h2>ITSA 程式競賽</h2>
  <ul class="contest-list">
    <li class="contest" data-contest-id="itsa-2026-05">
      <a href="https://e-tutor.itsa.org.tw/contest/itsa-2026-05" class="contest-title">
        ITSA 2026-05 月賽
      </a>
      <span class="contest-time" data-start="2026-05-20T19:00:00+08:00"
                                 data-end="2026-05-20T22:00:00+08:00">
        2026/05/20 19:00 ~ 22:00
      </span>
      <a class="register-link" href="https://e-tutor.itsa.org.tw/contest/itsa-2026-05/register">
        報名
      </a>
      <ol class="problem-list">
        <li><a href="https://e-tutor.itsa.org.tw/problem/p1">A. 兩數之和</a>
            <span class="difficulty">★★☆</span>
            <span class="tags">math, greedy</span></li>
        <li><a href="https://e-tutor.itsa.org.tw/problem/p2">B. 樹狀DP</a>
            <span class="difficulty">★★★</span>
            <span class="tags">tree, dp</span></li>
      </ol>
    </li>
    <li class="contest" data-contest-id="itsa-2026-06">
      <a href="https://e-tutor.itsa.org.tw/contest/itsa-2026-06" class="contest-title">
        ITSA 2026-06 月賽
      </a>
      <span class="contest-time" data-start="2026-06-20T19:00:00+08:00"
                                 data-end="2026-06-20T22:00:00+08:00">
        TBD
      </span>
      <!-- no register-link, no problem-list — should be skipped -->
    </li>
  </ul>
</div>
</body></html>
```

- [ ] **Step 2: Write failing tests**

Create `bot/tests/test_itsa.py`:

```python
from datetime import datetime, timezone, timedelta
from pathlib import Path

import pytest

from bot.scrapers.itsa import ItsaScraper, parse_contests

FIXTURE = Path(__file__).parent / "fixtures" / "itsa_synthetic.html"


def test_parse_contests_extracts_two_entries():
    contests = parse_contests(FIXTURE.read_text())
    assert len(contests) == 2


def test_parse_contests_first_has_problems_populated():
    contests = parse_contests(FIXTURE.read_text())
    c = contests[0]
    assert c.source == "itsa"
    assert c.source_id == "itsa-2026-05"
    assert c.title == "ITSA 2026-05 月賽"
    assert c.url == "https://e-tutor.itsa.org.tw/contest/itsa-2026-05"
    assert c.registration_url == "https://e-tutor.itsa.org.tw/contest/itsa-2026-05/register"
    tz = timezone(timedelta(hours=8))
    assert c.start_time == datetime(2026, 5, 20, 19, 0, tzinfo=tz)
    assert c.end_time == datetime(2026, 5, 20, 22, 0, tzinfo=tz)
    assert len(c.problems) == 2
    p = c.problems[0]
    assert p.title == "A. 兩數之和"
    assert p.url == "https://e-tutor.itsa.org.tw/problem/p1"
    assert p.difficulty == "★★☆"
    assert p.tags == ("math", "greedy")


def test_parse_contests_second_has_empty_problems():
    contests = parse_contests(FIXTURE.read_text())
    assert contests[1].source_id == "itsa-2026-06"
    assert contests[1].problems == []
    assert contests[1].registration_url is None


def test_scraper_source_attribute():
    assert ItsaScraper().source == "itsa"


def test_scraper_registered():
    from bot.scrapers import SCRAPERS
    assert any(s.source == "itsa" for s in SCRAPERS)
```

- [ ] **Step 3: Run to confirm failure**

Run: `pytest bot/tests/test_itsa.py -v`
Expected: FAIL with `ModuleNotFoundError: No module named 'bot.scrapers.itsa'`

- [ ] **Step 4: Implement `bot/scrapers/itsa.py`**

```python
from __future__ import annotations

import time
from datetime import datetime
from typing import Callable

import httpx
from bs4 import BeautifulSoup, Tag

from ..models import Contest, Problem

USER_AGENT = (
    "contest-tracker-bot/1.0 "
    "(+https://github.com/danghoangnhan/CompetitiveProgramming)"
)
ITSA_LISTING_URL = "https://e-tutor.itsa.org.tw/e-Tutor/course/category.php?id=716"


def parse_contests(html: str) -> list[Contest]:
    soup = BeautifulSoup(html, "lxml")
    out: list[Contest] = []
    for li in soup.select("li.contest"):
        source_id = li.get("data-contest-id")
        title_a = li.select_one("a.contest-title")
        if not source_id or not title_a:
            continue
        url = str(title_a.get("href", "")).strip()
        title = title_a.get_text(strip=True)

        time_span = li.select_one(".contest-time")
        start_time = _parse_iso(time_span.get("data-start")) if time_span else None
        end_time = _parse_iso(time_span.get("data-end")) if time_span else None

        reg_a = li.select_one("a.register-link")
        registration_url = str(reg_a.get("href")).strip() if reg_a else None

        problems = _parse_problems(li)
        out.append(Contest(
            source="itsa",
            source_id=str(source_id),
            title=title,
            url=url,
            start_time=start_time,
            end_time=end_time,
            registration_url=registration_url,
            problems=problems,
        ))
    return out


def _parse_iso(value: str | None) -> datetime | None:
    if not value:
        return None
    return datetime.fromisoformat(value)


def _parse_problems(contest_li: Tag) -> list[Problem]:
    problems: list[Problem] = []
    for li in contest_li.select("ol.problem-list > li"):
        a = li.select_one("a")
        if not a:
            continue
        diff_el = li.select_one(".difficulty")
        tags_el = li.select_one(".tags")
        tags: tuple[str, ...] = ()
        if tags_el:
            tags = tuple(t.strip() for t in tags_el.get_text().split(",") if t.strip())
        problems.append(Problem(
            title=a.get_text(strip=True),
            url=str(a.get("href", "")).strip(),
            difficulty=diff_el.get_text(strip=True) if diff_el else None,
            tags=tags,
        ))
    return problems


class ItsaScraper:
    source = "itsa"

    def __init__(
        self,
        url: str = ITSA_LISTING_URL,
        client_factory: Callable[[], httpx.Client] | None = None,
        sleep: Callable[[float], None] = time.sleep,
    ) -> None:
        self._url = url
        self._sleep = sleep
        self._client_factory = client_factory or (
            lambda: httpx.Client(headers={"User-Agent": USER_AGENT}, timeout=30.0)
        )

    def fetch(self) -> list[Contest]:
        with self._client_factory() as client:
            for attempt in range(1, 4):
                r = client.get(self._url)
                if 200 <= r.status_code < 300:
                    return parse_contests(r.text)
                if 400 <= r.status_code < 500:
                    raise RuntimeError(f"itsa fetch failed: {r.status_code}")
                if attempt < 3:
                    self._sleep(2.0 ** (attempt - 1))
            raise RuntimeError(f"itsa fetch failed after 3 attempts (last {r.status_code})")
```

- [ ] **Step 5: Register the scraper**

Replace the body of `bot/scrapers/__init__.py` with:

```python
from __future__ import annotations

from .base import Scraper
from .itsa import ItsaScraper

SCRAPERS: list[Scraper] = [ItsaScraper()]

__all__ = ["Scraper", "SCRAPERS"]
```

- [ ] **Step 6: Run tests**

Run: `pytest bot/tests/test_itsa.py -v`
Expected: 5 passed.

- [ ] **Step 7: Capture live HTML and verify against real site (manual)**

Run: `curl -A "contest-tracker-bot/1.0" -o bot/tests/fixtures/itsa_live_$(date -u +%Y-%m-%d).html "https://e-tutor.itsa.org.tw/e-Tutor/course/category.php?id=716"`

Then run a quick smoke check in Python (do not commit this snippet, it's diagnostic):

```bash
python -c "
from pathlib import Path
from bot.scrapers.itsa import parse_contests
html = next(Path('bot/tests/fixtures').glob('itsa_live_*.html')).read_text()
contests = parse_contests(html)
print(f'parsed {len(contests)} contests from live ITSA')
for c in contests[:3]:
    print(f'  - {c.source_id}: {c.title} ({len(c.problems)} problems)')
"
```

Expected: prints at least one contest. If it prints zero, the live HTML's CSS classes differ from the synthetic fixture. Inspect the saved HTML (`less bot/tests/fixtures/itsa_live_*.html`), find the actual classes/structure used by ITSA, and update the selectors in `parse_contests` and `_parse_problems`. Then re-run both the synthetic unit test (must still pass — adjust the synthetic fixture if you changed the structure) and the smoke check.

Commit the live fixture file alongside the parser changes so future regressions can be caught.

- [ ] **Step 8: Commit**

```bash
git add bot/scrapers/itsa.py bot/scrapers/__init__.py bot/tests/test_itsa.py \
        bot/tests/fixtures/itsa_synthetic.html bot/tests/fixtures/itsa_live_*.html
git commit -m "feat(bot): add ITSA scraper with synthetic and live fixtures"
```

---

## Task 8: Orchestrator (`bot/main.py`)

**Files:**
- Create: `bot/main.py`
- Test: `bot/tests/test_main.py`

- [ ] **Step 1: Write failing tests**

Create `bot/tests/test_main.py`:

```python
from datetime import datetime
from pathlib import Path
from unittest.mock import MagicMock

import pytest

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


def test_run_creates_issue_for_new_contest_with_problems(tmp_path: Path):
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


def test_run_skips_already_tracked(tmp_path: Path):
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


def test_run_skips_contest_with_no_problems(tmp_path: Path):
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


def test_run_records_per_source_failure_and_continues(tmp_path: Path):
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


def test_run_does_not_persist_state_if_create_issue_raises(tmp_path: Path):
    state_path = tmp_path / "state.json"
    contest = make_contest(source_id="c4", problems=[Problem("A", "u")])

    def boom(**kw):
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
```

- [ ] **Step 2: Run to confirm failure**

Run: `pytest bot/tests/test_main.py -v`
Expected: FAIL with `ModuleNotFoundError: No module named 'bot.main'`

- [ ] **Step 3: Implement `bot/main.py`**

```python
from __future__ import annotations

import logging
import os
import sys
from dataclasses import dataclass, field
from datetime import datetime, timezone
from pathlib import Path
from typing import Callable, Iterable

import httpx

from .github_issue import create_issue as default_create_issue
from .github_issue import render_issue_body, render_issue_title
from .models import State, TrackedEntry
from .scrapers import SCRAPERS, Scraper
from .state import load_state, save_state

log = logging.getLogger("contest-tracker")

DEFAULT_STATE_PATH = Path("data/tracked_contests.json")


CreateIssueFn = Callable[..., int]


@dataclass
class Summary:
    new_issues: list[tuple[str, int]] = field(default_factory=list)
    failures: list[tuple[str, str]] = field(default_factory=list)


def run(
    *,
    scrapers: Iterable[Scraper],
    state_path: Path,
    create_issue: CreateIssueFn,
    repo: str,
    token: str,
    now_iso: str,
) -> Summary:
    summary = Summary()
    state = load_state(state_path)

    for scraper in scrapers:
        try:
            contests = scraper.fetch()
        except Exception as e:                       # noqa: BLE001
            log.error("[%s] fetch failed: %s", scraper.source, e)
            summary.failures.append((scraper.source, str(e)))
            continue

        for contest in contests:
            if contest.key in state.entries:
                continue
            if not contest.problems:
                continue
            title = render_issue_title(contest)
            body = render_issue_body(contest, now_iso=now_iso)
            labels = [f"source:{contest.source}", "auto-tracked"]
            try:
                issue_number = create_issue(
                    repo=repo, token=token, title=title, body=body, labels=labels,
                )
            except Exception as e:                   # noqa: BLE001
                log.error("[%s] create_issue failed for %s: %s",
                          contest.source, contest.key, e)
                summary.failures.append((contest.source, str(e)))
                continue
            state.entries[contest.key] = TrackedEntry(issue_number=issue_number,
                                                     tracked_at=now_iso)
            summary.new_issues.append((contest.key, issue_number))

    save_state(state_path, state)
    return summary


def _real_create_issue(*, repo: str, token: str, title: str, body: str,
                       labels: list[str]) -> int:
    with httpx.Client(base_url="https://api.github.com") as client:
        return default_create_issue(
            client=client, repo=repo, token=token, title=title, body=body, labels=labels,
        )


def main() -> int:
    logging.basicConfig(level=logging.INFO, format="%(asctime)s %(levelname)s %(message)s")
    repo = os.environ["GITHUB_REPOSITORY"]
    token = os.environ["GITHUB_TOKEN"]
    now_iso = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    summary = run(
        scrapers=SCRAPERS,
        state_path=DEFAULT_STATE_PATH,
        create_issue=_real_create_issue,
        repo=repo,
        token=token,
        now_iso=now_iso,
    )
    log.info("new issues: %s", summary.new_issues)
    log.info("failures:   %s", summary.failures)
    if summary.failures and len(summary.failures) >= len(list(SCRAPERS)):
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
```

- [ ] **Step 4: Run tests**

Run: `pytest bot/tests/ -v`
Expected: all tests pass (tests from Tasks 2–8).

- [ ] **Step 5: Commit**

```bash
git add bot/main.py bot/tests/test_main.py
git commit -m "feat(bot): orchestrate scrape → dedup → create-issue → persist"
```

---

## Task 9: GitHub Actions workflow + bootstrap commit

**Files:**
- Create: `.github/workflows/contest-tracker.yml`
- Create: `.github/workflows/lint.yml`

- [ ] **Step 1: Create the main workflow**

Create `.github/workflows/contest-tracker.yml`:

```yaml
name: contest-tracker

on:
  schedule:
    - cron: "0 2 * * *"        # 02:00 UTC = 10:00 Asia/Taipei
  workflow_dispatch: {}

permissions:
  issues: write
  contents: write

jobs:
  track:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-python@v5
        with:
          python-version: "3.12"
      - name: Install bot
        run: pip install -e bot/
      - name: Run tracker
        run: python -m bot.main
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
          GITHUB_REPOSITORY: ${{ github.repository }}
      - name: Commit state changes
        uses: stefanzweifel/git-auto-commit-action@v5
        with:
          commit_message: "bot: update tracked_contests.json"
          file_pattern: "data/tracked_contests.json"
          commit_user_name: "contest-tracker-bot"
          commit_user_email: "bot@users.noreply.github.com"
```

- [ ] **Step 2: Create the lint workflow**

Create `.github/workflows/lint.yml`:

```yaml
name: lint

on:
  pull_request:
    paths:
      - "bot/**"
      - ".github/workflows/lint.yml"

jobs:
  checks:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-python@v5
        with:
          python-version: "3.12"
      - run: pip install -e bot/[dev]
      - run: ruff check bot/
      - run: mypy --strict bot/
      - run: pytest bot/ -v
```

- [ ] **Step 3: Verify YAML syntax locally**

Run: `python -c "import yaml; yaml.safe_load(open('.github/workflows/contest-tracker.yml')); yaml.safe_load(open('.github/workflows/lint.yml')); print('ok')"`
Expected: prints `ok`. (PyYAML ships with most Python distros; if not, `pip install pyyaml`.)

- [ ] **Step 4: Run the full local test suite one more time**

Run: `pytest bot/ -v && ruff check bot/ && mypy --strict bot/`
Expected: all green.

- [ ] **Step 5: Commit and push to a feature branch for first deployment**

```bash
git checkout -b feat/contest-tracker-bot
git add .github/workflows/contest-tracker.yml .github/workflows/lint.yml
git commit -m "ci: schedule contest-tracker daily and lint PRs"
git push -u origin feat/contest-tracker-bot
```

Open a PR titled "Contest tracker bot (ITSA only)". Wait for the lint workflow to pass on the PR. After merge, manually trigger the workflow once from the Actions tab (`Run workflow` button) to validate the end-to-end path on `main`. Verify that at least one issue is created and that `data/tracked_contests.json` gets updated by the auto-commit step.

---

## Task 10: Deployment validation checkpoint

**No files changed** — this is a hold point. Do not start Task 11 until:

- [ ] **Step 1: Confirm the manual workflow_dispatch run finished successfully** on `main`. The Actions logs should show a summary line like `new issues: [('itsa:itsa-2026-XX', NN)]` and `failures: []`.

- [ ] **Step 2: Inspect a created issue.** Open the issue in the GitHub UI. Confirm:
  - Title looks like `[itsa] ...`
  - Labels `source:itsa` and `auto-tracked` are present
  - Problem table renders with markdown links and a difficulty/tag column

- [ ] **Step 3: Confirm `data/tracked_contests.json` was committed back to `main`** by `contest-tracker-bot` and the `entries` map now contains at least one ITSA key.

- [ ] **Step 4: If anything is off, fix it on a new feature branch before continuing.** Do not pile additional scrapers on top of a broken pipeline.

---

## Task 11: TIOJ scraper

> **Real-world HTML note:** Same procedure as ITSA — synthetic unit fixture first, then capture a live snapshot from `https://tioj.ck.tp.edu.tw/contests` and the per-contest page at `/contests/<id>`, and adjust selectors. TIOJ's listing is typically a `<table class="table">` of contests with rows linking to `/contests/<id>`. Each contest detail page has a problems table with rows linking to `/problems/<id>`.

**Files:**
- Create: `bot/scrapers/tioj.py`
- Create: `bot/tests/fixtures/tioj_synthetic_listing.html`
- Create: `bot/tests/fixtures/tioj_synthetic_contest.html`
- Modify: `bot/scrapers/__init__.py`
- Test: `bot/tests/test_tioj.py`

- [ ] **Step 1: Create synthetic listing fixture**

Create `bot/tests/fixtures/tioj_synthetic_listing.html`:

```html
<!DOCTYPE html><html><body>
<table class="table">
  <tr><th>名稱</th><th>開始</th><th>結束</th></tr>
  <tr>
    <td><a href="/contests/98">全國賽考古題</a></td>
    <td>2026-05-01 10:00:00 +0800</td>
    <td>2026-05-01 13:00:00 +0800</td>
  </tr>
  <tr>
    <td><a href="/contests/99">TOI模考古題</a></td>
    <td>2026-06-01 10:00:00 +0800</td>
    <td>2026-06-01 13:00:00 +0800</td>
  </tr>
</table>
</body></html>
```

- [ ] **Step 2: Create synthetic contest-detail fixture**

Create `bot/tests/fixtures/tioj_synthetic_contest.html`:

```html
<!DOCTYPE html><html><body>
<h1>全國賽考古題</h1>
<table class="problem-list">
  <tr><th>#</th><th>題目</th></tr>
  <tr><td>1</td><td><a href="/problems/1234">A. 找零錢</a></td></tr>
  <tr><td>2</td><td><a href="/problems/1235">B. 最短路徑</a></td></tr>
</table>
</body></html>
```

- [ ] **Step 3: Write failing tests**

Create `bot/tests/test_tioj.py`:

```python
from datetime import datetime, timezone, timedelta
from pathlib import Path

from bot.scrapers.tioj import TiojScraper, parse_listing, parse_contest_detail

LISTING = Path(__file__).parent / "fixtures" / "tioj_synthetic_listing.html"
DETAIL = Path(__file__).parent / "fixtures" / "tioj_synthetic_contest.html"


def test_parse_listing_yields_two_contests():
    contests = parse_listing(LISTING.read_text(), base_url="https://tioj.ck.tp.edu.tw")
    assert len(contests) == 2
    c = contests[0]
    assert c.source == "tioj"
    assert c.source_id == "contest-98"
    assert c.title == "全國賽考古題"
    assert c.url == "https://tioj.ck.tp.edu.tw/contests/98"
    tz = timezone(timedelta(hours=8))
    assert c.start_time == datetime(2026, 5, 1, 10, 0, tzinfo=tz)
    assert c.end_time == datetime(2026, 5, 1, 13, 0, tzinfo=tz)
    # Detail not fetched at listing parse → problems empty until fetch step
    assert c.problems == []


def test_parse_contest_detail_yields_two_problems():
    problems = parse_contest_detail(DETAIL.read_text(), base_url="https://tioj.ck.tp.edu.tw")
    assert len(problems) == 2
    assert problems[0].title == "A. 找零錢"
    assert problems[0].url == "https://tioj.ck.tp.edu.tw/problems/1234"
    assert problems[0].difficulty is None
    assert problems[0].tags == ()


def test_scraper_source_attribute():
    assert TiojScraper().source == "tioj"


def test_scraper_registered():
    from bot.scrapers import SCRAPERS
    assert any(s.source == "tioj" for s in SCRAPERS)
```

- [ ] **Step 4: Run to confirm failure**

Run: `pytest bot/tests/test_tioj.py -v`
Expected: FAIL with `ModuleNotFoundError`.

- [ ] **Step 5: Implement `bot/scrapers/tioj.py`**

```python
from __future__ import annotations

import time
from datetime import datetime
from typing import Callable
from urllib.parse import urljoin

import httpx
from bs4 import BeautifulSoup

from ..models import Contest, Problem

USER_AGENT = (
    "contest-tracker-bot/1.0 "
    "(+https://github.com/danghoangnhan/CompetitiveProgramming)"
)
TIOJ_BASE = "https://tioj.ck.tp.edu.tw"
TIOJ_LISTING = f"{TIOJ_BASE}/contests"


def _parse_dt(text: str) -> datetime | None:
    text = text.strip()
    if not text:
        return None
    # Format observed: "2026-05-01 10:00:00 +0800"
    try:
        return datetime.strptime(text, "%Y-%m-%d %H:%M:%S %z")
    except ValueError:
        return None


def parse_listing(html: str, *, base_url: str = TIOJ_BASE) -> list[Contest]:
    soup = BeautifulSoup(html, "lxml")
    out: list[Contest] = []
    for row in soup.select("table.table tr"):
        a = row.select_one("td a[href^='/contests/']")
        if not a:
            continue
        href = str(a.get("href", "")).strip()
        contest_id = href.rsplit("/", 1)[-1]
        cells = row.find_all("td")
        if len(cells) < 3:
            continue
        out.append(Contest(
            source="tioj",
            source_id=f"contest-{contest_id}",
            title=a.get_text(strip=True),
            url=urljoin(base_url + "/", href.lstrip("/")),
            start_time=_parse_dt(cells[1].get_text()),
            end_time=_parse_dt(cells[2].get_text()),
            registration_url=None,
            problems=[],
        ))
    return out


def parse_contest_detail(html: str, *, base_url: str = TIOJ_BASE) -> list[Problem]:
    soup = BeautifulSoup(html, "lxml")
    problems: list[Problem] = []
    for row in soup.select("table.problem-list tr"):
        a = row.select_one("td a[href^='/problems/']")
        if not a:
            continue
        href = str(a.get("href", "")).strip()
        problems.append(Problem(
            title=a.get_text(strip=True),
            url=urljoin(base_url + "/", href.lstrip("/")),
        ))
    return problems


class TiojScraper:
    source = "tioj"

    def __init__(
        self,
        listing_url: str = TIOJ_LISTING,
        client_factory: Callable[[], httpx.Client] | None = None,
        sleep: Callable[[float], None] = time.sleep,
    ) -> None:
        self._listing_url = listing_url
        self._sleep = sleep
        self._client_factory = client_factory or (
            lambda: httpx.Client(headers={"User-Agent": USER_AGENT}, timeout=30.0)
        )

    def fetch(self) -> list[Contest]:
        with self._client_factory() as client:
            listing_html = self._get(client, self._listing_url)
            contests = parse_listing(listing_html, base_url=TIOJ_BASE)
            enriched: list[Contest] = []
            for c in contests:
                detail_html = self._get(client, c.url)
                problems = parse_contest_detail(detail_html, base_url=TIOJ_BASE)
                enriched.append(Contest(
                    source=c.source, source_id=c.source_id, title=c.title, url=c.url,
                    start_time=c.start_time, end_time=c.end_time,
                    registration_url=c.registration_url, problems=problems,
                ))
            return enriched

    def _get(self, client: httpx.Client, url: str) -> str:
        for attempt in range(1, 4):
            r = client.get(url)
            if 200 <= r.status_code < 300:
                return r.text
            if 400 <= r.status_code < 500:
                raise RuntimeError(f"tioj GET {url} failed: {r.status_code}")
            if attempt < 3:
                self._sleep(2.0 ** (attempt - 1))
        raise RuntimeError(f"tioj GET {url} failed after 3 attempts")
```

- [ ] **Step 6: Register the scraper**

Edit `bot/scrapers/__init__.py` so SCRAPERS includes both:

```python
from __future__ import annotations

from .base import Scraper
from .itsa import ItsaScraper
from .tioj import TiojScraper

SCRAPERS: list[Scraper] = [ItsaScraper(), TiojScraper()]

__all__ = ["Scraper", "SCRAPERS"]
```

- [ ] **Step 7: Run tests + lint + types**

Run: `pytest bot/ -v && ruff check bot/ && mypy --strict bot/`
Expected: all green.

- [ ] **Step 8: Capture live HTML and verify (manual)**

Run (one for listing, one for the most recent contest you see):

```bash
curl -A "contest-tracker-bot/1.0" -o "bot/tests/fixtures/tioj_live_listing_$(date -u +%Y-%m-%d).html" \
     "https://tioj.ck.tp.edu.tw/contests"
# pick a contest ID from the listing (replace 98) and fetch its detail:
curl -A "contest-tracker-bot/1.0" -o "bot/tests/fixtures/tioj_live_contest_$(date -u +%Y-%m-%d).html" \
     "https://tioj.ck.tp.edu.tw/contests/98"
```

Run the same diagnostic snippet pattern as Task 7 step 7, but importing `parse_listing` and `parse_contest_detail`. If the parsed result is empty, inspect the saved HTML, find the actual table classes used by TIOJ, and update both the parser and the synthetic fixture so the unit test still represents the real structure.

- [ ] **Step 9: Commit**

```bash
git add bot/scrapers/tioj.py bot/scrapers/__init__.py bot/tests/test_tioj.py \
        bot/tests/fixtures/tioj_synthetic_*.html bot/tests/fixtures/tioj_live_*.html
git commit -m "feat(bot): add TIOJ scraper (listing + per-contest detail)"
```

---

## Task 12: ZeroJudge scraper

> **Real-world HTML note:** ZeroJudge's contest listing lives at `https://zerojudge.tw/Contest`. Each contest has a URL like `/Contest/Detail/<id>`. Same fixture-first approach.

**Files:**
- Create: `bot/scrapers/zerojudge.py`
- Create: `bot/tests/fixtures/zerojudge_synthetic_listing.html`
- Create: `bot/tests/fixtures/zerojudge_synthetic_contest.html`
- Modify: `bot/scrapers/__init__.py`
- Test: `bot/tests/test_zerojudge.py`

- [ ] **Step 1: Create synthetic listing fixture**

Create `bot/tests/fixtures/zerojudge_synthetic_listing.html`:

```html
<!DOCTYPE html><html><body>
<div id="contests">
  <div class="contest-card" data-id="500">
    <a class="contest-link" href="/Contest/Detail/500">2026 校際模擬賽</a>
    <div class="time">
      <span class="start" data-iso="2026-05-15T14:00:00+08:00">2026/05/15 14:00</span>
      <span class="end" data-iso="2026-05-15T17:00:00+08:00">17:00</span>
    </div>
  </div>
  <div class="contest-card" data-id="501">
    <a class="contest-link" href="/Contest/Detail/501">2026 期末練習賽</a>
    <div class="time">
      <span class="start" data-iso="2026-06-15T14:00:00+08:00">2026/06/15 14:00</span>
      <span class="end" data-iso="2026-06-15T17:00:00+08:00">17:00</span>
    </div>
  </div>
</div>
</body></html>
```

- [ ] **Step 2: Create synthetic contest fixture**

Create `bot/tests/fixtures/zerojudge_synthetic_contest.html`:

```html
<!DOCTYPE html><html><body>
<h1>2026 校際模擬賽</h1>
<table class="problems">
  <tr><th>編號</th><th>題目</th></tr>
  <tr><td>a001</td><td><a href="/ShowProblem?problemid=a001">A. 哈囉世界</a></td></tr>
  <tr><td>a002</td><td><a href="/ShowProblem?problemid=a002">B. 質數判定</a></td></tr>
</table>
</body></html>
```

- [ ] **Step 3: Write failing tests**

Create `bot/tests/test_zerojudge.py`:

```python
from datetime import datetime, timezone, timedelta
from pathlib import Path

from bot.scrapers.zerojudge import (
    ZeroJudgeScraper,
    parse_listing,
    parse_contest_detail,
)

LISTING = Path(__file__).parent / "fixtures" / "zerojudge_synthetic_listing.html"
DETAIL = Path(__file__).parent / "fixtures" / "zerojudge_synthetic_contest.html"


def test_parse_listing_two_contests():
    contests = parse_listing(LISTING.read_text(), base_url="https://zerojudge.tw")
    assert len(contests) == 2
    c = contests[0]
    assert c.source == "zerojudge"
    assert c.source_id == "contest-500"
    assert c.title == "2026 校際模擬賽"
    assert c.url == "https://zerojudge.tw/Contest/Detail/500"
    tz = timezone(timedelta(hours=8))
    assert c.start_time == datetime(2026, 5, 15, 14, 0, tzinfo=tz)
    assert c.end_time == datetime(2026, 5, 15, 17, 0, tzinfo=tz)
    assert c.problems == []


def test_parse_contest_detail_two_problems():
    problems = parse_contest_detail(DETAIL.read_text(), base_url="https://zerojudge.tw")
    assert len(problems) == 2
    assert problems[0].title == "A. 哈囉世界"
    assert problems[0].url == "https://zerojudge.tw/ShowProblem?problemid=a001"


def test_scraper_source_and_registered():
    from bot.scrapers import SCRAPERS
    assert any(s.source == "zerojudge" for s in SCRAPERS)
    assert ZeroJudgeScraper().source == "zerojudge"
```

- [ ] **Step 4: Run to confirm failure**

Run: `pytest bot/tests/test_zerojudge.py -v`
Expected: FAIL with `ModuleNotFoundError`.

- [ ] **Step 5: Implement `bot/scrapers/zerojudge.py`**

```python
from __future__ import annotations

import time
from datetime import datetime
from typing import Callable
from urllib.parse import urljoin

import httpx
from bs4 import BeautifulSoup

from ..models import Contest, Problem

USER_AGENT = (
    "contest-tracker-bot/1.0 "
    "(+https://github.com/danghoangnhan/CompetitiveProgramming)"
)
ZEROJUDGE_BASE = "https://zerojudge.tw"
ZEROJUDGE_LISTING = f"{ZEROJUDGE_BASE}/Contest"


def _iso(text: str | None) -> datetime | None:
    if not text:
        return None
    try:
        return datetime.fromisoformat(text)
    except ValueError:
        return None


def parse_listing(html: str, *, base_url: str = ZEROJUDGE_BASE) -> list[Contest]:
    soup = BeautifulSoup(html, "lxml")
    out: list[Contest] = []
    for card in soup.select(".contest-card[data-id]"):
        contest_id = card.get("data-id")
        a = card.select_one("a.contest-link")
        if not contest_id or not a:
            continue
        href = str(a.get("href", "")).strip()
        start_span = card.select_one(".time .start")
        end_span = card.select_one(".time .end")
        out.append(Contest(
            source="zerojudge",
            source_id=f"contest-{contest_id}",
            title=a.get_text(strip=True),
            url=urljoin(base_url + "/", href.lstrip("/")),
            start_time=_iso(start_span.get("data-iso") if start_span else None),
            end_time=_iso(end_span.get("data-iso") if end_span else None),
            registration_url=None,
            problems=[],
        ))
    return out


def parse_contest_detail(html: str, *, base_url: str = ZEROJUDGE_BASE) -> list[Problem]:
    soup = BeautifulSoup(html, "lxml")
    problems: list[Problem] = []
    for row in soup.select("table.problems tr"):
        a = row.select_one("td a[href*='ShowProblem']")
        if not a:
            continue
        href = str(a.get("href", "")).strip()
        problems.append(Problem(
            title=a.get_text(strip=True),
            url=urljoin(base_url + "/", href.lstrip("/")),
        ))
    return problems


class ZeroJudgeScraper:
    source = "zerojudge"

    def __init__(
        self,
        listing_url: str = ZEROJUDGE_LISTING,
        client_factory: Callable[[], httpx.Client] | None = None,
        sleep: Callable[[float], None] = time.sleep,
    ) -> None:
        self._listing_url = listing_url
        self._sleep = sleep
        self._client_factory = client_factory or (
            lambda: httpx.Client(headers={"User-Agent": USER_AGENT}, timeout=30.0)
        )

    def fetch(self) -> list[Contest]:
        with self._client_factory() as client:
            listing_html = self._get(client, self._listing_url)
            contests = parse_listing(listing_html, base_url=ZEROJUDGE_BASE)
            enriched: list[Contest] = []
            for c in contests:
                detail_html = self._get(client, c.url)
                problems = parse_contest_detail(detail_html, base_url=ZEROJUDGE_BASE)
                enriched.append(Contest(
                    source=c.source, source_id=c.source_id, title=c.title, url=c.url,
                    start_time=c.start_time, end_time=c.end_time,
                    registration_url=c.registration_url, problems=problems,
                ))
            return enriched

    def _get(self, client: httpx.Client, url: str) -> str:
        for attempt in range(1, 4):
            r = client.get(url)
            if 200 <= r.status_code < 300:
                return r.text
            if 400 <= r.status_code < 500:
                raise RuntimeError(f"zerojudge GET {url} failed: {r.status_code}")
            if attempt < 3:
                self._sleep(2.0 ** (attempt - 1))
        raise RuntimeError(f"zerojudge GET {url} failed after 3 attempts")
```

- [ ] **Step 6: Register the scraper**

Edit `bot/scrapers/__init__.py`:

```python
from __future__ import annotations

from .base import Scraper
from .itsa import ItsaScraper
from .tioj import TiojScraper
from .zerojudge import ZeroJudgeScraper

SCRAPERS: list[Scraper] = [ItsaScraper(), TiojScraper(), ZeroJudgeScraper()]

__all__ = ["Scraper", "SCRAPERS"]
```

- [ ] **Step 7: Run full suite**

Run: `pytest bot/ -v && ruff check bot/ && mypy --strict bot/`
Expected: all green.

- [ ] **Step 8: Capture live HTML and verify (manual)**

Same procedure as Task 11 step 8 with `https://zerojudge.tw/Contest` and the detail URL for one current contest. Save under `bot/tests/fixtures/zerojudge_live_*.html` and update selectors if needed.

- [ ] **Step 9: Commit**

```bash
git add bot/scrapers/zerojudge.py bot/scrapers/__init__.py bot/tests/test_zerojudge.py \
        bot/tests/fixtures/zerojudge_synthetic_*.html bot/tests/fixtures/zerojudge_live_*.html
git commit -m "feat(bot): add ZeroJudge scraper"
```

---

## Task 13: TOI scraper

> **Real-world HTML note:** TOI announcements have changed hosts over the years. The engineer must confirm the current canonical URL (likely the TOI page on the official Taiwan Computer Olympiad site or the TIOJ TOI archive). If TOI's announcement page is essentially a list of static links to contest PDFs without a structured problem index, this scraper may yield contests with empty `problems` lists — the orchestrator will skip them until problems are published in a parsable form, which is the desired behaviour. The synthetic fixture below assumes a TOI page with both contest metadata and a problem list.

**Files:**
- Create: `bot/scrapers/toi.py`
- Create: `bot/tests/fixtures/toi_synthetic.html`
- Modify: `bot/scrapers/__init__.py`
- Test: `bot/tests/test_toi.py`

- [ ] **Step 1: Create synthetic fixture**

Create `bot/tests/fixtures/toi_synthetic.html`:

```html
<!DOCTYPE html><html><body>
<section class="toi-year" data-year="2026">
  <h2><a href="https://toi.example.tw/2026">TOI 2026 全國資訊學科能力競賽</a></h2>
  <div class="schedule">
    <time datetime="2026-04-15T09:00:00+08:00">2026-04-15 09:00</time>
    →
    <time datetime="2026-04-15T13:00:00+08:00">2026-04-15 13:00</time>
  </div>
  <ul class="problems">
    <li><a href="https://toi.example.tw/2026/p1">P1. 最大公因數</a></li>
    <li><a href="https://toi.example.tw/2026/p2">P2. 圖論</a></li>
    <li><a href="https://toi.example.tw/2026/p3">P3. 動態規劃</a></li>
  </ul>
</section>
<section class="toi-year" data-year="2025">
  <h2><a href="https://toi.example.tw/2025">TOI 2025 全國資訊學科能力競賽</a></h2>
  <!-- old contest with no problems list -->
</section>
</body></html>
```

- [ ] **Step 2: Write failing tests**

Create `bot/tests/test_toi.py`:

```python
from datetime import datetime, timezone, timedelta
from pathlib import Path

from bot.scrapers.toi import ToiScraper, parse_contests

FIXTURE = Path(__file__).parent / "fixtures" / "toi_synthetic.html"


def test_parse_contests_two_years():
    contests = parse_contests(FIXTURE.read_text())
    assert len(contests) == 2
    c2026 = next(c for c in contests if c.source_id == "toi-2026")
    assert c2026.title.startswith("TOI 2026")
    assert c2026.url == "https://toi.example.tw/2026"
    tz = timezone(timedelta(hours=8))
    assert c2026.start_time == datetime(2026, 4, 15, 9, 0, tzinfo=tz)
    assert c2026.end_time == datetime(2026, 4, 15, 13, 0, tzinfo=tz)
    assert [p.title for p in c2026.problems] == ["P1. 最大公因數", "P2. 圖論", "P3. 動態規劃"]


def test_parse_contests_skips_problemless_year():
    contests = parse_contests(FIXTURE.read_text())
    c2025 = next(c for c in contests if c.source_id == "toi-2025")
    assert c2025.problems == []


def test_scraper_source_and_registered():
    from bot.scrapers import SCRAPERS
    assert any(s.source == "toi" for s in SCRAPERS)
    assert ToiScraper().source == "toi"
```

- [ ] **Step 3: Run to confirm failure**

Run: `pytest bot/tests/test_toi.py -v`
Expected: FAIL with `ModuleNotFoundError`.

- [ ] **Step 4: Implement `bot/scrapers/toi.py`**

```python
from __future__ import annotations

import time
from datetime import datetime
from typing import Callable

import httpx
from bs4 import BeautifulSoup, Tag

from ..models import Contest, Problem

USER_AGENT = (
    "contest-tracker-bot/1.0 "
    "(+https://github.com/danghoangnhan/CompetitiveProgramming)"
)
# Engineer: confirm and replace before merging — see Task 13 note.
TOI_LISTING_URL = "https://toi.example.tw/archive"


def _iso(value: str | None) -> datetime | None:
    if not value:
        return None
    try:
        return datetime.fromisoformat(value)
    except ValueError:
        return None


def _problems(section: Tag) -> list[Problem]:
    out: list[Problem] = []
    for li in section.select("ul.problems > li"):
        a = li.select_one("a")
        if not a:
            continue
        out.append(Problem(
            title=a.get_text(strip=True),
            url=str(a.get("href", "")).strip(),
        ))
    return out


def parse_contests(html: str) -> list[Contest]:
    soup = BeautifulSoup(html, "lxml")
    out: list[Contest] = []
    for section in soup.select("section.toi-year[data-year]"):
        year = section.get("data-year")
        h2_a = section.select_one("h2 a")
        if not year or not h2_a:
            continue
        times = section.select("div.schedule time[datetime]")
        start_time = _iso(times[0].get("datetime")) if len(times) >= 1 else None
        end_time = _iso(times[1].get("datetime")) if len(times) >= 2 else None
        out.append(Contest(
            source="toi",
            source_id=f"toi-{year}",
            title=h2_a.get_text(strip=True),
            url=str(h2_a.get("href", "")).strip(),
            start_time=start_time,
            end_time=end_time,
            registration_url=None,
            problems=_problems(section),
        ))
    return out


class ToiScraper:
    source = "toi"

    def __init__(
        self,
        url: str = TOI_LISTING_URL,
        client_factory: Callable[[], httpx.Client] | None = None,
        sleep: Callable[[float], None] = time.sleep,
    ) -> None:
        self._url = url
        self._sleep = sleep
        self._client_factory = client_factory or (
            lambda: httpx.Client(headers={"User-Agent": USER_AGENT}, timeout=30.0)
        )

    def fetch(self) -> list[Contest]:
        with self._client_factory() as client:
            for attempt in range(1, 4):
                r = client.get(self._url)
                if 200 <= r.status_code < 300:
                    return parse_contests(r.text)
                if 400 <= r.status_code < 500:
                    raise RuntimeError(f"toi fetch failed: {r.status_code}")
                if attempt < 3:
                    self._sleep(2.0 ** (attempt - 1))
            raise RuntimeError(f"toi fetch failed after 3 attempts")
```

- [ ] **Step 5: Register the scraper**

Edit `bot/scrapers/__init__.py`:

```python
from __future__ import annotations

from .base import Scraper
from .itsa import ItsaScraper
from .tioj import TiojScraper
from .toi import ToiScraper
from .zerojudge import ZeroJudgeScraper

SCRAPERS: list[Scraper] = [
    ItsaScraper(),
    TiojScraper(),
    ZeroJudgeScraper(),
    ToiScraper(),
]

__all__ = ["Scraper", "SCRAPERS"]
```

- [ ] **Step 6: Run full suite**

Run: `pytest bot/ -v && ruff check bot/ && mypy --strict bot/`
Expected: all green.

- [ ] **Step 7: Identify and capture live HTML, update selectors (manual)**

Find the current canonical TOI announcements URL (ask the user if unsure — could be a TIOJ archive page, the Taiwan Computer Olympiad site, or a GitHub-hosted index). Update `TOI_LISTING_URL` to the real URL. Fetch it:

```bash
curl -A "contest-tracker-bot/1.0" -o "bot/tests/fixtures/toi_live_$(date -u +%Y-%m-%d).html" \
     "$TOI_REAL_URL"
```

Run a diagnostic Python snippet to confirm `parse_contests` returns at least one entry. If the page's structure doesn't match the synthetic fixture (very likely — TOI's actual page may use different markup), update `parse_contests` to match the real structure and update the synthetic fixture to mirror it (the synthetic fixture is meant to *represent* the real structure for offline tests).

- [ ] **Step 8: Commit**

```bash
git add bot/scrapers/toi.py bot/scrapers/__init__.py bot/tests/test_toi.py \
        bot/tests/fixtures/toi_synthetic.html bot/tests/fixtures/toi_live_*.html
git commit -m "feat(bot): add TOI scraper"
```

---

## Task 14: README quick-start

**Files:**
- Modify: `bot/README.md` (create if missing)

- [ ] **Step 1: Write the README**

Create `bot/README.md`:

```markdown
# contest-tracker-bot

Auto-tracks Taiwanese competitive programming contests as GitHub issues in this repository.

## What it does

A daily GitHub Actions workflow runs `python -m bot.main`, which:

1. Loads the tracking state from `data/tracked_contests.json`.
2. Asks each registered scraper (ITSA, TIOJ, ZeroJudge, TOI) for the list of contests
   currently visible on its site.
3. For each contest whose problem set is published and whose key is not yet in state,
   opens a GitHub issue with the contest metadata and per-problem table, and records
   the issue number in state.
4. Commits the updated state file back to `main` via `stefanzweifel/git-auto-commit-action`.

## Local development

```bash
python -m venv .venv && source .venv/bin/activate
pip install -e bot/[dev]
pytest bot/ -v
ruff check bot/
mypy --strict bot/
```

## Adding a new source

1. Create `bot/scrapers/<source>.py` with a class implementing the `Scraper` Protocol
   (`source: str` attribute and `fetch() -> list[Contest]` method).
2. Add an instance to `SCRAPERS` in `bot/scrapers/__init__.py`.
3. Add a test under `bot/tests/test_<source>.py` using a synthetic HTML fixture.
4. Capture a live HTML snapshot at `bot/tests/fixtures/<source>_live_<YYYY-MM-DD>.html`
   and update selectors if needed.

## Running manually

In the GitHub UI, go to *Actions* → *contest-tracker* → *Run workflow*.

## Troubleshooting

- **Workflow run is red with "scraper X failed":** open the run log to see which source
  failed and why. Most often this means the site changed its HTML — capture a fresh
  live fixture, diff against the previous one, update selectors.
- **No new issues opened but I expected one:** check that the contest's problem set is
  actually published. The bot intentionally skips contests with empty `problems` lists.
- **Issue created but data is wrong:** edit the issue manually; the bot will never
  overwrite an existing issue.
```

- [ ] **Step 2: Commit**

```bash
git add bot/README.md
git commit -m "docs(bot): add quick-start and troubleshooting README"
```

---

## Task 15: Final integration PR

**Files:**
- None modified — this is the merge and validation step.

- [ ] **Step 1: Push the branch and open the PR**

```bash
git push -u origin feat/contest-tracker-bot
gh pr create --title "Contest tracker bot: ITSA + TIOJ + ZeroJudge + TOI" \
             --body "$(cat <<'EOF'
## Summary
- Scaffolds a Python bot under `bot/` with models, JSON state, issue rendering, and a GitHub REST client
- Adds four scrapers (ITSA, TIOJ, ZeroJudge, TOI), each with a synthetic fixture and a live snapshot
- Schedules a daily GitHub Actions workflow that opens issues for newly published contests and commits the state file
- Adds a lint workflow that runs `ruff` + `mypy --strict` + `pytest` on PRs touching `bot/`

## Test plan
- [ ] Lint workflow green on this PR
- [ ] After merge, manual `workflow_dispatch` run on `main` completes with `failures: []`
- [ ] At least one new issue is opened with correct title, labels, problem table
- [ ] `data/tracked_contests.json` updated by `contest-tracker-bot` author
- [ ] Re-running the workflow opens zero new issues (dedup works)

🤖 Generated with [Claude Code](https://claude.com/claude-code)
EOF
)"
```

- [ ] **Step 2: Wait for lint workflow to pass on the PR.**

- [ ] **Step 3: Merge and trigger first run.**

After merge, in the GitHub UI go to *Actions* → *contest-tracker* → *Run workflow* on `main`.

- [ ] **Step 4: Verify the test plan items above all check out.**

- [ ] **Step 5: Done.** The daily cron will take over from this point. Future site-layout breakages will surface as red workflow runs; fix by capturing a new live fixture and updating the relevant parser.
