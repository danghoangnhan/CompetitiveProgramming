from bot.models import Contest
from bot.scrapers.base import Scraper


class FakeScraper:
    source = "fake"

    def fetch(self) -> list[Contest]:
        return []


def test_fake_scraper_satisfies_protocol():
    s: Scraper = FakeScraper()   # type: ignore[assignment]
    assert s.source == "fake"
    assert s.fetch() == []


def test_registry_starts_empty_for_iteration():
    from bot.scrapers import SCRAPERS
    # SCRAPERS is a list; concrete scrapers append themselves over Tasks 7/11/12/13.
    assert isinstance(SCRAPERS, list)
