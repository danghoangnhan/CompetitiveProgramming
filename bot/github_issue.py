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
