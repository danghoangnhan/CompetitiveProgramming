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
