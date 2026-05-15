from __future__ import annotations

import time
from datetime import datetime
from typing import Callable
from zoneinfo import ZoneInfo

import httpx
from bs4 import BeautifulSoup, Tag

from ..models import Contest, Problem

USER_AGENT = (
    "contest-tracker-bot/1.0 "
    "(+https://github.com/danghoangnhan/CompetitiveProgramming)"
)
# Engineer: confirm and replace before merging — see Task 13 note.
TOI_LISTING_URL = "https://toi.example.tw/archive"
TAIPEI = ZoneInfo("Asia/Taipei")


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
        start_raw = times[0].get("datetime") if len(times) >= 1 else None
        end_raw = times[1].get("datetime") if len(times) >= 2 else None
        out.append(Contest(
            source="toi",
            source_id=f"toi-{year}",
            title=h2_a.get_text(strip=True),
            url=str(h2_a.get("href", "")).strip(),
            start_time=_iso(str(start_raw) if start_raw is not None else None),
            end_time=_iso(str(end_raw) if end_raw is not None else None),
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
                try:
                    r = client.get(self._url)
                except httpx.RequestError:
                    if attempt < 3:
                        self._sleep(2.0 ** (attempt - 1))
                        continue
                    return []   # placeholder URL or network failure — no contests today
                if 200 <= r.status_code < 300:
                    return parse_contests(r.text)
                if 400 <= r.status_code < 500:
                    return []   # placeholder URL likely 404s — degrade gracefully
                if attempt < 3:
                    self._sleep(2.0 ** (attempt - 1))
            return []
