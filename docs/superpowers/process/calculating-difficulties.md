# calculating-difficulties — run state

This file is **state, not method**. It says where the work stopped, what it cost,
and what must not be paid for twice. It never explains how to do a task.

## Read first, in this order

1. `docs/superpowers/specs/2026-09-04-difficulty-recognition-design.md` — why, and the shape.
2. `docs/superpowers/plans/2026-09-16-calculating-difficulties.md` — what to do, task by task.

Do not start work before both are read. Then return here for the current position.

## Where we are

Task 10 done — Phase A complete. Next: Task 11 (freeze the 80-problem sample), pending user go-ahead.

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
| 11 | B corpus | not started | `calibration/corpus.md` + `.cache-cf-corpus/` | 80 rows, 80 `.txt` files | 0 agents, ~4 min curl | — |
| 12 | B corpus | not started | `calibration/eval-set.md`, blind copies | `check` prints `ALL CHECKS PASSED` | 0 agents | — |
| 13 | B corpus | not started | `references/anchors.md` (real, 56) | 56 rows, `check` still passes | ~6 agents | — |
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
  measurements Phase C exists to produce.
- **Task 10's Branch 1 was not run blind.** The agent read the brief's expected floor before
  running the skill. Branch 2 (the `not estimable` gate) is unaffected — a file-existence
  check cannot be swayed by foreknowledge — and Branch 1's floor lookup, anchor window,
  caps and arithmetic were each re-verified against the reference files afterwards. But the
  claim Task 10 exists to prove, that a cold reader of `SKILL.md` reaches the right floor
  unaided, is not demonstrated. A fresh-agent re-run of Branch 1 alone would settle it.

## Before ending a session

1. Update **Where we are** and the ledger row you touched.
2. Commit. An uncommitted change makes every row in this file a lie.
3. If you stopped mid-task, say so in **Where we are** — name the step number you reached.
