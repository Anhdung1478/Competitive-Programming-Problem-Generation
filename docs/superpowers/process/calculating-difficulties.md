# calculating-difficulties — run state

This file is **state, not method**. It says where the work stopped, what it cost,
and what must not be paid for twice. It never explains how to do a task.

## Read first, in this order

1. `docs/superpowers/specs/2026-09-04-difficulty-recognition-design.md` — why, and the shape.
2. `docs/superpowers/plans/2026-09-16-calculating-difficulties.md` — what to do, task by task.

Do not start work before both are read. Then return here for the current position.

## Where we are

Task 14 in progress — metrics verb added, baseline predictions running.

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
| 14 | C calibrate | not started | `calibration/predictions-baseline.md` | 24 rows | ~5 agents | — |
| 15 | C calibrate | not started | `calibration/predictions-round1.md` | 24 rows | ~5 agents | — |
| 16 | C calibrate | not started | `calibration/metrics.md` | baseline + round 1 sections | 0 agents | — |
| 17 | C calibrate | not started | `metrics.md` rounds 2-3 | ≤ 3 rounds total | ~5 agents/round | — |
| 18 | C calibrate | not started | frozen `SKILL.md` | no `Uncalibrated` string remains | 0 agents | — |

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
- **Summarization (Task 13).** ~6 agents over 56 statements. `references/anchors.md` having
  56 rows means it is done. Re-run only if the anchor id set itself changed.
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

- **Task 12: `1181C` excluded from both roles.** Codeforces serves its statement as a
  native PDF (`%PDF-1.5`, no `problem-statement` div), not HTML, so it cannot be fetched or
  summarized like the other 79 sampled problems. It was excluded via an `EXCLUDED` constant
  in `fetch-corpus.py` rather than dropped from `corpus.md` — the frozen row set is
  untouched; only its `role` cell reads `excluded`. The split is 55 anchor / 24 eval / 1
  excluded (not the planned 56/24), and the `1900-2099` band draws its 3 eval from 9
  candidates instead of 10.

## Before ending a session

1. Update **Where we are** and the ledger row you touched.
2. Commit. An uncommitted change makes every row in this file a lie.
3. If you stopped mid-task, say so in **Where we are** — name the step number you reached.
