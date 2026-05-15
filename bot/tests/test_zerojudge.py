from datetime import datetime
from pathlib import Path
from zoneinfo import ZoneInfo

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
    assert c.url == "https://zerojudge.tw/ShowContest?contestid=500"
    # ZeroJudge times are in Asia/Taipei; scraper tags them accordingly
    assert c.start_time == datetime(2026, 5, 15, 14, 0, 0, tzinfo=ZoneInfo("Asia/Taipei"))
    # No end time available in listing
    assert c.end_time is None
    assert c.problems == []


def test_parse_contest_detail_two_problems():
    problems = parse_contest_detail(DETAIL.read_text(), base_url="https://zerojudge.tw")
    assert len(problems) == 2
    assert problems[0].title == "A. 哈囉世界"
    assert problems[0].url == "https://zerojudge.tw/ShowProblem?problemid=a001"


def test_scraper_disabled_by_default_but_importable():
    from bot.scrapers import SCRAPERS
    # Intentionally NOT in default SCRAPERS — detail pages require login.
    assert not any(s.source == "zerojudge" for s in SCRAPERS)
    # But the class can still be instantiated for users who want to provide auth.
    assert ZeroJudgeScraper().source == "zerojudge"
