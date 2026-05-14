from __future__ import annotations

from .base import Scraper
from .itsa import ItsaScraper

SCRAPERS: list[Scraper] = [ItsaScraper()]

__all__ = ["Scraper", "SCRAPERS"]
