# contest-tracker-bot

Auto-tracks Taiwanese competitive programming contests as GitHub issues in this repository.

## What it does

A daily GitHub Actions workflow runs `python -m bot.main`, which:

1. Loads the tracking state from `data/tracked_contests.json`.
2. Asks each registered scraper (ITSA, TIOJ, TOI) for the list of contests
   currently visible on its site.
3. For each contest whose problem set is published and whose key is not yet in state,
   opens a GitHub issue with the contest metadata and per-problem table, and records
   the issue number in state.
4. Commits the updated state file back to `main` via `stefanzweifel/git-auto-commit-action`.

## Source-specific notes

| Source | Status |
|---|---|
| ITSA | Synthetic-fixture parser; selectors may need updating against the live `e-tutor.itsa.org.tw` HTML — capture a live snapshot and adjust if no contests parse. |
| TIOJ | Parser aligned with live HTML as of 2026-05-14; tracks the `/contests` listing and drills into `/contests/<id>/problems/<pid>` for each contest. |
| ZeroJudge | **Disabled by default** — detail pages require login, so problems list is always empty. To enable, provide an authenticated `client_factory` and append `ZeroJudgeScraper(client_factory=...)` to `SCRAPERS` in `bot/scrapers/__init__.py`. |
| TOI | Points at the canonical site `https://tpmso.org/toi/` (official 資訊奧林匹亞競賽 site, confirmed live 2026-05-15). The site is a WordPress/Elementor blog without machine-readable contest listings, so the scraper degrades gracefully to `[]`. Update `parse_contests` in `bot/scrapers/toi.py` if tpmso.org ever adds structured contest markup. |

## Local development

```bash
python3 -m venv .venv && . .venv/bin/activate
pip install -e "bot/[dev]"
pytest bot/ -v
ruff check bot/
mypy --strict bot/
```

## Adding a new source

1. Create `bot/scrapers/<source>.py` with a class implementing the `Scraper` Protocol
   (`source: str` attribute and `fetch() -> list[Contest]` method).
2. Add an instance to `SCRAPERS` in `bot/scrapers/__init__.py`.
3. Add a test under `bot/tests/test_<source>.py` using a synthetic HTML fixture.
4. Capture a live HTML snapshot at `bot/tests/fixtures/<source>_live_<YYYY-MM-DD>.html`
   and update selectors if needed.

## Running manually

In the GitHub UI, go to *Actions* → *contest-tracker* → *Run workflow*.

## Troubleshooting

- **Workflow run is red with "scraper X failed":** open the run log to see which source
  failed and why. Most often this means the site changed its HTML — capture a fresh
  live fixture, diff against the previous one, update selectors.
- **No new issues opened but I expected one:** check that the contest's problem set is
  actually published. The bot intentionally skips contests with empty `problems` lists.
- **Issue created but data is wrong:** edit the issue manually; the bot will never
  overwrite an existing issue.
- **ZeroJudge contests are never tracked:** intentional — see "Source-specific notes".
  Re-enable by adding `ZeroJudgeScraper()` (with auth) back to `SCRAPERS`.
- **TOI scraper returns nothing:** The official TOI site (`https://tpmso.org/toi/`) is
  a WordPress/Elementor blog with no machine-readable contest listings, so the scraper
  returns `[]` by design. If tpmso.org adds structured markup, update `parse_contests`
  in `bot/scrapers/toi.py` to match the new HTML structure.
