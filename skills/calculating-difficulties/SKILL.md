---
name: calculating-difficulties
description: Estimate a Codeforces-style difficulty rating (800-3500) for a prepared problem and record it in outputs/difficulty.md, written in English. Use for workflow Step 8a, whenever Step 8 runs, or whenever the user asks how hard a problem is — estimate difficulty, expected rating, Codeforces rating, độ khó, ước lượng rating, bài này rating bao nhiêu, bài này khó cỡ nào. Rates the whole problem only: never a per-subtask rating, never a Vietnamese tier. Requires a solution validated by Step 1; with none it writes `not estimable` instead of guessing.
---

# Estimate problem difficulty

Write one file, `outputs/difficulty.md`, in the problem repository. Change nothing else — Step 8 reads this file and copies the number; it does not re-estimate.

**Write this file in English**, even though the editorial and the rest of the workflow are in Vietnamese. It is an audit trail for the setter and for the agent that re-reads it, not a student-facing document, and the anchors, tag names, and calibration vocabulary it cites are all English. Step 8 copies the number, not the prose.

## What the number means

**The rating the full-constraint version would receive as a single all-or-nothing problem in a modern Div1/Div2 round.**

The word **modern** is load-bearing. The scale drifts, so a rating only means something with a date attached; this skill states its answer on today's scale, and Pass C.1 is what puts it there.

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

## Pass C — anchor placement

**This pass produces the estimate and carries the accuracy.** Judging magnitude on an unfamiliar scale is hard; comparing two concrete problems is not. Do the second.

Read [`references/anchors.md`](references/anchors.md). Select the window `[floor, floor+600]` on the table's **printed** `rating` column — the window is fixed by the floor, and selection happens before any correction, or the window would slide with every anchor. Inside it, pick 2-3 anchors and place this problem against them:

- one that is clearly **harder** than this problem — one sentence saying why;
- one **similar** — one sentence;
- one clearly **easier** — one sentence.

**Choose the best-matched anchors, and let their years fall where they may.** An earlier version of this pass told you to prefer the more recent of two comparable anchors. Blind round 6 measured that instruction and it cost 33 points of MAE: only 30 of the 171 anchors are from 2025-2026, so the preference quietly shrank the usable table to a fifth of its size and the comparisons got worse. Match quality beats recency every time — Pass C.1 puts an old anchor onto today's scale arithmetically, which is exactly so that you never have to avoid one.

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

The `year` column of `references/anchors.md` supplies the year. Four rules keep this honest:

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
4. Emit an interval, not a bare point estimate: `<number> ± 300`. The measured MAE is 254, which earns a `± 300` interval but nothing tighter — see `Calibration status` below. Clamp the interval's endpoints to `[800, 3500]` too, the same bound as step 3.
5. Write `outputs/difficulty.md`.

The era correction can be what drives a placement under the floor, and step 1 then raises it back. That is the design working, not a conflict: a discount says an old *label* reads high, while the floor says a *technique* cannot be learned cheaply. When both fire, report both in `Confidence`.

## The output file

Write exactly this shape, in English:

```markdown
# Estimated difficulty

**Expected rating: 2100 ± 300 (estimated)**

This is a comparability figure on the Codeforces scale, not a prediction of how any
particular group of students will perform, and it does not apply to individual subtasks.
It is stated on today's scale: anchor ratings were era-corrected before comparison.

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
from 2020 or earlier, an unusual technique, a floor that overrode the adjustments, or a
solution whose complexity depends on input shape>

---
Source: `calculating-difficulties` skill, blind calibration 2026-09-19, MAE 254, n=24,
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

Calibrated against 171 anchors drawn from rated Div1/Div2 problems (2018 onward), and
measured on the same 24 blind problems used for every round (see the selection note below) —
agents that saw the statement and these references, never a true rating. Those agents derived
each intended solution from the statement alone; at runtime this skill reads a validated
implementation instead, so the figures below are measured under harder conditions than it
normally works in — likely pessimistic, but by an unmeasured amount.

| | value |
|---|---|
| MAE | 254 |
| within ±200 | 54% |
| within ±300 | 62% |
| signed bias | −46 |
| worst-band \|bias\| | 333 |
| band spread | 633 |
| eval n | 24 |
| baseline MAE (no rubric) | 658 |
| measured | blind round 7, 2026-09-19 |

**Two of the four accuracy targets are missed.** The targets are MAE ≤ 200, ≥65% within
±200, |bias| ≤ 75, and worst-band |bias| ≤ 200. Measured MAE is 254 and 54% of estimates land
within ±200, so those two are not met; |bias| is −46 and passes; worst-band bias is 333 and
does not. The skill roughly halves the error of an unaided guess (658 → 254) and doubles the
hit rate within ±200 (25% → 54%), and that is the whole of what it has demonstrated.

Because measured MAE is 254, which is at or below 300, the skill emits a `± 300` interval
rather than a bare range. Report the estimate as `<placement> ± 300 (estimated)`. Nothing
tighter is earned.

**|bias| ≤ 75 is met, and on its own the number is misleading.** Signed bias is −46, but that
near-zero aggregate is substantially **cancellation** — the per-band biases are:

| band | bias |
|---|---|
| 1100-1299 | +200 |
| 1300-1499 | +300 |
| 1500-1699 | −33 |
| 1700-1899 | +33 |
| 1900-2099 | −100 |
| 2100-2299 | −100 |
| 2300-2499 | −333 |
| 2500-2699 | −333 |

The scale is still compressed from both ends: easy problems over-rated, hard ones
under-rated, the two offsetting into a small aggregate. Read the per-band column, never the
aggregate alone. This is why `worst-band |bias| ≤ 200` was added as a fourth target — an
average of signed errors cannot fail on a symmetrically compressed scale.

**Pass C.1 is supported by blind rounds 6 and 7, and the story is worth knowing.** Round 6
added the era correction together with two extras: a `Pass D.1` that expanded estimates ×1.20
about 1800, and a sentence in Pass C preferring the more recent of two comparable anchors.
Round 6 measured **worse** than the round-4 baseline — MAE 321 against 279, worst-band bias
467 against 400.

The culprit was the recency preference, not the arithmetic. Each round makes 72 anchor
citations; the table is 46% anchors from 2018-2020. Round 4 cited that era 33 times, round 6
cited it **3 times**. One sentence of preference emptied nearly half the anchor table, and
the placements got worse for want of well-matched comparisons. Round 7 removed that sentence
and `Pass D.1`, keeping the era discount alone, and produced the best figures of any round:
MAE 254, worst-band 333, spread 633.

| round | configuration | MAE | worst band | spread |
|---|---|---|---|---|
| 4 | no era correction | 279 | 400 | 767 |
| 6 | era + recency preference + de-compression | 321 | 467 | 767 |
| 7 | era correction alone (shipped) | 254 | 333 | 633 |

Two lessons are recorded in `calibration/metrics.md` rather than left to be rediscovered.
**First: a preference that narrows the anchor pool costs more than any arithmetic correction
gains.** Match quality is what Pass C runs on. **Second: both discarded steps were justified
by a retrodiction** — recomputing what alternative arithmetic would have produced from
recorded placements — which predicted MAE 277 and worst-band 283 where the blind round
delivered 321 and 467. Retrodiction holds the estimator's behaviour fixed and can only score
arithmetic, so it is blind to exactly the changes that matter most. No change to these passes
ships on retrodiction evidence again.

Also tested and rejected: replacing the estimate with the median of the compared anchors (MAE
292) or with the hardest anchor compared (MAE 283). The first is informative — it rules out
pinning the estimate to an anchor, and shows anchor *selection* is compressed too.


**Known ceiling.** A problem whose prerequisites match no row in `tag-floors.md` floors at
`1100`, and the passes above can then reach at most about `2200`. That is not a judgement
the skill is making — it is a limit of the floor table, and 37% of the anchors are themselves
untagged while spanning roughly 1100 to 2600, so an untagged problem is not necessarily an
easy one. When Pass B lands on `1100`, treat the result as a **lower bound** rather than a
placement, and say so in `Confidence`. **The ceiling bound in the measurement above.** The
highest estimate any floor-`1100` problem received was exactly `2200`, and two floor-`1100`
problems in that run had true ratings above it — `2400` and `2300`, both estimated `1800`.
The ceiling is one cause of high-end under-rating but not the only one: of the two largest
under-estimates in that run, one floored at `1100` (true `2400`, estimated `1800`) and the
other floored at `1200` (true `2200`, estimated `1400`).

**Selection honesty.** This configuration was chosen out of seven rounds measured against the
same 24 problems. Choosing against a fixed eval set is itself a form of fitting: after seven
rounds those 24 problems are not held out in any strict sense, so the figures above are
optimistic by an unmeasured amount, and the honest next step is a fresh eval set rather than
an eighth round against this one. One earlier round measured 275 and was rejected anyway,
because it produced a prerequisite floor above a problem's true rating — which disqualifies a
construct whose only job is to be a lower bound, whatever its MAE.

Round 7's 254 leads round 4's 279 by 25 points, which is inside the ±40 standard error, so
the era correction's headline gain is suggestive rather than established. What is established
is the negative result: round 6's 321 against round 4's 279, with a mechanism confirmed in
the anchor citations, is a real regression and a real diagnosis.

A 24-problem eval set carries roughly ±40 standard error, so treat these figures as accurate
to about that much and no better. Codeforces ratings themselves quantize to 100 and carry
about ±150 of inherent noise: no method places a problem more precisely than that.
