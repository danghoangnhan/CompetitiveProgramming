from __future__ import annotations

from .base import Scraper
from .itsa import ItsaScraper
from .tioj import TiojScraper
from .toi import ToiScraper
from .zerojudge import ZeroJudgeScraper  # noqa: F401 -- kept for re-enablement with auth

# ZeroJudge is intentionally NOT in SCRAPERS. Its detail pages 302 to /Login
# so problems[] is always empty in unauthenticated mode. To re-enable,
# pass an authenticated client_factory to ZeroJudgeScraper() and append
# the instance to SCRAPERS.
SCRAPERS: list[Scraper] = [
    ItsaScraper(),
    TiojScraper(),
    ToiScraper(),
]

__all__ = ["Scraper", "SCRAPERS"]
