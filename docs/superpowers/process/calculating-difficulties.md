# calculating-difficulties — run state

This file is **state, not method**. It says where the work stopped, what it cost,
and what must not be paid for twice. It never explains how to do a task.

## Read first, in this order

1. `docs/superpowers/specs/2026-09-04-difficulty-recognition-design.md` — why, and the shape.
2. `docs/superpowers/plans/2026-09-16-calculating-difficulties.md` — what to do, task by task.

Do not start work before both are read. Then return here for the current position.

## Where we are

Task 3 done. Next: Task 4 (tag-floors.md).

## Phase ledger

| Task | Phase | Status | Artifact | Verify it is really done | Cost | Date |
|---|---|---|---|---|---|---|
| 1 | A wiring | done | `docs/superpowers/process/calculating-difficulties.md` | this file exists and is committed | — | 2026-09-16 |
| 2 | A wiring | done | `skills/calculating-difficulties/calibration/fetch-corpus.py` | `python …/fetch-corpus.py` prints usage | 0 agents | 2026-09-16 |
| 3 | A wiring | done | `skills/calculating-difficulties/SKILL.md` | frontmatter `name:` matches the directory | 0 agents | 2026-09-16 |
| 4 | A wiring | not started | `references/tag-floors.md` | 8 technique rows | 0 agents | — |
| 5 | A wiring | not started | `references/anchors.md` (starter) | 8 rows, ratings from the API | 0 agents | — |
| 6 | A wiring | not started | `skills/creating-problems/SKILL.md` | `grep -c "Step 8a"` ≥ 3 | 0 agents | — |
| 7 | A wiring | not started | `skills/writing-editorials/SKILL.md` | `grep -c "difficulty.md"` ≥ 2 | 0 agents | — |
| 8 | A wiring | not started | `preference.yml` | `grep -c step8a_difficulty` = 1 | 0 agents | — |
| 9 | A wiring | not started | `README.md` | `grep -c calculating-difficulties` = 2 | 0 agents | — |
| 10 | A wiring | not started | smoke-test transcript | both branches produced a file | 0 agents | — |
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

(none yet)

## Before ending a session

1. Update **Where we are** and the ledger row you touched.
2. Commit. An uncommitted change makes every row in this file a lie.
3. If you stopped mid-task, say so in **Where we are** — name the step number you reached.
