# `calculating-difficulties` — extend the corpus to 200

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development to implement this plan task-by-task.

**Goal:** grow the frozen 80-problem calibration corpus to 200 by appending 120 new anchors, re-measure blind on the *same* 24 eval problems as round 4, rule on the `tag-floors.md` vocabulary question with its own separately measured round 5, and freeze whichever configuration the measurement supports.

**Spec:** `docs/superpowers/process/calculating-difficulties-extend-200.md` (the session prompt — it is the binding authority for this plan).

**Prior generation:** spec `docs/superpowers/specs/2026-09-04-difficulty-recognition-design.md`, plan `docs/superpowers/plans/2026-09-16-calculating-difficulties.md`, run state `docs/superpowers/process/calculating-difficulties.md`.

**Architecture:** one build-time tool (`calibration/fetch-corpus.py`) owns every mechanical step. The extension is **append-only**: the frozen 80 rows of `corpus.md` are never edited, `sample` and `split` are never re-run. Code first (Task 1), data second (Tasks 2-4), measurement last (Tasks 5-6), freeze at the end (Task 7).

---

## Global Constraints

- Every path is relative to the repository root, `E:/Project/plugins/Competitive-Programming-Problem-Generation`.
- **Append-only.** NEVER re-run `sample`. NEVER re-run `split`. NEVER edit an existing row of `calibration/corpus.md`. Both verbs already refuse by design; do not work around a refusal.
- **The eval set is frozen** at the same 24 problems, same slots, same true ratings. A different `n` produces an MAE that cannot be compared with the recorded 658 / 408 / 475 / 329.
- Final corpus state: **200 rows = 175 anchor + 24 eval + 1 excluded**, 25 per band across the 8 bands `1100-1299 … 2500-2699`, 3 eval per band.
- `1181C` stays excluded — Codeforces serves its statement as a native PDF. Any *new* id that serves a PDF instead of HTML is excluded the same way: by adding it to the `EXCLUDED` constant, **never** by editing or deleting a `corpus.md` row.
- **`SKILL.md` must never name `calibration/`**, and neither must any file under `references/`. `grep -c "calibration/"` over both must stay `0`. The runtime half cannot see the build-time half or the blind split is fake.
- Eval agents get the blind statement text and the runtime references only — never a true rating, never any path under `calibration/`.
- **`SKILL.md` and `references/tag-floors.md` are unchanged for round 4.** Round 4 varies exactly one thing: the size of the anchor table. Round 5 varies exactly one further thing: the `tag-floors.md` vocabulary.
- Anchor row format, unchanged: `| id | rating | div | prereq | one sentence |`, `prereq` copied verbatim from the left column of `tag-floors.md` or the literal `none`, sentence under 25 words naming the actual mechanism, `  <!-- unsure -->` appended when the summary cannot be reconciled with the true rating.
- **±40 is the standard error** of a 24-problem eval. A movement smaller than that is not an improvement and must not be described as one.
- Every round APPENDS to `calibration/metrics.md`. Never overwrite a round.
- `yagni-principles` applies to every file this plan touches.
- Commit after each task.

## Pre-registered decision rules

Recorded **before** any round-4 number exists, so that selecting a winner afterwards is not a free parameter.

**Ship rule.** Let `m4` be round 4's MAE and `m5` round 5's; `m3 = 329` is the shipped baseline.

| Condition | What ships | What `## Calibration status` says |
|---|---|---|
| best of `m4`/`m5` ≤ `289` (`m3 − 40`) | that round's configuration | that round's measured numbers, and that it beat 329 by more than the error bar |
| best within `(289, 369)` of 329 | the 175-anchor table (round 4 config), or round 5's if `m5 < m4` | the winning round's measured numbers, stating plainly that the change did **not** move accuracy beyond the ±40 error bar |
| both ≥ `369` (`m3 + 40`) | revert `references/anchors.md` to its 55-row round-3 content; the corpus extension and the 120 summaries stay committed as build-time data | 329 stays the shipped figure; the worse rounds are recorded in `metrics.md` and named in the status section as a negative result |

**Selection honesty.** Whatever ships, the status section must disclose that the figure is the best of several rounds measured on the same 24 problems and is therefore optimistic, and that after five rounds of selection the eval set is no longer strictly held out.

**Vocabulary rule (Task 6).** The `tag-floors.md` vocabulary question is answered by measurement, not argument: round 5 is round 4's anchor table plus an extended vocabulary, run on the same 24 slots. Round 4 must be scored and recorded *before* round 5's vocabulary edit is made, or neither number attributes to anything.

---

### Task 1: `extend` verb and the widened `check`

**Files:**
- Modify: `skills/calculating-difficulties/calibration/fetch-corpus.py`

**Interfaces:**
- Consumes: `corpus.md` (80 rows, roles assigned), the live Codeforces API through the existing `candidates()`.
- Produces: the `extend` verb Task 2 runs, and a `cmd_check` that asserts the 200-row shape Task 4 must satisfy.

- [ ] **Step 1: The check that must fail now**

```bash
python skills/calculating-difficulties/calibration/fetch-corpus.py extend
```
Expected now: the usage line and a non-zero exit — the verb does not exist.

- [ ] **Step 2: Constants**

Set `PER_BAND = 25` (it now means *problems per band in the frozen corpus*, and `cmd_sample` — already guarded by its own refusal — would sample that many for a future generation). Add a distinct extension seed next to `SEED`:

```python
EXTEND_SEED = 20260919
EXTEND_PER_BAND = 15
```

Update the module docstring's verb list with one `extend` line.

- [ ] **Step 3: `cmd_extend`**

Append-only, and refuses once the corpus is full:

```python
def cmd_extend():
    rows = read_corpus()
    if len(rows) >= len(BANDS) * PER_BAND:
        sys.exit("%s already holds %d rows — the corpus is at its target size and "
                 "`extend` is append-only. Raise PER_BAND deliberately to grow it "
                 "further." % (CORPUS, len(rows)))
    present = {r["id"] for r in rows}
    pool = candidates()
    rng = random.Random(EXTEND_SEED)
    added = []
    for b in range(len(BANDS)):
        fresh = sorted((e for e in pool[b] if e["id"] not in present),
                       key=lambda e: e["id"])
        if len(fresh) < EXTEND_PER_BAND:
            sys.exit("band %d has only %d unused candidates, need %d"
                     % (BANDS[b][0], len(fresh), EXTEND_PER_BAND))
        for entry in sorted(rng.sample(fresh, EXTEND_PER_BAND), key=lambda e: e["id"]):
            entry["role"] = "anchor"
            added.append(entry)
    write_corpus(sorted(rows + added, key=lambda r: (band_of(int(r["rating"])), r["id"])))
    print("appended %d anchors -> %s (%d rows total)"
          % (len(added), CORPUS, len(rows) + len(added)))
```

Two properties this must hold, and the reviewer must confirm both: it never mutates an existing row's `role` or any other cell, and the sort key it writes reproduces the existing file's band-then-id order so the 80 frozen rows keep their text.

- [ ] **Step 4: Widen `cmd_check`**

Keep every existing assertion — unassigned rows, `excluded` matching the `EXCLUDED` constant, anchor/eval disjointness, 3 eval per band, no eval id present in `anchors.md`, the label-integrity comparison of each `anchors.md` row's rating and div against `corpus.md`, and `eval-set.md` agreeing with the corpus. Do not weaken any of them.

Add, derived rather than hard-coded so a second PDF-only exclusion does not silently pass:

```python
    expected_eval = len(BANDS) * EVAL_PER_BAND
    expected_anchors = len(BANDS) * PER_BAND - expected_eval - len(EXCLUDED)
    if len(evals) != expected_eval:
        failures.append("%d eval rows, expected %d" % (len(evals), expected_eval))
    if len(anchors) != expected_anchors:
        failures.append("%d anchor rows, expected %d" % (len(anchors), expected_anchors))
    for band in sorted({r["band"] for r in rows}):
        n_band = sum(1 for r in rows if r["band"] == band)
        if n_band != PER_BAND:
            failures.append("band %s has %d rows, expected %d" % (band, n_band, PER_BAND))
```

- [ ] **Step 5: Prove each changed assertion still fires**

A guard that has never failed is not known to work. Copy `corpus.md` to the session scratchpad, and for each of the four perturbations below apply it to the *working* copy, run `check`, confirm the named `FAIL` line and a non-zero exit, then restore from the scratchpad copy:

1. delete one anchor row → `corpus has 199 rows, expected 200`, `199 anchor…`/band-count failure;
2. flip one anchor row's role to `eval` → `4 eval, expected 3` for its band and the eval/anchor count failures;
3. change one anchor row's rating cell so it disagrees with `anchors.md` → `LABEL MISMATCH`;
4. append a duplicate of an existing eval row into `anchors.md`'s table → `CONTAMINATION: eval ids present in anchors.md`.

Restore the repository copy of both files afterwards and confirm `check` is back to passing (it will still fail on counts until Task 2 runs `extend` — that is expected and the report must say which failures are the expected pre-extension ones).

- [ ] **Step 6: Commit**

```bash
git add skills/calculating-difficulties/calibration/fetch-corpus.py
git commit -m "feat: add the append-only extend verb and widen the corpus check to 200"
```

---

### Task 2: Extend the corpus and fetch the 120 new statements

Mechanical: one API call and ~120 polite curls. No model tokens.

**Files:**
- Modify: `skills/calculating-difficulties/calibration/corpus.md` (append only)
- Create (gitignored): `.cache-cf-corpus/*.txt`

- [ ] **Step 1** — `python …/fetch-corpus.py extend`; expect `appended 120 anchors … (200 rows total)`. Run it again and confirm it refuses.
- [ ] **Step 2** — `git diff` `corpus.md` and confirm the diff is **pure insertion**: no line removed, no existing line modified.
- [ ] **Step 3** — `python …/fetch-corpus.py fetch`; retry until every id is cached or a failure is genuinely persistent.
- [ ] **Step 4** — any id that persistently returns `no statement in response` is PDF-only: add it to `EXCLUDED` with a one-line comment naming why, and re-run `check`. Never edit its corpus row.
- [ ] **Step 5** — `check` must report `anchors: 175   eval: 24   excluded: 1` and `ALL CHECKS PASSED`. Label integrity only walks rows *present in* `anchors.md`, so the 120 ids not yet summarised do not trip it. If `check` fails here, stop and read the failure.
- [ ] **Step 6** — commit `corpus.md` (and `fetch-corpus.py` if `EXCLUDED` changed).

---

### Task 3: Summarise the 120 new anchors — ~12 subagents

**Files:**
- Create (workspace, not the repo): one batch file per subagent.

- [ ] **Step 1** — list the 120 new anchor ids with rating and div; split into 12 batches of 10.
- [ ] **Step 2** — dispatch one subagent per batch with the prior generation's summariser prompt verbatim (Task 13 Step 2 of the 2026-09-16 plan), each writing its rows to its own batch file. **120 rows must not pass through the controller's context.**
- [ ] **Step 3** — verify each batch file: 10 rows, every `prereq` cell an exact left-column string from `tag-floors.md` or `none`, every sentence under 25 words, no row missing.

---

### Task 4: Assemble `references/anchors.md` to 175 rows

**Files:**
- Modify: `skills/calculating-difficulties/references/anchors.md`

- [ ] **Step 1** — concatenate the 55 existing rows with the 120 new ones, sorted by rating then id. Existing rows keep their text exactly.
- [ ] **Step 2** — update the trailing count sentence from `**55 anchors**` to `**175 anchors**`. The file must still not name `calibration/`.
- [ ] **Step 3** — `check` passes (it now also proves no eval id leaked in and no label disagrees with the corpus), `grep -c '^| [0-9]'` is `175`, and the tag distribution has no single tag holding an implausible share.
- [ ] **Step 4** — commit.

---

### Task 5: Round 4 — blind eval on the same 24 slots

**Files:**
- Create: `skills/calculating-difficulties/calibration/predictions-round4.md`
- Modify: `skills/calculating-difficulties/calibration/metrics.md` (append)

- [ ] **Step 1** — `SKILL.md` and `tag-floors.md` are byte-identical to their round-3 state. Verify with `git diff feat/calculating-difficulties -- <both paths>` producing empty output.
- [ ] **Step 2** — 5 subagents, 4-5 slots each, the round-1 eval prompt verbatim, blind.
- [ ] **Step 3** — write `predictions-round4.md`; 24 rows.
- [ ] **Step 4** — review each agent's report for contamination (a named real problem id, or any read under `calibration/`). A contaminated batch is re-run, never repaired.
- [ ] **Step 5** — append the `metrics` output to `metrics.md`; `grep -c '^## predictions-'` is `5`.
- [ ] **Step 6** — commit.

---

### Task 6: The vocabulary ruling and round 5

Runs only after round 4 is scored, recorded and committed.

**Files:**
- Modify: `skills/calculating-difficulties/references/tag-floors.md`
- Create: `skills/calculating-difficulties/calibration/predictions-round5.md`
- Modify: `skills/calculating-difficulties/calibration/metrics.md` (append)

- [ ] **Step 1** — extend the vocabulary with rows covering the gaps the summarisers actually named (interactive query design, constructive/ad-hoc casework, number theory, binary lifting, small-to-large merging), placing each at the floor its own anchors' rating distribution supports. Derive each new row's floor from the measured ratings of the 175 anchors that would carry that tag — not from intuition. Do not re-order or re-value any existing row.
- [ ] **Step 2** — re-tag the anchors whose `prereq` is `none` but which match a new row. This is an edit of `anchors.md`'s `prereq` column only; ratings, divs and sentences are untouched, so `check` must still pass.
- [ ] **Step 3** — round 5: 5 subagents, same prompt, same 24 slots, blind.
- [ ] **Step 4** — append to `metrics.md`; `grep -c '^## predictions-'` is `6`.
- [ ] **Step 5** — commit, and record the ruling with what it costs if wrong.

---

### Task 7: Freeze

**Files:**
- Modify: `skills/calculating-difficulties/SKILL.md` (`## Calibration status` only)
- Modify: `docs/superpowers/process/calculating-difficulties.md`
- Possibly modify: `skills/calculating-difficulties/references/anchors.md`, `references/tag-floors.md` (revert, per the ship rule)

- [ ] **Step 1** — apply the pre-registered ship rule to the measured `m4` and `m5`.
- [ ] **Step 2** — rewrite `## Calibration status` with the shipped configuration's measured MAE, within-±200, bias, eval `n`, baseline, anchor count and date, the selection-honesty disclosure, and no interval it did not earn.
- [ ] **Step 3** — `grep -c "calibration/"` over `SKILL.md` and every file under `references/` is `0`; `check` passes; `grep -c '^## predictions-'` in `metrics.md` is `6`.
- [ ] **Step 4** — update the run-state doc: ledger rows for this generation, **Where we are**, the deviations, and the rulings.
- [ ] **Step 5** — commit. Do **not** merge and do **not** push.

---

## Verification summary

| Property | Proved by | Task |
|---|---|---|
| The frozen 80 are untouched | `git diff` on `corpus.md` is pure insertion | 2 |
| `sample`/`split` were never re-run | their refusals are intact; `corpus.md` row text unchanged | 1, 2 |
| The corpus is 200 = 175 + 24 + 1 | `check` counts, derived from constants | 1, 4 |
| Every widened assertion actually fires | four scratchpad perturbations | 1 |
| Anchors and eval stay disjoint | `check`'s leakage scan over `anchors.md` | 4 |
| Anchor labels match the corpus | `check`'s label-integrity comparison | 4 |
| Round 4 varies only the anchor count | empty `git diff` for `SKILL.md` and `tag-floors.md` | 5 |
| Round 5 varies only the vocabulary | round 4 scored and committed first | 6 |
| No round was overwritten | `metrics.md` holds 6 `## predictions-` sections | 5, 6, 7 |
| Runtime cannot see build-time | `grep -c "calibration/"` = 0 in `SKILL.md` and `references/` | 7 |
| The shipped claim is measured, not asserted | `## Calibration status` quotes the shipped round | 7 |
