---
name: calculating-difficulties
description: Use for workflow Step 8a, whenever Step 8 runs, or whenever the user asks how hard a prepared problem is, what rating it would get, or wants outputs/difficulty.md written — estimate difficulty, expected rating, Codeforces rating, 800-3500, độ khó, ước lượng rating, bài này rating bao nhiêu, bài này khó cỡ nào.
---

# Estimate problem difficulty

Write one file, `outputs/difficulty.md`, in the problem repository. Change nothing else — Step 8 reads this file and copies the number; it does not re-estimate.

**Write this file in English**, even though the editorial and the rest of the workflow are in Vietnamese. It is an audit trail for the setter and for the agent that re-reads it, not a student-facing document, and the anchors, tag names, and calibration vocabulary it cites are all English. Step 8 copies the number, not the prose.

## What the number means

**The rating the full-constraint version would receive as a single all-or-nothing problem in a modern Div1/Div2 round.**

The word **modern** is load-bearing. The scale drifts, so a rating only means something with a date attached; this skill states its answer on today's scale, and Pass C.1 is what puts it there.

**One number, for the whole problem.** Never a per-subtask rating, and never a Vietnamese tier (`dễ` / `trung bình` / `khó`): the output is an integer on the Codeforces scale, or `not estimable`.

It is a **comparability figure, not a prediction**. It tells a Codeforces-literate reader what class of problem this is. It does not predict how a particular group of students will perform, and it says nothing about individual subtasks. The output file must repeat this, or the number will be over-trusted.

Codeforces ratings are fitted from contest performance — roughly the rating at which in-contest solve probability is 50%. Tags and constraints are not inputs to that fit. There is no formula to recover; this skill places a problem against real rated problems instead.

## Gate: is this estimable at all?

Estimate only when the implementation being rated was validated:

1. `source/solution.cpp` exists and passed the complete Step 1 audit; or
2. a full-scope AC in `outputs/solution/manifest.md` is declared validated.

File existence is not proof of validation. With neither, **stop and write the `not estimable` file** in `Failure branch` below. Do not guess from the statement alone, and do not block the workflow waiting for a solution.

## Pass A — features, no numbers

Read `source/problem-context.md` and the validated implementation. Write down, before considering any number:

- the prerequisite techniques the intended solution genuinely requires;
- the count of **independent** non-obvious observations. Two restatements of one idea count once. A step that follows routinely once the main technique is chosen is not a separate observation, however separable it looks — reducing a range query to two prefix queries, or compressing a state to its residue, is scaffolding, not insight. Count an observation only if a solver could have the main idea and still fail to find it;
- the single hardest step, in one sentence;
- implementation weight: how much code, and how error-prone;
- whether the constraints force the intended solution, or a simpler one also fits;
- whether the statement disguises a standard object (a graph described as a story, a DP described as a game).

**Write no rating in this pass.** Separating extraction from scoring is what stops the estimate being reverse-engineered from a number you already had in mind.

## Pass B — the prerequisite floor

Read [`references/tag-floors.md`](references/tag-floors.md). Take the hardest technique the solution *requires* — not one that merely appears — and read off its floor.

A floor is not an estimate. It is the level below which this problem cannot land, whatever the code looks like. It exists to block the common failure of rating a digit-DP problem 1400 because the implementation came out clean.

If the solution requires nothing on the list, the floor is `1100`.

**A `1100` floor caps what the later passes can reach at about `2200`**, because Pass C's window starts at the floor. That is a limit of the floor table, not a judgement about the problem: 37% of the anchors are themselves untagged while spanning roughly 1100 to 2600. So when Pass B lands on `1100`, treat the final number as a **lower bound** and say so in `Confidence`.

## Pass C — anchor placement

**This pass produces the estimate and carries the accuracy.** Judging magnitude on an unfamiliar scale is hard; comparing two concrete problems is not. Do the second.

Read [`references/anchors.md`](references/anchors.md). Select the window `[floor, floor+600]` on the table's **printed** `rating` column — the window is fixed by the floor, and selection happens before any correction, or the window would slide with every anchor. Inside it, pick 2-3 anchors and place this problem against them:

- one that is clearly **harder** than this problem — one sentence saying why;
- one **similar** — one sentence;
- one clearly **easier** — one sentence.

**Copy each anchor's `id`, `rating` and `year` out of the table, character for character.** An id you cannot find in `anchors.md` is one you invented, and the comparison resting on it is worth nothing — two recorded calibration rounds cited `1063D`, which is not a Codeforces problem.

**Choose the best-matched anchors, and let their years fall where they may.** An earlier version of this pass told you to prefer the more recent of two comparable anchors. Blind round 6 measured that instruction and it cost 42 points of MAE: only 30 of the 171 anchors are from 2025-2026, so the preference quietly shrank the usable table to a fifth of its size and the comparisons got worse. Match quality beats recency every time — Pass C.1 puts an old anchor onto today's scale arithmetically, which is exactly so that you never have to avoid one.

**Two categories are enough when the third cannot exist.** The window starts at the floor, so a problem that genuinely sits at its floor has nothing easier to compare against — that is the floor working, not a gap to paper over. Fill the categories the window supports, name the missing one in the output, and do not reach below the floor or invent a comparison to fill a slot.

**Trust the anchor's rating over your own sense of difficulty.** The characteristic failure of this pass is treating the anchors as a sanity check on a number you already formed — which leaves the scale compressed, easy problems rated too high and hard ones too low. Read the anchor's rating first, decide whether this problem is harder or easier than *that specific problem*, and let the number follow from the comparison. If your estimate ends more than `300` away from every anchor you compared against, you did not place the problem against them — redo the comparison rather than keeping the number.

State the estimate this placement implies, using the corrected ratings from Pass C.1. If no anchor in the window is comparable, widen to `[floor, floor+800]` and say in the output that the placement was weak. Widening searches upward only; it can never supply an easier anchor, so never widen for that reason.

## Pass C.1 — era correction

A Codeforces rating is fitted from how the field that competed **that year** actually performed. The field has not stayed still: techniques that were exotic in 2018 are standard preparation now, editorial and blog coverage of them is far denser, and the median contestant has seen more of them. Set the same problem in a round today and more of the field solves it, so it is fitted lower.

Two consequences follow, and they point the same way:

- **The older the problem, the lower its difficulty on today's scale.** An old printed rating overstates what the same problem would be rated now.
- **The older the problem, the larger the bias in its label.** Old ratings were fitted on a smaller, noisier field and on a scale that has since drifted, so age adds uncertainty as well as offset.

So for every anchor Pass C selected, compute a **today rating** and compare against that, never against the printed one:

```
today(anchor) = printed rating − era discount(year)
```

| Anchor year | Era discount | Label bias it carries |
|---|---|---|
| 2025-2026 | `0` | none |
| 2023-2024 | `−50` | small |
| 2021-2022 | `−100` | moderate |
| 2019-2020 | `−150` | large |
| 2018 or earlier | `−200` | large |

The `year` column of `references/anchors.md` supplies the year.

**This is a level shift, not a de-compression.** Across the anchor table the discount averages
`−102` and stays between `−75` and `−117` in every rating band, so in practice it lowers the
whole comparison set by about `100` rather than stretching it. That is all it is meant to do.
An explicit de-compression step was measured blind on held-out problems and made every figure
worse — see `Calibration status`.

Four rules keep this honest:

1. **Correct the anchor, never the estimate.** Once the anchors are on today's scale, the placement they imply is already on today's scale. Discounting the result again double-counts the same drift.
2. **Never correct the floor.** `tag-floors.md` states what knowledge a solution presupposes, which is a fact about the technique, not about the year some other problem was set. An era correction that pushes a placement under the floor is resolved by Pass E, which raises it back.
3. **The discount caps at `−200`.** It moves a placement by at most two rating steps. It is a correction, not a second opinion, and it can never carry a problem across the scale.
4. **Old anchors cost confidence.** If half or more of the anchors compared are from `2020` or earlier, the placement rests on the labels needing the largest and least certain correction: drop confidence one level (high→medium, medium→low) and say so in `Confidence`.

Report the printed rating, the year, and the today rating for every anchor, so a reader can audit the correction instead of trusting it.

## Pass D — adjustments, capped at ±300

Apply only what Pass A actually found:

| Adjustment | When |
|---|---|
| `+100` each, cap `+200` | each independent insight beyond the first |
| `+100` | the statement disguises a standard object |
| `+100` | genuinely heavy implementation, not merely long |
| `-100` to `-200` | a textbook exercise in its technique. `-100` is the default; charge `-200` only when the problem is the technique's bare demonstration case, with no secondary constraint and nothing combined on top |
| `-100` | constraint leakage — a bound that names the technique by itself, readable from the constraints line alone without the legend, as `n ≤ 20` broadcasts "bitmask". A bound that merely rules out brute force is not leakage |

**The total is capped at ±300**, and that cap is the point: deltas must never override the anchors. If the deltas want more than ±300, the anchor placement in Pass C was wrong — redo Pass C instead of raising the cap.

The era correction is not one of these adjustments and does not count against this cap. It applies to the anchors in Pass C.1, before a placement exists; these deltas apply to the placement afterwards.

## Pass E — gates and output

1. **If Pass D pushed the number below the Pass B floor, raise it back to that floor.** The floor binds the final answer, not merely the anchor placement — otherwise a negative adjustment reopens exactly the failure Pass B exists to close. When this fires, say so in `Confidence`: the adjustments disagreed with the floor, and the floor won.
2. Round to the nearest `100`.
3. Clamp to `[800, 3500]`.
4. Emit an interval, not a bare point estimate: `<number> ± 300`. The measured MAE is 210, which earns a `± 300` interval but nothing tighter — see `Calibration status` below. Clamp the interval's endpoints to `[800, 3500]` too, the same bound as step 3. **The output must say what that interval covered** — it is a measured error band, not a confidence interval, and a reader who is not told will assume otherwise.
5. **Look up every anchor id you wrote in `references/anchors.md`.** One that is not there means Pass C compared this problem against one that does not exist — redo Pass C rather than shipping the placement.
6. Write `outputs/difficulty.md`.

The era correction can be what drives a placement under the floor, and step 1 then raises it back. That is the design working, not a conflict: a discount says an old *label* reads high, while the floor says a *technique* cannot be learned cheaply. When both fire, report both in `Confidence`.

## The output file

Write exactly this shape, in English:

```markdown
# Estimated difficulty

**Expected rating: 2100 ± 300 (estimated)**

This is a comparability figure on the Codeforces scale, not a prediction of how any
particular group of students will perform, and it does not apply to individual subtasks.
It is stated on today's scale: anchor ratings were era-corrected before comparison.
The `± 300` is a measured error band, not a confidence interval — in blind calibration it
contained 85% of estimates, and ±400 contained 88%.

## Basis

- **Prerequisite floor:** 1900 — digit DP is the hardest technique the solution requires.
- **Independent observations:** 2
- **Hardest step:** <one sentence>
- **Implementation weight:** <one sentence>

## Anchor placement

| Problem | Printed | Year | Today | Comparison |
|---|---|---|---|---|
| 2210D | 2100 | 2026 | 2100 | Harder: <one sentence> |
| 1867E1 | 2000 | 2023 | 1950 | Similar: <one sentence> |
| 2133D | 1900 | 2025 | 1900 | Easier: <one sentence> |

Placement against the today ratings: 2000.

## Adjustments

| Adjustment | Reason |
|---|---|
| +100 | <reason> |

Total adjustment: +100 (capped at ±300). 2000 + 100 = 2100.

## Confidence

<high / medium / low, and one sentence saying why — a weak anchor window, anchors mostly
from 2020 or earlier, an unusual technique, a floor that overrode the adjustments, a
solution whose complexity depends on input shape, or a `1100` floor, which makes the number
a lower bound rather than a placement>

---
Source: `calculating-difficulties` skill, blind calibration 2026-09-22, MAE 210, n=48 held out,
era correction applied.
```

Keep it to that. This file is an audit trail, not an essay.

## Failure branch

No validated implementation ⇒ write `outputs/difficulty.md` containing:

```markdown
# Estimated difficulty

**Expected rating: not estimable**

No solution has been validated at Step 1, so the difficulty cannot be estimated.
<one sentence naming what is missing>

---
Source: `calculating-difficulties` skill.
```

Then continue. A missing estimate is not a workflow failure — Step 8 renders `chưa xác định` and moves on. Never replace the absence with a guess.

## Calibration status

Measured blind at round 9A on 2026-09-22 against **48 held-out problems the rubric was never
tuned against** — verified disjoint from the anchor table, from the old eval set, and from
the frozen corpus. Agents saw the statement and these references, never a true rating. This
is the skill's first un-fitted measurement, and it measures exactly the configuration shipped
here. The round history and the rejected candidates live in the skill's build-time
calibration record; this section carries only what changes how the number is reported.

| | value | target | met |
|---|---|---|---|
| MAE | 210 | ≤ 200 | misses by 10 |
| within ±200 | 71% | ≥ 65% | yes |
| signed bias | +19 | \|bias\| ≤ 75 | yes |
| worst-band \|bias\| | 233 | ≤ 200 | misses by 33 |
| MAE, no rubric, same model | 277 | — | — |

Two targets met, two missed by less than the ±28 standard error of a 48-problem set. **The
rubric is worth about 67 MAE points** against unaided guessing on the same model, and most of
that is calibration rather than raw accuracy: unaided estimates run +140 biased, the rubric
+19.

MAE 210 earns the `± 300` interval. **That interval is not a confidence interval**, but it is
closer to one than it used to be: ±300 contained 85% of blind estimates and ±400 contained
88%.

**Do not try to de-compress this estimate.** Earlier figures, taken on problems the rubric had
been tuned against for eight rounds, showed a badly compressed scale (slope `0.54`) and an
explicit expansion step looked justified. On held-out problems the slope is `0.875`, and a
blind arm applying that expansion measured worse on every figure — MAE 250, ±200 60%,
worst-band 350 — and moved the slope the wrong way. The compression was mostly a property of
the old sample.

**The floor table is the binding limitation.** 19 of the 48 problems required no technique on
`tag-floors.md` and floored at `1100`, which carries no information; two more received a floor
above the problem's true rating. When Pass B lands on `1100`, the number rests entirely on the
Pass C placement — say so in `Confidence`.

**Known cost.** Four arms were compared on this set, so these figures carry a small
multiple-comparison optimism; the shipped arm was the control rather than a selected winner,
which limits it. Codeforces ratings themselves quantize to 100 and carry about ±150 of
inherent noise: no method places a problem more precisely than that.
