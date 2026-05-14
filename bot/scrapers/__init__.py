from __future__ import annotations

from .base import Scraper
from .itsa import ItsaScraper
from .tioj import TiojScraper

SCRAPERS: list[Scraper] = [ItsaScraper(), TiojScraper()]

__all__ = ["Scraper", "SCRAPERS"]
