from __future__ import annotations

import time
from datetime import datetime
from typing import Callable, Iterable
from zoneinfo import ZoneInfo

import httpx

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
