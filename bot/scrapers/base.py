from __future__ import annotations

from typing import Protocol

from ..models import Contest


class Scraper(Protocol):
    """A read-only contest source.

    ``fetch()`` returns every contest the scraper can currently see. A contest with an empty
    ``problems`` list is treated as "not yet published" by the orchestrator and is silently
    skipped (no issue is created) until problems show up.
    """

    source: str

    def fetch(self) -> list[Contest]: ...
