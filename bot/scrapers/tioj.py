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
