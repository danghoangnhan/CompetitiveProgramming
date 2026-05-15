# Contest Tracker Bot — Design

**Status:** Draft for review
**Author:** Daniel Du
**Date:** 2026-05-14
**Repo:** `danghoangnhan/CompetitiveProgramming`

## 1. Goal

Auto-track newly published Taiwanese competitive programming contests by opening one GitHub issue per contest in this repository. Each issue contains the contest's metadata and the list of problems with direct links and difficulty/tags, so the repo doubles as a personal index of contests to attempt.

## 2. Scope

### In scope
- Scraping four Taiwanese sources on a daily schedule:
  - **ITSA** — `itsa.org.tw` (monthly programming contests)
  - **TIOJ** — `tioj.ck.tp.edu.tw` contest pages
  - **ZeroJudge** — `zerojudge.tw` contest pages
  - **TOI** — Taiwan Olympiad in Informatics official announcements

  TIOJ and ZeroJudge are kept as separate scraper modules because they have different HTML layouts despite being grouped together in the user's source-selection.
- Creating one GitHub issue per newly discovered contest *once its problem set is published*.
- Deduplication via a committed state file (`data/tracked_contests.json`).
- Running entirely inside GitHub Actions using the built-in `GITHUB_TOKEN` — no external services, no GitHub App registration, no PAT.

### Out of scope
- Tracking non-Taiwanese contests (Codeforces, AtCoder, etc.).
- Embedding full problem statements in issues (only title, link, difficulty, tags).
- Auto-solving, auto-submitting, or any interaction with the contest sites beyond read-only HTTP fetches.
- Editing or closing issues after creation. The bot only **creates**; the user owns the issue lifecycle thereafter.
- Notifications (email, Slack, etc.). The user reads issues via the GitHub UI.

## 3. Source confirmation

A web search performed during brainstorming confirmed that none of ITSA, TIOJ, or ZeroJudge expose RSS/Atom feeds suitable for problem-level detail. Therefore custom HTML scraping is required. Marketplace-action-only approaches were considered and rejected for that reason.

## 4. Repository layout

```
.github/
  workflows/
    contest-tracker.yml            # daily cron + manual dispatch
bot/
  __init__.py
  main.py                          # entrypoint: orchestrates run
  models.py                        # Contest, Problem, TrackedEntry dataclasses
  state.py                         # read/write data/tracked_contests.json
  github_issue.py                  # render issue body, call REST API
  scrapers/
    __init__.py                    # registry of enabled scrapers
    base.py                        # Scraper protocol
    itsa.py
    tioj.py
    zerojudge.py
    toi.py
  tests/
    fixtures/                      # captured HTML snapshots per source
      itsa_2026-04.html
      tioj_contest_98.html
      zerojudge_contest_2026-05.html
      toi_2026.html
    test_itsa.py
    test_tioj.py
    test_zerojudge.py
    test_toi.py
    test_state.py
    test_issue_render.py
    test_main_dedup.py
  pyproject.toml                   # deps: httpx, beautifulsoup4, lxml, pytest, ruff, mypy
data/
  tracked_contests.json            # bot-managed state, committed each run
```

The bot lives in `bot/` so it's a clear sibling to the solution folders (`ITSA/`, `PUPC/`, `TOPC-2020/`, `自辦賽/`) and does not get confused with contest work. State is in `data/` so it's visible but separate from solutions.

## 5. Data model

```python
# bot/models.py
from dataclasses import dataclass, field
from datetime import datetime

@dataclass(frozen=True)
class Problem:
    title: str                          # "Problem A — Sum of Two Numbers"
    url: str                            # direct link to the problem page
    difficulty: str | None = None       # e.g. "Easy" / "★★☆"
    tags: tuple[str, ...] = ()          # e.g. ("greedy", "math")

@dataclass(frozen=True)
class Contest:
    source: str                         # "itsa" | "tioj" | "zerojudge" | "toi"
    source_id: str                      # source-stable ID, e.g. "itsa-2026-05"
    title: str                          # human-readable title
    url: str                            # contest page URL
    start_time: datetime | None         # Asia/Taipei aware
    end_time: datetime | None           # Asia/Taipei aware
    registration_url: str | None
    problems: list[Problem] = field(default_factory=list)

    @property
    def key(self) -> str:
        return f"{self.source}:{self.source_id}"   # globally unique

@dataclass(frozen=True)
class TrackedEntry:
    issue_number: int
    tracked_at: str                     # ISO-8601 UTC timestamp
```

`data/tracked_contests.json` schema:

```json
{
  "version": 1,
  "entries": {
    "itsa:itsa-2026-05": { "issue_number": 42, "tracked_at": "2026-05-14T02:00:13Z" },
    "tioj:contest-98":   { "issue_number": 43, "tracked_at": "2026-05-14T02:00:14Z" }
  }
}
```

The `version` field allows future schema migrations. Deleting an entry causes the bot to re-open that contest's issue on the next run.

## 6. Scraper contract

```python
# bot/scrapers/base.py
from typing import Protocol
from ..models import Contest

class Scraper(Protocol):
    source: str                         # short identifier, matches Contest.source

    def fetch(self) -> list[Contest]:
        """Return all contests the scraper can currently see.

        Each contest's ``problems`` list MUST be empty if the problem set is
        not yet published; the orchestrator uses that to decide whether to
        open an issue yet."""
```

Each scraper is responsible for:
1. Issuing HTTP requests with a polite `User-Agent` (`contest-tracker-bot/1.0 (+https://github.com/danghoangnhan/CompetitiveProgramming)`).
2. Parsing the contest listing page and, for each contest, optionally drilling into the contest detail page to enumerate problems.
3. Returning a `Contest` with a stable `source_id` derived from the source's own ID (URL slug, numeric ID, etc.) — not from the title, which can change.

Retry policy: 3 attempts with exponential backoff (1 s, 2 s, 4 s) for any 5xx or connection error. 4xx is fatal for that source on that run.

## 7. Workflow definition

`.github/workflows/contest-tracker.yml`:

```yaml
name: contest-tracker

on:
  schedule:
    - cron: "0 2 * * *"           # 02:00 UTC daily = 10:00 Asia/Taipei
  workflow_dispatch: {}            # manual trigger for ad-hoc runs

permissions:
  issues: write                    # to open/edit issues
  contents: write                  # to commit tracked_contests.json

jobs:
  track:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-python@v5
        with: { python-version: "3.12" }
      - run: pip install -e bot/
      - run: python -m bot.main
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
          GITHUB_REPOSITORY: ${{ github.repository }}
      - uses: stefanzweifel/git-auto-commit-action@v5
        with:
          commit_message: "bot: update tracked_contests.json"
          file_pattern: "data/tracked_contests.json"
          commit_user_name: "contest-tracker-bot"
          commit_user_email: "bot@users.noreply.github.com"
```

A separate `lint.yml` workflow runs `ruff` + `mypy --strict` + `pytest` on pull requests touching `bot/` — kept off the daily cron path to avoid blocking the tracker on lint regressions.

## 8. Orchestrator logic

```
# bot/main.py (pseudocode)
1. state = state.load("data/tracked_contests.json")
2. new_issues = []
3. failures = []
4. for scraper in registered_scrapers:
       try:
           contests = scraper.fetch()
       except Exception as e:
           failures.append((scraper.source, e))
           log_error(scraper.source, e)
           continue
       for c in contests:
           if c.key in state.entries:           # already tracked
               continue
           if not c.problems:                    # problem set not yet published
               continue
           body  = issue.render(c)
           title = issue.title_for(c)
           issue_number = github.create_issue(
               title=title,
               body=body,
               labels=[f"source:{c.source}", "auto-tracked"],
           )
           state.entries[c.key] = TrackedEntry(issue_number, utc_now_iso())
           new_issues.append((c.key, issue_number))
5. state.save("data/tracked_contests.json")
6. print_summary(new_issues, failures)
7. if failures and len(failures) == len(registered_scrapers):
       sys.exit(1)         # every source failed — surface as red workflow run
```

Key properties:
- **One source's failure does not block others.** Per-source try/except.
- **State is only mutated after a successful API call.** If GitHub returns an error creating an issue, the contest stays unseen and will be retried next run.
- **`problems` empty ⇒ skip for now.** Announcements without published problem sets are intentionally deferred until problems are available.

## 9. Issue body template

Title format: `[{source}] {contest_title}` (e.g. `[itsa] ITSA 2026-05 Monthly Contest`).

Body:

```markdown
# {contest_title}

**Source:** `{source}` • [Contest page]({contest_url})
**Schedule:** {start_time} → {end_time} (Asia/Taipei)
**Registration:** [link]({registration_url})    ← line omitted if registration_url is None

## Problems

| # | Problem | Difficulty | Tags |
|---|---------|-----------|------|
| A | [Sum of Two]({problem_url}) | ★★☆ | math, greedy |
| B | [Tree DP]({problem_url}) | ★★★ | tree, dp |
| … |

---
*Auto-tracked by `contest-tracker` on {iso_timestamp}. Edit freely — the bot only creates, it does not overwrite.*
```

Labels applied: `source:itsa` | `source:tioj` | `source:zerojudge` | `source:toi` plus `auto-tracked`. GitHub auto-creates labels referenced in `POST /repos/{owner}/{repo}/issues` if they do not yet exist, so no separate label-bootstrap step is needed.

## 10. Error handling

| Failure mode | Behaviour |
|---|---|
| Single scraper raises (network, parse, anything) | Logged with source and exception. Other scrapers continue. Workflow run stays green unless **all** sources fail. |
| GitHub API returns non-2xx on `create_issue` | Retry twice with 2 s and 4 s backoff. If still failing, leave the contest out of state so the next run retries. Log a warning. |
| `tracked_contests.json` missing | Treated as empty state. The first run will (re-)create issues for every contest the scrapers see — acceptable on initial bootstrap. |
| `tracked_contests.json` malformed | Abort the run with exit 1 *before* any API call. Do not silently overwrite, since that risks losing the issue-number mapping. |
| Auto-commit step fails (e.g. branch protection) | Workflow run goes red. State updates are lost; on the next run the bot will try to create the same issues again, which is wasted work — so this needs an alert. **Mitigation:** the workflow runs on `main`, with branch protection configured to allow the `GITHUB_TOKEN` actor to push state-only commits, OR the bot's commits go through a `bot/` prefix branch and the auto-commit action targets that. Decision: push directly to `main`; this is a personal repo with no required reviews. |

## 11. Testing strategy

- **Unit tests per scraper** load a captured HTML fixture from `bot/tests/fixtures/` and assert the parser returns the expected `Contest`. No network calls in CI.
- **State round-trip test** writes, reloads, and diffs the JSON to catch schema drift.
- **Issue render snapshot test** asserts the markdown body for a known `Contest` matches a checked-in expected string. Catches accidental template changes.
- **End-to-end dedup test** uses a fake scraper and a stub GitHub client to verify: (a) new contest creates issue and updates state; (b) already-tracked contest is skipped; (c) contest with empty `problems` is skipped; (d) GitHub API failure leaves state unchanged.
- **Linting:** `ruff` + `mypy --strict`, enforced in `lint.yml` on PRs.

When a source changes layout, the relevant parser test fails. Fix: capture a fresh HTML fixture from the live site, update the parser, re-run tests.

## 12. Dependencies

```toml
# bot/pyproject.toml
[build-system]
requires = ["hatchling"]
build-backend = "hatchling.build"

[project]
name = "contest-tracker-bot"
version = "0.1.0"
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
```

The `[build-system]` table is required for `pip install -e bot/` to work; `hatchling` is the lightweight default.

`PyGithub` was considered for the GitHub API call but rejected — there's exactly one endpoint we hit (`POST /repos/{owner}/{repo}/issues`), and `httpx` handles it in ~15 lines without an extra dependency.

## 13. Operational notes

- **Schedule:** 02:00 UTC daily = 10:00 Asia/Taipei. Adjustable in `contest-tracker.yml`.
- **Manual runs:** `workflow_dispatch` is enabled, so a fresh sync can be triggered any time from the Actions tab.
- **Permissions:** `issues: write` and `contents: write` only. No org-level or external secrets.
- **Cost:** Public repo on free GitHub Actions tier; each run takes <30 s, well within free quota.
- **Privacy:** Workflow logs are public on a public repo. Scrapers must not log raw HTML or anything that could include personal data.

## 14. Future work (explicitly deferred)

- Edit existing issues to reflect updated contest metadata (currently bot only creates).
- Slack / email / push notifications on new issues.
- Additional Taiwanese sources (e.g. NPSC, school OJs) — add a new scraper module under `bot/scrapers/`.
- A GitHub Project board view auto-populated from issue labels.
- Migration to a long-lived state store (database) if scaling demands it. Current JSON-in-repo is fine for personal scale.

## 15. Open questions

None at design time. Any new ambiguity surfaced during implementation should be raised as a comment on the implementation plan rather than silently resolved.
