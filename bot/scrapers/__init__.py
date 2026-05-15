from __future__ import annotations

from .base import Scraper
from .itsa import ItsaScraper
from .tioj import TiojScraper
from .zerojudge import ZeroJudgeScraper

SCRAPERS: list[Scraper] = [ItsaScraper(), TiojScraper(), ZeroJudgeScraper()]

__all__ = ["Scraper", "SCRAPERS"]
