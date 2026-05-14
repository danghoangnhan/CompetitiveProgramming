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
from .models import TrackedEntry
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
