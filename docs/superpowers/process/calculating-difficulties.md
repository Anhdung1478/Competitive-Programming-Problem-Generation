# calculating-difficulties — run state

This file is **state, not method**. It says where the work stopped, what it cost,
and what must not be paid for twice. It never explains how to do a task.

## Read first, in this order

1. `docs/superpowers/specs/2026-09-04-difficulty-recognition-design.md` — why, and the shape.
2. `docs/superpowers/plans/2026-09-16-calculating-difficulties.md` — what to do, task by task.

Do not start work before both are read. Then return here for the current position.

## Where we are

Second generation complete. Frozen 2026-09-19 on branch
`feat/calculating-difficulties-200`: MAE 279, bias −29, 58% within ±200, n=24,
baseline 658, **171 anchors**. That is round 4's configuration — the 171-anchor table
against the original 8-row floor vocabulary. Because MAE is at or below 300, the skill
now emits a `± 300` interval instead of the range it shipped at 329.

MAE ≤ 200 and ≥65% within ±200 are still missed. |bias| ≤ 75 is met for the first time,
and the status section says plainly that −29 is cancellation between an over-rated low
end and an under-rated high end, not accuracy.

Round 5 (two added floor rows, 13 anchors re-tagged off `none`) measured MAE 275 and was
**reverted** — see the deviations below. Both reference files are back to their round-4
state; every round stays recorded in `metrics.md`.

The branch is deliberately **unmerged and unpushed** at the user's instruction.

## Phase ledger

| Task | Phase | Status | Artifact | Verify it is really done | Cost | Date |
|---|---|---|---|---|---|---|
| 1 | A wiring | done | `docs/superpowers/process/calculating-difficulties.md` | this file exists and is committed | — | 2026-09-16 |
| 2 | A wiring | done | `skills/calculating-difficulties/calibration/fetch-corpus.py` | `python …/fetch-corpus.py` prints usage | 0 agents | 2026-09-16 |
| 3 | A wiring | done | `skills/calculating-difficulties/SKILL.md` | frontmatter `name:` matches the directory | 0 agents | 2026-09-16 |
| 4 | A wiring | done | `references/tag-floors.md` | 8 technique rows | 0 agents | 2026-09-16 |
| 5 | A wiring | done | `references/anchors.md` (starter) | 8 rows, ratings from the API | 0 agents | 2026-09-16 |
| 6 | A wiring | done | `skills/creating-problems/SKILL.md` | `grep -c "Step 8a"` ≥ 3 | 0 agents | 2026-09-16 |
| 7 | A wiring | done | `skills/writing-editorials/SKILL.md` | `grep -c "difficulty.md"` ≥ 2 | 0 agents | 2026-09-16 |
| 8 | A wiring | done | `preference.yml` | `grep -c step8a_difficulty` = 1 | 0 agents | 2026-09-16 |
| 9 | A wiring | done | `README.md` | `grep -c calculating-difficulties` = 2 | 0 agents | 2026-09-16 |
| 10 | A wiring | done | smoke-test transcript (scratchpad, not committed) | both branches produced a file | 0 agents | 2026-09-16 |
| 10b | A wiring | done | `SKILL.md` five tie-breaks closed | `grep -c "below the Pass B floor"` = 1 | 0 agents | 2026-09-16 |
| 11 | B corpus | done | `calibration/corpus.md` + `.cache-cf-corpus/` | 80 rows, 79 `.txt` files (1181C is PDF-only) | 0 agents, ~4 min curl | 2026-09-16 |
| 12 | B corpus | done | `calibration/eval-set.md`, blind copies | `check` prints `ALL CHECKS PASSED` | 0 agents | 2026-09-16 |
| 13 | B corpus | done | `references/anchors.md` (real, 55) | 55 rows, `check` still passes | 6 agents | 2026-09-16 |
| 14 | C calibrate | done | `calibration/predictions-baseline.md` | 24 rows | 5 agents | 2026-09-16 |
| 15 | C calibrate | done | `calibration/predictions-round1.md` | 24 rows | 5 agents | 2026-09-16 |
| 16 | C calibrate | done | `calibration/metrics.md` | baseline + round 1 sections | 0 agents | 2026-09-16 |
| 17 | C calibrate | done | `metrics.md` rounds 2-3 | ≤ 3 rounds total | 10 agents, 2 rounds | 2026-09-16 |
| 18 | C calibrate | done | frozen `SKILL.md` | no `Uncalibrated` string remains | 0 agents | 2026-09-16 |

### Generation 2 — corpus extended to 200 (plan `2026-09-19-calculating-difficulties-extend-200.md`)

| Task | Phase | Status | Artifact | Verify it is really done | Cost | Date |
|---|---|---|---|---|---|---|
| 1 | A tooling | done | `calibration/fetch-corpus.py` | `extend` verb exists; `check` asserts the 200-row shape | 0 agents | 2026-09-19 |
| 2 | B corpus | done | `calibration/corpus.md` (200 rows) | `check` prints `anchors: 175   eval: 24   excluded: 1` | 0 agents | 2026-09-19 |
| 3 | B corpus | done | 12 batch files (workspace, not the repo) | 116 of 120 rows summarised; 4 ids failed to fetch | 12 agents | 2026-09-19 |
| 4 | B corpus | done | `references/anchors.md` (171) | `grep -c '^\| [0-9]'` = 171, `check` still passes | 0 agents | 2026-09-19 |
| 5 | C calibrate | done | `predictions-round4.md`, `metrics.md` round 4 | 24 rows; MAE 279 recorded | 5 agents | 2026-09-19 |
| 6 | C calibrate | done | `predictions-round5.md`, `metrics.md` round 5 | 24 rows; MAE 275 recorded, then reverted | 5 agents | 2026-09-19 |
| 7 | D freeze | done | frozen `SKILL.md`, this file | `grep -c "calibration/"` = 0 in `SKILL.md` and `references/` | 0 agents | 2026-09-19 |

**Files on disk beat this table.** If a row says `done` and the artifact is not there, the
artifact wins — correct the row and redo the task. A resume protocol that trusts its own
bookkeeping is how expensive work gets silently repeated or silently skipped.

## Never re-run without a reason

- **`sample` (Task 11).** The Codeforces problemset grows, so a second run picks a
  different 80 problems and invalidates every measurement taken against the first.
  `corpus.md` is committed; it *is* the frozen sample. The script refuses a re-run.
  Override it only to deliberately start a new calibration generation.
- **`split` (Task 12).** Same reasoning — it refuses once roles are assigned. Re-splitting
  moves problems between the anchor and eval sets, which retroactively contaminates every
  round already measured.
- **Summarization (Task 13).** 6 agents over 55 statements. `references/anchors.md` having
  55 rows means it is done. Re-run only if the anchor id set itself changed.
- **Each eval round (Tasks 14, 15, 17).** ~5 agents each. Every round writes its own
  `predictions-round<N>.md` and *appends* a section to `metrics.md` — never overwrite one.
  The comparison between rounds is the only evidence that tuning helped.
- **Statement fetching (Task 11)** is cheap — curl only, no model tokens — but
  `.cache-cf-corpus/` is gitignored, so a fresh clone must re-run `fetch`. That is expected.
  It is also why the *summaries* live in committed files and the raw statements do not.

## Deviations from the plan

- **Known contradiction in `SKILL.md`, unfixed as of Task 10.** Pass B calls the prerequisite
  floor "the level below which this problem cannot land, whatever the code looks like", but
  Pass E's gates are only *round to 100* and *clamp to `[800, 3500]`* — nothing re-clamps to
  the floor, so Pass D's adjustments can undercut it. The Task 10 smoke run hit this live:
  Pass C placed the problem at 1900, Pass D totalled `-200`, and the naive result 1700 sat
  below the 1900 digit-DP floor. The run treated the floor as binding and emitted 1900.
  This is not an edge case — it reopens the exact failure Pass B exists to close, and
  digit-DP problems disproportionately draw both negative adjustments (textbook exercise,
  constraint leakage). **Fix before Task 14**: running baseline predictions against this
  contradiction lets each eval agent invent its own tie-break, which corrupts the very
  measurements Phase C exists to produce. **Closed by Task 10b.**
- **Task 10's Branch 1 was not run blind — since settled by a re-run.** The original agent
  read the expected floor before running the skill. A fresh agent then re-ran Branch 1 with
  `docs/`, `.superpowers/`, the earlier run's output and git history all fenced off, against
  an isolated fixture. It reached the same floor (1900) and the same final rating (1900)
  unaided, so the claim holds: a cold reader of `SKILL.md` lands on the right floor.

- **Four further under-specified points in `SKILL.md`, found by that blind run.** None was
  fatal to a single run; all four are sources of *between-agent variance*, which is what
  Phase C measures. Fix them together with the floor gap above, before Task 14:
  1. **Pass C's "clearly easier" anchor slot is unsatisfiable when the placement sits at the
     floor** — and the documented fallback widens the window *upward* only, so it cannot
     produce an easier anchor. Textbook cases sit at the floor by construction, so this
     fires often rather than rarely.
  2. **`-100 to -200` (textbook exercise) gives no rule for choosing within the range.**
  3. **"Constraint leakage" is undefined at its boundary** — readable from the constraints
     alone, or from constraints plus legend? The two runs split on exactly this question.
  4. **Counting "independent non-obvious observations" is underdetermined** when sub-steps
     are genuinely separable but individually routine; the stated rule ("two restatements of
     one idea count once") only covers duplicate framings.

  Evidence that these are real and not hypothetical: the two runs reached the same `-200`
  Pass D total by *different routes* — one applied `-100` textbook plus `-100` constraint
  leakage, the other `-200` textbook with leakage explicitly rejected. They tied by
  coincidence. With ~5 agents per eval round, that is noise recorded as measurement. **Closed by Task 10b.**

- **Tuning ran the full 3-round cap and still missed the targets.** Round 1 (rubric v0)
  brought MAE from a 658 baseline down to 408. Round 2 tried shifting every technique floor
  down 200 to remove signed bias; it made both MAE (475) and bias (+250) worse and was
  reverted in round 3, which instead fixed Pass C's anchor placement and kept the original
  floors. Round 3 landed at MAE 329, bias +179, 50% within ±200 — the plan's hard cap of 3
  rounds was reached before MAE ≤ 200 or |bias| ≤ 75 was hit. Because measured MAE (329) is
  above the plan's 300 threshold for emitting a `±` interval, the frozen skill reports a
  range (`<placement−300>-<placement+300>`) instead of a `±` interval, per Task 18.

- **Task 12: `1181C` excluded from both roles.** Codeforces serves its statement as a
  native PDF (`%PDF-1.5`, no `problem-statement` div), not HTML, so it cannot be fetched or
  summarized like the other 79 sampled problems. It was excluded via an `EXCLUDED` constant
  in `fetch-corpus.py` rather than dropped from `corpus.md` — the frozen row set is
  untouched; only its `role` cell reads `excluded`. The split is 55 anchor / 24 eval / 1
  excluded (not the planned 56/24), and the `1900-2099` band draws its 3 eval from 9
  candidates instead of 10.

### Generation 2 (2026-09-19)

- **Codeforces now serves a Cloudflare Managed Challenge to `curl`.** Generation 1 fetched
  all 79 statements with plain `curl`. On 2026-09-19 every id — new and previously cached
  alike — returned HTTP 403 with a JS challenge page, from `curl`, from the harness's own
  fetcher, and from the public mirrors. Re-polling did not clear it. The 120 new statements
  were fetched through the Exa MCP fetcher instead, by the Task 3 summariser agents, which
  does reach clean statement text. `fetch-corpus.py`'s `fetch` verb was **not** weakened to
  work around this and no assertion was relaxed; the fetcher changed, the checks did not.
  Expect a future session to hit the same wall and need a fetcher that solves the challenge.

- **Four anchor ids could not be fetched at all: `1016C`, `1163B2`, `1322C`, `1638B`.** Not
  PDF-only — the Exa fetcher returned `CRAWL_UNKNOWN_ERROR` / `CRAWL_LIVECRAWL_TIMEOUT` for
  these four across 5-10 attempts each, both URL forms and the `?locale=` variants, and two
  further controller re-probes afterwards. They keep
  `role = anchor` in `corpus.md` and are absent from `references/anchors.md`, which is why
  the anchor table holds **171 rows against 175 corpus anchors**. They were deliberately not
  added to `EXCLUDED`: `EXCLUDED` means "Codeforces does not serve this as HTML", a permanent
  property of the problem, while these four are a transient fetch failure that a later run
  may resolve. `check`'s label-integrity pass only walks rows *present in* `anchors.md`, so
  the gap passes cleanly and is recorded here rather than hidden in a constant.

- **Round 4 beat the shipped baseline: MAE 329 → 279.** Same 24 eval slots, same `SKILL.md`,
  same 8-row `tag-floors.md`; the only thing that changed was the anchor table growing from
  55 rows to 171. Bias moved from +179 to −29 and within ±200 from 50% to 58%. A 50-point
  MAE move on n=24 is just outside the ±40 standard error, so it is weak evidence of a real
  improvement and was ruled to ship on that basis.

- **Round 5 is a null result and was reverted.** Round 5 added two data-derived rows to
  `tag-floors.md` (factorization/gcd/modular-periodicity and interactive-reconstruction, both
  floored at 1500) and re-tagged 13 anchors off `none`. It measured MAE 275, bias +8, 54%
  within ±200 — a 4-point MAE difference against round 4 on n=24, which is evidence of
  nothing, and *worse* on within ±200. It also produced a demonstrated defect: `eval-22`
  (true rating 1400) picked up the new interactive row and floored at **1500**, above the
  problem's own true rating. A floor that sits above the true rating disqualifies the
  construct, whose only job is to be a lower bound. Both reference files were restored to
  their round-4 content with `git checkout bb2dddc --`. **Round 5 stays recorded** in
  `metrics.md` and `predictions-round5.md`: a negative result that is deleted is not a result.
  What this costs if wrong: the vocabulary question is left open for another generation, and
  any real gain those two rows carried is forgone.

## Next generation — the high end is a Pass C window problem

Not started. This section records the finding, not a plan.

**The finding.** Two generations of tuning have not fixed high-end under-rating, and the
round-4 data says why: it is a Pass C **window** problem, not a Pass B vocabulary problem.
Of the nine `none`-tagged anchors rated 2300 or above, **six have no floorable prerequisite
at all** — no row in any vocabulary tried so far describes what they require. The three that
do have one floor at only **1500** (they are the number-theory and interactive-reconstruction
rows round 5 added). Pass C then searches `[floor, floor+600]`, so the widest window those
three can open is `[1500, 2100]` — it cannot reach a 2300 problem even in principle, and for
the other six the window starts at `1100` and tops out around `1700`. A richer vocabulary
does not close that gap: round 5 added exactly those rows and measured nothing.

**The next lever is the `[floor, floor+600]` placement rule in Pass C**, not `tag-floors.md`.
Whatever replaces it — a wider window, a window that scales with the floor, a two-stage
placement that re-anchors after a first pass — it is a change to `SKILL.md`'s Pass C and it
must be measured the same way: one variable, same 24 slots, appended to `metrics.md`.

**A larger eval set costs comparability.** Six rounds are now recorded against the same 24
problems (658 / 408 / 475 / 329 / 279 / 275). Changing `n` makes every one of those numbers
incomparable with the new one, which throws away the only evidence that any tuning helped. It
is still the right thing to do eventually — after six rounds of selection those 24 are no
longer strictly held out and the shipped 279 is optimistic by an unmeasured amount — but it
needs a **fresh generation with its own baseline**, not a patch to this one.

**Four ids are owed a re-fetch.** `1016C`, `1163B2`, `1322C` and `1638B` are `role = anchor`
in `corpus.md` and missing from `anchors.md`. A session that gets past the Cloudflare
challenge should summarise them and bring the anchor table to 175.

**Never, under any circumstances:** re-run `sample`, re-run `split`, or edit an existing row
of `corpus.md`. Those would re-select or re-assign the frozen rows and silently invalidate
every measurement in `metrics.md`. Any further extension is append-only.

## Before ending a session

1. Update **Where we are** and the ledger row you touched.
2. Commit. An uncommitted change makes every row in this file a lie.
3. If you stopped mid-task, say so in **Where we are** — name the step number you reached.
