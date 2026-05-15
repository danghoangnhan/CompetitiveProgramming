from __future__ import annotations

from .base import Scraper
from .itsa import ItsaScraper
from .tioj import TiojScraper
from .toi import ToiScraper
from .zerojudge import ZeroJudgeScraper

SCRAPERS: list[Scraper] = [
    ItsaScraper(),
    TiojScraper(),
    ZeroJudgeScraper(),
    ToiScraper(),
]

__all__ = ["Scraper", "SCRAPERS"]
