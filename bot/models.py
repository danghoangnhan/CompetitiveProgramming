from __future__ import annotations

from dataclasses import dataclass, field
from datetime import datetime
from typing import Any


@dataclass(frozen=True)
class Problem:
    title: str
    url: str
    difficulty: str | None = None
    tags: tuple[str, ...] = ()


@dataclass(frozen=True)
class Contest:
    source: str
    source_id: str
    title: str
    url: str
    start_time: datetime | None
    end_time: datetime | None
    registration_url: str | None
    problems: list[Problem] = field(default_factory=list)

    @property
    def key(self) -> str:
        return f"{self.source}:{self.source_id}"


@dataclass(frozen=True)
class TrackedEntry:
    issue_number: int
    tracked_at: str

    def to_dict(self) -> dict[str, Any]:
        return {"issue_number": self.issue_number, "tracked_at": self.tracked_at}

    @classmethod
    def from_dict(cls, d: dict[str, Any]) -> "TrackedEntry":
        return cls(issue_number=int(d["issue_number"]), tracked_at=str(d["tracked_at"]))


@dataclass
class State:
    version: int
    entries: dict[str, TrackedEntry]

    SUPPORTED_VERSION = 1

    def to_dict(self) -> dict[str, Any]:
        return {
            "version": self.version,
            "entries": {k: v.to_dict() for k, v in self.entries.items()},
        }

    @classmethod
    def from_dict(cls, d: dict[str, Any]) -> "State":
        version = int(d.get("version", 0))
        if version != cls.SUPPORTED_VERSION:
            raise ValueError(f"unsupported state version: {version}")
        entries = {k: TrackedEntry.from_dict(v) for k, v in d.get("entries", {}).items()}
        return cls(version=version, entries=entries)
