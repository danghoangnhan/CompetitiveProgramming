from datetime import datetime, timezone, timedelta
from pathlib import Path

from bot.scrapers.itsa import ItsaScraper, parse_contests

FIXTURE = Path(__file__).parent / "fixtures" / "itsa_synthetic.html"


def test_parse_contests_extracts_two_entries():
    contests = parse_contests(FIXTURE.read_text())
    assert len(contests) == 2


def test_parse_contests_first_has_problems_populated():
    contests = parse_contests(FIXTURE.read_text())
    c = contests[0]
    assert c.source == "itsa"
    assert c.source_id == "itsa-2026-05"
    assert c.title == "ITSA 2026-05 月賽"
    assert c.url == "https://e-tutor.itsa.org.tw/contest/itsa-2026-05"
    assert c.registration_url == "https://e-tutor.itsa.org.tw/contest/itsa-2026-05/register"
    tz = timezone(timedelta(hours=8))
    assert c.start_time == datetime(2026, 5, 20, 19, 0, tzinfo=tz)
    assert c.end_time == datetime(2026, 5, 20, 22, 0, tzinfo=tz)
    assert len(c.problems) == 2
    p = c.problems[0]
    assert p.title == "A. 兩數之和"
    assert p.url == "https://e-tutor.itsa.org.tw/problem/p1"
    assert p.difficulty == "★★☆"
    assert p.tags == ("math", "greedy")


def test_parse_contests_second_has_empty_problems():
    contests = parse_contests(FIXTURE.read_text())
    assert contests[1].source_id == "itsa-2026-06"
    assert contests[1].problems == []
    assert contests[1].registration_url is None


def test_scraper_source_attribute():
    assert ItsaScraper().source == "itsa"


def test_scraper_registered():
    from bot.scrapers import SCRAPERS
    assert any(s.source == "itsa" for s in SCRAPERS)
