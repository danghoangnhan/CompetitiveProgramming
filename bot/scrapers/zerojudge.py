from __future__ import annotations

import re
import time
from datetime import datetime
from typing import Callable
from urllib.parse import urljoin
from zoneinfo import ZoneInfo

import httpx
from bs4 import BeautifulSoup

from ..models import Contest, Problem

USER_AGENT = (
    "contest-tracker-bot/1.0 "
    "(+https://github.com/danghoangnhan/CompetitiveProgramming)"
)
ZEROJUDGE_BASE = "https://zerojudge.tw"
ZEROJUDGE_LISTING = f"{ZEROJUDGE_BASE}/Contests"

TAIPEI = ZoneInfo("Asia/Taipei")

# Real ZeroJudge listing format: "YYYY-MM-DD HH:MM:SS.0"
_START_RE = re.compile(r"開始時間[：:]\s*(\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2})")


def _parse_start(text: str) -> datetime | None:
    m = _START_RE.search(text)
    if not m:
        return None
    try:
        return datetime.strptime(m.group(1).strip(), "%Y-%m-%d %H:%M:%S").replace(tzinfo=TAIPEI)
    except ValueError:
        return None


def parse_listing(html: str, *, base_url: str = ZEROJUDGE_BASE) -> list[Contest]:
    """Parse the ZeroJudge /Contests listing page.

    Real page structure (per live fixture 2026-05-14):
      Each contest is in a ``div.well.well-lg`` block containing:
        - ``<h1>`` — contest title (in ``div.col-md-8``)
        - ``button.doJoinContestByOnlineUser[data-contestid]`` — contest ID
        - ``a[href*="ShowContest"]`` — detail page link
        - ``div#contestInfo`` — text containing "開始時間：YYYY-MM-DD HH:MM:SS.0"
      No end time is available in the listing HTML.
    """
    soup = BeautifulSoup(html, "lxml")
    out: list[Contest] = []
    for well in soup.select("div.well.well-lg"):
        h1 = well.select_one("h1")
        if not h1:
            continue
        title = h1.get_text(strip=True)
        if not title:
            continue
        btn = well.select_one("button.doJoinContestByOnlineUser[data-contestid]")
        if not btn:
            continue
        contest_id = str(btn.get("data-contestid", "")).strip()
        if not contest_id:
            continue
        # URL: prefer the "ShowContest" link; fall back to constructing from ID
        a = well.select_one("a[href*='ShowContest']")
        if a:
            href = str(a.get("href", "")).strip().lstrip("./")
            url = urljoin(base_url + "/", href)
        else:
            url = f"{base_url}/ShowContest?contestid={contest_id}"
        # Start time from contestInfo text
        contest_info = well.select_one("div#contestInfo")
        start_time = _parse_start(contest_info.get_text()) if contest_info else None
        out.append(Contest(
            source="zerojudge",
            source_id=f"contest-{contest_id}",
            title=title,
            url=url,
            start_time=start_time,
            end_time=None,
            registration_url=None,
            problems=[],
        ))
    return out


def parse_contest_detail(html: str, *, base_url: str = ZEROJUDGE_BASE) -> list[Problem]:
    """Parse a ZeroJudge ShowContest detail page.

    Real page structure: a ``table.table`` with ``<a href="/ShowProblem?problemid=...">``
    links in the problem rows.
    """
    soup = BeautifulSoup(html, "lxml")
    problems: list[Problem] = []
    for a in soup.select("a[href*='ShowProblem']"):
        href = str(a.get("href", "")).strip()
        if href.startswith("/"):
            url = urljoin(base_url, href)
        else:
            url = urljoin(base_url + "/", href.lstrip("./"))
        problems.append(Problem(
            title=a.get_text(strip=True),
            url=url,
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
            lambda: httpx.Client(
                headers={"User-Agent": USER_AGENT},
                timeout=30.0,
                follow_redirects=False,
            )
        )

    def fetch(self) -> list[Contest]:
        with self._client_factory() as client:
            listing_html = self._get(client, self._listing_url)
            contests = parse_listing(listing_html, base_url=ZEROJUDGE_BASE)
            enriched: list[Contest] = []
            for c in contests:
                try:
                    detail_html = self._get(client, c.url)
                    problems = parse_contest_detail(detail_html, base_url=ZEROJUDGE_BASE)
                except RuntimeError:
                    # Detail page may require login; keep contest with empty problems list
                    problems = []
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
            if 300 <= r.status_code < 400 or 400 <= r.status_code < 500:
                raise RuntimeError(f"zerojudge GET {url} failed: {r.status_code}")
            if attempt < 3:
                self._sleep(2.0 ** (attempt - 1))
        raise RuntimeError(f"zerojudge GET {url} failed after 3 attempts")
