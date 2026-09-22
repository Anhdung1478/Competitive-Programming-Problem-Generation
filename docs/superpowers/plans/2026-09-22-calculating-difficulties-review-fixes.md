# calculating-difficulties review fixes — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Apply the consistency fixes and prompt-craft improvements from the 2026-09-22 review of the `calculating-difficulties` skill without changing any pass's arithmetic, and record every runtime-wording change as unmeasured drift with a pre-registered round 10.

**Architecture:** Three documentation tasks on an existing skill. Task 1 fixes stale and contradictory figures and five malformed table rows. Task 2 restructures `SKILL.md` so the calibration figures live in one place, the procedure is scannable, the placement range is stated, and the gate names its proof. Task 3 records the drift in `calibration/metrics.md` and pre-registers round 10, where the method changes the review proposed are measured instead of shipped.

**Tech Stack:** Markdown; Python 3 for `calibration/fetch-corpus.py check`, an offline integrity check.

**Spec:** `docs/superpowers/specs/2026-09-22-calculating-difficulties-review.md`

## Global Constraints

- **Single home for calibration figures.** After this plan, the MAE, n, coverage percentages, bias and unaided-baseline figures of the current round appear in exactly one place: the `## Calibration status` section of `skills/calculating-difficulties/SKILL.md`. `README.md`, Pass E, the output template and its `Source:` line point there and carry no figure. The round id `9A` and its date `2026-09-22` may appear elsewhere as identifiers.
- **No arithmetic changes.** Do not edit `references/tag-floors.md`. Do not change any anchor's `rating` or `year`. Do not change the window `[floor, floor+600]`, the widening `[floor, floor+800]`, the era-discount table, the Pass D table or its `±300` cap, the `± 300` interval, or the `[800, 3500]` clamp.
- **Every runtime-wording change is recorded.** Each change to text an estimating agent reads (`SKILL.md`, `references/anchors.md`) is listed in `calibration/metrics.md` under `## Configuration drift since round 9A (2026-09-22)`, marked unmeasured. Task 3 writes that table; Tasks 1 and 2 must not introduce changes it does not list.
- **Integrity check passes after every task:** `python skills/calculating-difficulties/calibration/fetch-corpus.py check` prints `ALL CHECKS PASSED`.
- **The template's rating line is untouched:** the line `**Expected rating: 2100 ± 300 (estimated)**` stays character for character. `writing-editorials` copies it and translates `(estimated)`.
- **The frontmatter `description` of `SKILL.md` is untouched.**
- **Line endings are preserved.** Every file in this plan uses LF. After each edit, `git diff --stat` must show only the lines you meant to change; a whole-file line-ending conversion shows as every line changed and must be reverted.
- **YAGNI.** No new files beyond those named. No version bump in `.claude-plugin/plugin.json`.
- Prose in `SKILL.md` stays English. The description's Vietnamese trigger phrases stay.
- All paths below are relative to the repository root `E:\Project\plugins\Competitive-Programming-Problem-Generation`.

---

### Task 1: Mechanical consistency fixes

**Files:**
- Modify: `skills/calculating-difficulties/SKILL.md` (Pass C, the paragraph beginning `**Choose the best-matched anchors`)
- Modify: `skills/calculating-difficulties/references/anchors.md` (the five rows whose first cell is `2109C1`, `1129A2`, `2196C1`, `1063C`, `1783F`)
- Modify: `README.md:38`

**Interfaces:**
- Consumes: nothing from other tasks.
- Produces: the Pass C paragraph containing `cost 42 points of MAE` (Task 2 rewrites that paragraph and expects this text); `README.md` row 8a with no calibration figure.

- [ ] **Step 1: Correct the contradictory figure in Pass C**

In `skills/calculating-difficulties/SKILL.md`, replace exactly this text:

```
Blind round 6 measured that instruction and it cost 33 points of MAE:
```

with:

```
Blind round 6 measured that instruction and it cost 42 points of MAE:
```

- [ ] **Step 2: Verify Step 1**

```
grep -c "33 points" skills/calculating-difficulties/SKILL.md
grep -c "42 points of MAE" skills/calculating-difficulties/SKILL.md
```

Expected: `0` then `1`.

- [ ] **Step 3: Normalise the five seven-cell anchor rows**

In `skills/calculating-difficulties/references/anchors.md`, five table rows end with a seventh cell holding the unsure marker: the rows whose first cell is `2109C1`, `1129A2`, `2196C1`, `1063C` and `1783F`. In each of those five rows, replace the trailing text

```
| <!-- unsure --> |
```

with

```
 <!-- unsure --> |
```

so the marker becomes the last words of the sixth cell. For example, the `2109C1` row then ends with `rebuilds x as n. <!-- unsure --> |`. Change nothing else in any row.

- [ ] **Step 4: Verify every anchor row has six cells**

```
python -c "import io; bad=[(len(l.strip().strip('|').split('|')), l[:40]) for l in io.open('skills/calculating-difficulties/references/anchors.md', encoding='utf-8') if l.startswith('| ') and not l.startswith('| id ') and len(l.strip().strip('|').split('|')) != 6]; print('rows with cell count != 6:', bad)"
grep -c "<!-- unsure -->" skills/calculating-difficulties/references/anchors.md
```

Expected: `rows with cell count != 6: []` and `6` (five rows plus the footnote sentence that explains the marker).

- [ ] **Step 5: Remove the stale figures from the README row**

`README.md` line 38 is a single table row that starts with `| 8a | `calculating-difficulties` |`. Inside it, replace exactly this sentence:

```
Blind MAE 254 on 24 held-out problems (unaided guessing: 658), measured over seven blind rounds; the era correction's first form was rejected by round 6 and the surviving form set the best figures of any round — see `calibration/metrics.md`.
```

with:

```
Blind-calibrated against a held-out set; the current figures, the interval they earn and the round record live in the skill's `Calibration status` section and in `calibration/metrics.md`.
```

The rest of the row, including the closing sentence `It emits a `± 300` interval, not a bare point estimate |`, is unchanged.

- [ ] **Step 6: Verify Step 5**

```
grep -c "MAE 254" README.md
grep -c "658" README.md
grep -c "Calibration status" README.md
```

Expected: `0`, `0`, `1`.

- [ ] **Step 7: Run the integrity check and the diff check**

```
python skills/calculating-difficulties/calibration/fetch-corpus.py check
git diff --stat
```

Expected: the check prints `ALL CHECKS PASSED`. The stat shows three files with small line counts: `README.md` 1 changed line, `SKILL.md` 1 changed line, `anchors.md` 5 changed lines.

- [ ] **Step 8: Commit**

```
git add README.md skills/calculating-difficulties/SKILL.md skills/calculating-difficulties/references/anchors.md
git commit -m "fix(calculating-difficulties): correct the round-6 cost, drop stale README figures, fix five anchor rows"
```

---

### Task 2: SKILL.md restructure — one home for figures, a procedure map, the placement range, a concrete gate

**Files:**
- Modify: `skills/calculating-difficulties/SKILL.md` only.

**Interfaces:**
- Consumes: Task 1's Pass C paragraph containing `cost 42 points of MAE`.
- Produces: two section headings Task 3 names verbatim, `## The procedure at a glance` and `## Calibration status`; the template `Source:` line in the form `Source: `calculating-difficulties` skill, calibration round 9A (2026-09-22), era correction` followed on the next line by `applied.`

Every step below is an exact replacement. The old text is quoted from the file as it stands after Task 1; if a quoted block is not found character for character, stop and report `NEEDS_CONTEXT` rather than approximating.

- [ ] **Step 1: Insert the procedure map**

Directly after the paragraph that ends with `Step 8 copies the number, not the prose.` and before the heading `## What the number means`, insert this block, with one blank line on each side:

```
## The procedure at a glance

| Pass | Does | The rule that binds it |
|---|---|---|
| Gate | Confirms the implementation being rated was validated | No proof ⇒ write the `not estimable` file and continue |
| A | Extracts features: techniques, independent observations, hardest step, implementation weight | No number is written in this pass |
| B | Reads the prerequisite floor from `references/tag-floors.md` | The hardest technique *required*, not merely present; nothing listed ⇒ `1100` |
| C | Places the problem against 2-3 anchors from `references/anchors.md` in `[floor, floor+600]` | Ids copied verbatim; the estimate ends within `300` of an anchor compared |
| C.1 | Era-corrects each anchor's printed rating to today's scale | Corrects the anchors, never the estimate and never the floor |
| D | Applies feature adjustments | Total capped at `±300` |
| E | Floor gate, round, clamp, interval, anchor-id check, write | Interval is `± 300`; every id cited exists in `anchors.md` |

Each pass below states its rule in full. The table is the map, not the procedure.
```

- [ ] **Step 2: State the placement range**

Directly after the paragraph that begins `**One number, for the whole problem.**` and before the paragraph that begins `It is a **comparability figure, not a prediction**.`, insert this paragraph, with one blank line on each side:

```
**The placement range is about `1100` to `2900`, narrower than the scale.** The anchor table runs from `1100` to `2600`, the lowest floor is `1100`, and Pass C's window starts at the floor, so nothing below `1100` can be placed against an anchor and nothing above `2600` plus Pass D's cap can. The top three rows of `tag-floors.md` have no anchor carrying their technique. A problem that Pass A suggests lies outside this range still gets a number, but `Confidence` is `low` and says the placement ran off the end of the anchor table.
```

- [ ] **Step 3: Make the gate's proof concrete**

Replace exactly:

```
Estimate only when the implementation being rated was validated:

1. `source/solution.cpp` exists and passed the complete Step 1 audit; or
2. a full-scope AC in `outputs/solution/manifest.md` is declared validated.

File existence is not proof of validation. With neither, **stop and write the `not estimable` file** in `Failure branch` below. Do not guess from the statement alone, and do not block the workflow waiting for a solution.
```

with:

```
Estimate only when the implementation being rated was validated, and accept only these as proof:

1. the Step 1 audit of `source/solution.cpp` reported `PASS` in this session, or the user states that it did; or
2. `outputs/solution/manifest.md` declares a full-scope AC as validated.

File existence is not proof of validation, and a clean compile is not an audit. With neither, **stop and write the `not estimable` file** in `Failure branch` below, naming which proof is missing. Do not guess from the statement alone, and do not block the workflow waiting for a solution.
```

- [ ] **Step 4: Cut the round-6 story in Pass C to three sentences**

Replace exactly:

```
**Choose the best-matched anchors, and let their years fall where they may.** An earlier version of this pass told you to prefer the more recent of two comparable anchors. Blind round 6 measured that instruction and it cost 42 points of MAE: only 30 of the 171 anchors are from 2025-2026, so the preference quietly shrank the usable table to a fifth of its size and the comparisons got worse. Match quality beats recency every time — Pass C.1 puts an old anchor onto today's scale arithmetically, which is exactly so that you never have to avoid one.
```

with:

```
**Choose the best-matched anchors, and let their years fall where they may.** Blind round 6 measured a preference for recent anchors at +42 MAE: only 30 of the 171 anchors are from 2025-2026, so the preference shrank the usable table to a fifth of its size and the comparisons got worse. Pass C.1 puts an old anchor onto today's scale arithmetically, which is exactly so that you never have to avoid one.
```

- [ ] **Step 5: Remove the figure from Pass E step 4**

Replace exactly:

```
4. Emit an interval, not a bare point estimate: `<number> ± 300`. The measured MAE is 210, which earns a `± 300` interval but nothing tighter — see `Calibration status` below. Clamp the interval's endpoints to `[800, 3500]` too, the same bound as step 3. **The output must say what that interval covered** — it is a measured error band, not a confidence interval, and a reader who is not told will assume otherwise.
```

with:

```
4. Emit an interval, not a bare point estimate: `<number> ± 300`. The measured error earns `± 300` and nothing tighter; the figure itself lives in `Calibration status` below and nowhere else. Clamp the interval's endpoints to `[800, 3500]` too, the same bound as step 3. **The output must say what that interval is** — a measured error band, not a confidence interval, and a reader who is not told will assume otherwise.
```

- [ ] **Step 6: Remove the figures from the output template**

Three replacements inside the fenced template under `## The output file`.

6a. Replace exactly:

```
The `± 300` is a measured error band, not a confidence interval — in blind calibration it
contained 85% of estimates, and ±400 contained 88%.
```

with:

```
The `± 300` is a measured error band from the skill's blind calibration, not a confidence
interval; the coverage it achieved is recorded in the skill's `Calibration status`.
```

6b. Replace exactly:

```
<high / medium / low, and one sentence saying why — a weak anchor window, anchors mostly
from 2020 or earlier, an unusual technique, a floor that overrode the adjustments, a
solution whose complexity depends on input shape, or a `1100` floor, which makes the number
a lower bound rather than a placement>
```

with:

```
<high / medium / low, and one sentence saying why — a weak anchor window, anchors mostly
from 2020 or earlier, an unusual technique, a floor that overrode the adjustments, a
solution whose complexity depends on input shape, a `1100` floor, which makes the number
a lower bound rather than a placement, or a floor of `2100` or above or a placement of
`2500` or above, where the anchor table thins out>
```

6c. Replace exactly:

```
Source: `calculating-difficulties` skill, blind calibration 2026-09-22, MAE 210, n=48 held out,
era correction applied.
```

with:

```
Source: `calculating-difficulties` skill, calibration round 9A (2026-09-22), era correction
applied.
```

- [ ] **Step 7: Replace the Calibration status section**

Delete everything from the line `## Calibration status` to the end of the file, and write this in its place (the file ends after the last paragraph, with a single trailing newline):

```
## Calibration status

The figures below are the only copy in this repository; every other file points here.
Measured blind at round 9A on 2026-09-22 against 48 held-out problems the rubric was never
tuned against, disjoint from the anchor table, the old eval set and the frozen corpus. Agents
saw the statement and these references, never a true rating. The round history, the rejected
candidates and the pre-registered rules for the next round live in `calibration/metrics.md`.

| | value | target | met |
|---|---|---|---|
| MAE | 210 | ≤ 200 | misses by 10 |
| within ±200 | 71% | ≥ 65% | yes |
| within ±300 | 85% | — | — |
| within ±400 | 88% | — | — |
| signed bias | +19 | \|bias\| ≤ 75 | yes |
| worst-band \|bias\| | 233 | ≤ 200 | misses by 33 |
| MAE, no rubric, same model | 277 | — | — |

The two misses are inside the ±28 standard error of a 48-problem set. MAE 210 earns the
`± 300` interval Pass E emits, and that interval is a measured error band, not a confidence
interval. Two results bind the passes above: an explicit de-compression step measured worse
on every figure in a blind arm, so none is applied; and 19 of the 48 problems floored at the
uninformative `1100`, so a `1100` floor leaves the number resting on Pass C alone. Four arms
were compared on this set, so these figures carry a small multiple-comparison optimism.
Codeforces ratings themselves quantize to 100 and carry about ±150 of inherent noise; no
method places a problem more precisely than that.
```

- [ ] **Step 8: Verify the figures live only in Calibration status**

```
grep -nE "\b210\b|85%|88%|\b277\b|n=48|\b254\b|\b658\b" skills/calculating-difficulties/SKILL.md
grep -n "^## Calibration status" skills/calculating-difficulties/SKILL.md
```

Expected: every line number printed by the first command is greater than the line number printed by the second, and no line contains `254` or `658`.

- [ ] **Step 9: Verify structure, size and the untouched lines**

```
grep -c "^## The procedure at a glance" skills/calculating-difficulties/SKILL.md
grep -c '^\*\*Expected rating: 2100 ± 300 (estimated)\*\*$' skills/calculating-difficulties/SKILL.md
git diff --stat -- skills/calculating-difficulties/SKILL.md
wc -w skills/calculating-difficulties/SKILL.md
awk '/^## Calibration status/{f=1} f' skills/calculating-difficulties/SKILL.md | wc -w
head -4 skills/calculating-difficulties/SKILL.md | grep -c "^description: Use for workflow Step 8a"
```

Expected: `1`; `1`; a stat whose changed-line count is well under the file's length (the file is about 230 lines; a full-file conversion would show every line); total words at most `2950` (the map costs about what the Calibration status trim saves; the trim is measured by the next figure); Calibration status at most `260` words; `1`.

- [ ] **Step 10: Run the integrity check**

```
python skills/calculating-difficulties/calibration/fetch-corpus.py check
```

Expected: `ALL CHECKS PASSED`.

- [ ] **Step 11: Commit**

```
git add skills/calculating-difficulties/SKILL.md
git commit -m "docs(calculating-difficulties): one home for the figures, a procedure map, the placement range, a concrete gate"
```

---

### Task 3: Record the drift and pre-register round 10

**Files:**
- Modify: `skills/calculating-difficulties/calibration/metrics.md` (append two sections at the end of the file)

**Interfaces:**
- Consumes: the section names Task 2 created in `SKILL.md`: `## The procedure at a glance` and `## Calibration status`.
- Produces: nothing downstream.

Read the existing sections `## Configuration drift since round 7 (2026-09-19)` and `## Round 9 — pre-registered decision rules` in `metrics.md` first; the two sections you append mirror their shape.

- [ ] **Step 1: Append the drift table**

Append to the end of `skills/calculating-difficulties/calibration/metrics.md`, after one blank line:

```
## Configuration drift since round 9A (2026-09-22)

Round 9A measured the skill as it stood on the morning of 2026-09-22. The shipped `SKILL.md`
has since gained wording. None of the changes below is measured; round 10's control arm
measures them together.

| change | why | risk |
|---|---|---|
| Pass C.1: the "level shift, not a de-compression" paragraph | round 9C's negative result, recorded where the next reader would otherwise re-propose the step | wording, no arithmetic |
| Pass B: a `1100` floor caps the reachable estimate near `2200` and is reported as a lower bound | moved from `Calibration status`, which the agent reads after answering, into the pass that fires | wording, no arithmetic |
| `## The procedure at a glance`: a seven-row map at the top | 2,900 words with no overview; the map gives the passes an order and one binding rule each | changes what the agent reads first; unmeasured |
| "What the number means": the placement range is about `1100` to `2900`; `Confidence` names a floor ≥ `2100` or a placement ≥ `2500` | the anchor table spans `1100`-`2600` and the top three floor rows have no anchor; the description advertises `800`-`3500` | `Confidence` text only |
| Gate: proof is a Step 1 `PASS` in this session, the user's word, or a validated full-scope AC in the manifest | `validate-solution` writes no file, so a standalone Step 8a had nothing to check | affects whether an estimate is produced, not its value |
| Pass C: the round-6 story cut to three sentences; `33 points` corrected to `42` | the figure contradicted `anchors.md` and this file | wording |
| `Calibration status` cut to the table and one paragraph; figures removed from Pass E, the template and `README.md` | five copies of the round figures, one already stale | changes what the agent reads; unmeasured |
| Template: the coverage sentence and the `Source:` line carry no figures | same | output prose only |
| `anchors.md`: five rows (`2109C1`, `1129A2`, `2196C1`, `1063C`, `1783F`) had a seventh cell holding the unsure marker | the table misaligned in any renderer | none; the parser reads four cells |
```

- [ ] **Step 2: Append the round-10 pre-registration**

Append, after one blank line:

```
## Round 10 — pre-registered decision rules

Written 2026-09-22, before any round-10 number exists, and before any arm was dispatched.

**Prerequisite: a third held-out sample.** The fresh 48 have been compared against by four
arms and are no longer pristine. Round 10 runs on `eval-set-3.md`: 48 problems, 6 per band
across the same 8 bands, disjoint from `corpus.md`, `eval-set.md` and `eval-set-2.md`.
`fetch-corpus.py fresh-build` writes `eval-set-2.md` by name and must be generalised before
this round can run; that is build tooling and touches nothing the skill reads.

**Arms.** Same 48 slots, the round-1 eval prompt verbatim, 8 subagents of 6 slots each.

| arm | configuration |
|---|---|
| 10A | the shipped skill as of this entry — the drift list above applied, nothing else — control |
| 10B | 10A with the `two pointers, prefix sums, sorting + greedy` row removed from `tag-floors.md`, so those problems floor at `1100`. Two anchors carrying that tag, `1923B` and `2245B`, are rated `1100`, and round 9 floored two true-`1100` problems, `fresh-04` and `fresh-05`, at `1200`: the row is not a lower bound |
| 10C | 10A with Pass C.1 removed: anchors compared on their printed ratings. The era correction has never been measured alone on held-out problems; round 7's 25-point lead over round 4 was inside a ±40 standard error |
| 10D | 10A plus one structural rule in Pass C: at least one of the anchors compared must lie in the base window `[floor, floor+600]`, and the output names it. In 9A, 30 of 142 citations sat outside the base window, and the worst miss, `fresh-11` (true `1400`, estimated `2200`), took all three anchors from the top of the widened window |

**Primary endpoints.** 10B: the count of floors above a true rating, which must be `0`, and
the `1100-1299` band bias. 10C: MAE, since the correction claims a level shift. 10D: the
`1100-1499` bias and worst-band |bias|. MAE is the guard for every arm.

**Ship rules**, `a` = 10A, evaluated independently for 10B and 10D:

| condition | outcome |
|---|---|
| `worst_band ≤ a.worst_band − 100` and `MAE ≤ a.MAE + 28` | that arm ships |
| `MAE ≤ a.MAE − 28` | that arm ships whatever worst-band does |
| any estimate clamped by the floor to above its true rating | that arm is disqualified whatever its MAE |
| otherwise | that arm is reverted and recorded as a negative result |

**The rule for 10C is inverted**, because it removes a pass rather than adding one:

| condition | outcome |
|---|---|
| `MAE ≤ a.MAE − 28` | Pass C.1 is removed: the correction was costing accuracy |
| otherwise | Pass C.1 stays, and its measured gain, `a.MAE` against 10C's, is stated in `Calibration status` for the first time, bounded by the ±28 standard error |

**If both 10B and 10D qualify**, ship the lower worst-band; within 30, the lower MAE. **They
are not combined.** A combination is its own arm in a later round.

**Whatever ships**, `Calibration status` is restated on the round-10 figures, which remain the
only copy in the repository, and this file records every arm.
```

- [ ] **Step 3: Verify**

```
grep -c "^## Configuration drift since round 9A (2026-09-22)" skills/calculating-difficulties/calibration/metrics.md
grep -c "^## Round 10 — pre-registered decision rules" skills/calculating-difficulties/calibration/metrics.md
git diff --stat -- skills/calculating-difficulties/calibration/metrics.md
python skills/calculating-difficulties/calibration/fetch-corpus.py check
```

Expected: `1`; `1`; a stat showing only insertions, no deletions; `ALL CHECKS PASSED`.

- [ ] **Step 4: Commit**

```
git add skills/calculating-difficulties/calibration/metrics.md
git commit -m "calibration: record the post-9A drift and pre-register round 10"
```
