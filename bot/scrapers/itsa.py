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
        start_raw = time_span.get("data-start") if time_span else None
        end_raw = time_span.get("data-end") if time_span else None
        start_time = _parse_iso(str(start_raw)) if start_raw is not None else None
        end_time = _parse_iso(str(end_raw)) if end_raw is not None else None

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
