from datetime import datetime
from zoneinfo import ZoneInfo

from bot.github_issue import render_issue_body, render_issue_title
from bot.models import Contest, Problem


def make_contest(**overrides) -> Contest:
    tz = ZoneInfo("Asia/Taipei")
    defaults: dict = dict(
        source="itsa",
        source_id="itsa-2026-05",
        title="ITSA 2026-05 Monthly",
        url="https://itsa.org.tw/c/itsa-2026-05",
        start_time=datetime(2026, 5, 20, 19, 0, tzinfo=tz),
        end_time=datetime(2026, 5, 20, 22, 0, tzinfo=tz),
        registration_url="https://itsa.org.tw/c/itsa-2026-05/register",
        problems=[
            Problem(title="A. Sum", url="https://itsa.org.tw/p/1", difficulty="★★☆",
                    tags=("math",)),
            Problem(title="B. Tree DP", url="https://itsa.org.tw/p/2"),
        ],
    )
    defaults.update(overrides)
    return Contest(**defaults)


def test_title_includes_source_prefix():
    c = make_contest()
    assert render_issue_title(c) == "[itsa] ITSA 2026-05 Monthly"


def test_body_contains_contest_url_and_schedule():
    body = render_issue_body(make_contest(), now_iso="2026-05-14T02:00:00Z")
    assert "https://itsa.org.tw/c/itsa-2026-05" in body
    assert "2026-05-20 19:00" in body
    assert "Asia/Taipei" in body


def test_body_problem_table_links_each_problem():
    body = render_issue_body(make_contest(), now_iso="2026-05-14T02:00:00Z")
    assert "| A. Sum |" not in body, "should render as a markdown link, not plain text"
    assert "[A. Sum](https://itsa.org.tw/p/1)" in body
    assert "[B. Tree DP](https://itsa.org.tw/p/2)" in body


def test_body_omits_difficulty_and_tags_when_absent():
    body = render_issue_body(make_contest(), now_iso="2026-05-14T02:00:00Z")
    # B has no difficulty or tags — its row should show em-dashes
    assert "[B. Tree DP](https://itsa.org.tw/p/2)" in body
    assert "| — | — |" in body


def test_body_omits_registration_line_when_none():
    c = make_contest(registration_url=None)
    body = render_issue_body(c, now_iso="2026-05-14T02:00:00Z")
    assert "Registration" not in body


def test_body_omits_schedule_when_times_none():
    c = make_contest(start_time=None, end_time=None)
    body = render_issue_body(c, now_iso="2026-05-14T02:00:00Z")
    assert "Schedule" not in body
