from datetime import datetime, timezone, timedelta
from pathlib import Path

from bot.scrapers.toi import ToiScraper, parse_contests

FIXTURE = Path(__file__).parent / "fixtures" / "toi_synthetic.html"


def test_parse_contests_two_years():
    contests = parse_contests(FIXTURE.read_text())
    assert len(contests) == 2
    c2026 = next(c for c in contests if c.source_id == "toi-2026")
    assert c2026.title.startswith("TOI 2026")
    assert c2026.url == "https://toi.example.tw/2026"
    tz = timezone(timedelta(hours=8))
    assert c2026.start_time == datetime(2026, 4, 15, 9, 0, tzinfo=tz)
    assert c2026.end_time == datetime(2026, 4, 15, 13, 0, tzinfo=tz)
    assert [p.title for p in c2026.problems] == ["P1. 最大公因數", "P2. 圖論", "P3. 動態規劃"]


def test_parse_contests_skips_problemless_year():
    contests = parse_contests(FIXTURE.read_text())
    c2025 = next(c for c in contests if c.source_id == "toi-2025")
    assert c2025.problems == []


def test_scraper_source_and_registered():
    from bot.scrapers import SCRAPERS
    assert any(s.source == "toi" for s in SCRAPERS)
    assert ToiScraper().source == "toi"
