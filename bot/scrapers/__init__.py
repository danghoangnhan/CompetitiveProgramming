from __future__ import annotations

from .base import Scraper

# Concrete scrapers append themselves as they are added.
SCRAPERS: list[Scraper] = []

__all__ = ["Scraper", "SCRAPERS"]
