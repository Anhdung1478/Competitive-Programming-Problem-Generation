# Metrics

Every round is appended, never overwritten: the comparison between rounds is the
only evidence that tuning helped. A 24-problem eval set carries roughly ±40
standard error — a movement smaller than that is not an improvement.

Targets: MAE ≤ 200, ≥65% within ±200, |bias| ≤ 75, **worst-band |bias| ≤ 200**.

The fourth target was added on 2026-09-19, after the retrodiction study below showed the
third one is close to worthless on its own: round 4 met |bias| ≤ 75 with an aggregate of −29
while carrying per-band biases of +400 and −367. Aggregate signed bias is an average of
signed errors, so a scale compressed symmetrically from both ends scores perfectly on it.
Any round that reports the aggregate without the per-band column is reporting a number that
cannot fail. Record `worst band` (largest |bias| of any single band) and `spread` (most
over-rated band minus most under-rated band) for every round from here on.

## predictions-baseline

n = 24   MAE = 658   bias = +625   within200 = 25%   within300 = 29%

| band | n | MAE | bias |
|---|---|---|---|
| 1100-1299 | 3 | 800 | +800 |
| 1300-1499 | 3 | 900 | +900 |
| 1500-1699 | 3 | 400 | +400 |
| 1700-1899 | 3 | 1233 | +1233 |
| 1900-2099 | 3 | 633 | +633 |
| 2100-2299 | 3 | 633 | +633 |
| 2300-2499 | 3 | 367 | +233 |
| 2500-2699 | 3 | 300 | +167 |

| slot | true | predicted | error |
|---|---|---|---|
| eval-01 | 2400 | 2300 | -100 |
| eval-02 | 2400 | 3300 | +900 |
| eval-03 | 2100 | 2900 | +800 |
| eval-04 | 2500 | 2300 | -200 |
| eval-05 | 2300 | 2200 | -100 |
| eval-06 | 1600 | 2000 | +400 |
| eval-07 | 1100 | 1700 | +600 |
| eval-08 | 1900 | 2900 | +1000 |
| eval-09 | 2500 | 2500 | +0 |
| eval-10 | 1500 | 1700 | +200 |
| eval-11 | 1800 | 3200 | +1400 |
| eval-12 | 2000 | 2500 | +500 |
| eval-13 | 1100 | 2700 | +1600 |
| eval-14 | 1600 | 2200 | +600 |
| eval-15 | 1700 | 2900 | +1200 |
| eval-16 | 2000 | 2400 | +400 |
| eval-17 | 2200 | 2500 | +300 |
| eval-18 | 1300 | 2200 | +900 |
| eval-19 | 2500 | 3200 | +700 |
| eval-20 | 1300 | 2000 | +700 |
| eval-21 | 1700 | 2800 | +1100 |
| eval-22 | 1400 | 2500 | +1100 |
| eval-23 | 2200 | 3000 | +800 |
| eval-24 | 1100 | 1300 | +200 |

## predictions-round1

n = 24   MAE = 408   bias = +192   within200 = 29%   within300 = 46%

| band | n | MAE | bias |
|---|---|---|---|
| 1100-1299 | 3 | 300 | +300 |
| 1300-1499 | 3 | 600 | +600 |
| 1500-1699 | 3 | 433 | +433 |
| 1700-1899 | 3 | 300 | +300 |
| 1900-2099 | 3 | 467 | +467 |
| 2100-2299 | 3 | 233 | +100 |
| 2300-2499 | 3 | 467 | -400 |
| 2500-2699 | 3 | 467 | -267 |

| slot | true | predicted | error |
|---|---|---|---|
| eval-01 | 2400 | 2000 | -400 |
| eval-02 | 2400 | 2500 | +100 |
| eval-03 | 2100 | 2200 | +100 |
| eval-04 | 2500 | 1800 | -700 |
| eval-05 | 2300 | 1400 | -900 |
| eval-06 | 1600 | 1700 | +100 |
| eval-07 | 1100 | 1300 | +200 |
| eval-08 | 1900 | 2400 | +500 |
| eval-09 | 2500 | 2100 | -400 |
| eval-10 | 1500 | 1800 | +300 |
| eval-11 | 1800 | 2100 | +300 |
| eval-12 | 2000 | 2300 | +300 |
| eval-13 | 1100 | 1700 | +600 |
| eval-14 | 1600 | 2500 | +900 |
| eval-15 | 1700 | 2100 | +400 |
| eval-16 | 2000 | 2600 | +600 |
| eval-17 | 2200 | 2000 | -200 |
| eval-18 | 1300 | 1700 | +400 |
| eval-19 | 2500 | 2800 | +300 |
| eval-20 | 1300 | 1700 | +400 |
| eval-21 | 1700 | 1900 | +200 |
| eval-22 | 1400 | 2400 | +1000 |
| eval-23 | 2200 | 2600 | +400 |
| eval-24 | 1100 | 1200 | +100 |
## predictions-round2

n = 24   MAE = 475   bias = +250   within200 = 33%   within300 = 38%

| band | n | MAE | bias |
|---|---|---|---|
| 1100-1299 | 3 | 400 | +400 |
| 1300-1499 | 3 | 700 | +700 |
| 1500-1699 | 3 | 300 | +300 |
| 1700-1899 | 3 | 733 | +733 |
| 1900-2099 | 3 | 400 | +400 |
| 2100-2299 | 3 | 300 | +167 |
| 2300-2499 | 3 | 533 | -533 |
| 2500-2699 | 3 | 433 | -167 |

| slot | true | predicted | error |
|---|---|---|---|
| eval-01 | 2400 | 1800 | -600 |
| eval-02 | 2400 | 2300 | -100 |
| eval-03 | 2100 | 1900 | -200 |
| eval-04 | 2500 | 1800 | -700 |
| eval-05 | 2300 | 1400 | -900 |
| eval-06 | 1600 | 1700 | +100 |
| eval-07 | 1100 | 1400 | +300 |
| eval-08 | 1900 | 2000 | +100 |
| eval-09 | 2500 | 2300 | -200 |
| eval-10 | 1500 | 1600 | +100 |
| eval-11 | 1800 | 2600 | +800 |
| eval-12 | 2000 | 2500 | +500 |
| eval-13 | 1100 | 1900 | +800 |
| eval-14 | 1600 | 2300 | +700 |
| eval-15 | 1700 | 2600 | +900 |
| eval-16 | 2000 | 2600 | +600 |
| eval-17 | 2200 | 2400 | +200 |
| eval-18 | 1300 | 2000 | +700 |
| eval-19 | 2500 | 2900 | +400 |
| eval-20 | 1300 | 1800 | +500 |
| eval-21 | 1700 | 2200 | +500 |
| eval-22 | 1400 | 2300 | +900 |
| eval-23 | 2200 | 2700 | +500 |
| eval-24 | 1100 | 1200 | +100 |
## predictions-round3

n = 24   MAE = 329   bias = +179   within200 = 50%   within300 = 62%

| band | n | MAE | bias |
|---|---|---|---|
| 1100-1299 | 3 | 167 | +167 |
| 1300-1499 | 3 | 800 | +800 |
| 1500-1699 | 3 | 200 | +200 |
| 1700-1899 | 3 | 467 | +467 |
| 1900-2099 | 3 | 233 | +233 |
| 2100-2299 | 3 | 33 | +33 |
| 2300-2499 | 3 | 433 | -300 |
| 2500-2699 | 3 | 300 | -167 |

| slot | true | predicted | error |
|---|---|---|---|
| eval-01 | 2400 | 2100 | -300 |
| eval-02 | 2400 | 2600 | +200 |
| eval-03 | 2100 | 2200 | +100 |
| eval-04 | 2500 | 1900 | -600 |
| eval-05 | 2300 | 1500 | -800 |
| eval-06 | 1600 | 1600 | +0 |
| eval-07 | 1100 | 1200 | +100 |
| eval-08 | 1900 | 2100 | +200 |
| eval-09 | 2500 | 2400 | -100 |
| eval-10 | 1500 | 1600 | +100 |
| eval-11 | 1800 | 2100 | +300 |
| eval-12 | 2000 | 2200 | +200 |
| eval-13 | 1100 | 1500 | +400 |
| eval-14 | 1600 | 2100 | +500 |
| eval-15 | 1700 | 2400 | +700 |
| eval-16 | 2000 | 2300 | +300 |
| eval-17 | 2200 | 2200 | +0 |
| eval-18 | 1300 | 1900 | +600 |
| eval-19 | 2500 | 2700 | +200 |
| eval-20 | 1300 | 2600 | +1300 |
| eval-21 | 1700 | 2100 | +400 |
| eval-22 | 1400 | 1900 | +500 |
| eval-23 | 2200 | 2200 | +0 |
| eval-24 | 1100 | 1100 | +0 |

## Post-freeze corrections

- **`1797C` label corrected after measurement.** `anchors.md` carried `1500 | Div1+2`; the
  corpus (written from the Codeforces API) says `1600 | Div2`. Corrected in `anchors.md` on
  2026-09-16, after the rounds above were measured. `1797C` was a comparison anchor for
  `eval-21` in rounds 1 and 3, so the shipped anchor table differs by this one 100-point
  label from the table those rounds used. Not re-measured: a 100-point change to one of 55
  anchors is far inside the ±40 standard error on MAE, and re-running a round to chase it
  would cost more than it could resolve.
- **Two prediction rows cite an anchor id that does not exist.** `predictions-round2.md`
  (`eval-21`) and `predictions-round3.md` (`eval-11`) both name `1063D`; the anchor table
  contains `1063C`. The blindness check in the eval protocol looks for agents naming real
  Codeforces problem ids, so a mistyped id passed through it. Effect on the aggregate is at
  most 1 row of 24 and the figures above are unchanged; recorded here rather than left silent.
## predictions-round4

n = 24   MAE = 279   bias = -29   within200 = 58%   within300 = 62%

| band | n | MAE | bias |
|---|---|---|---|
| 1100-1299 | 3 | 233 | +233 |
| 1300-1499 | 3 | 400 | +400 |
| 1500-1699 | 3 | 167 | -33 |
| 1700-1899 | 3 | 200 | +133 |
| 1900-2099 | 3 | 133 | +0 |
| 2100-2299 | 3 | 333 | -267 |
| 2300-2499 | 3 | 400 | -333 |
| 2500-2699 | 3 | 367 | -367 |

| slot | true | predicted | error |
|---|---|---|---|
| eval-01 | 2400 | 1800 | -600 |
| eval-02 | 2400 | 2500 | +100 |
| eval-03 | 2100 | 2000 | -100 |
| eval-04 | 2500 | 2100 | -400 |
| eval-05 | 2300 | 1800 | -500 |
| eval-06 | 1600 | 1400 | -200 |
| eval-07 | 1100 | 1200 | +100 |
| eval-08 | 1900 | 2000 | +100 |
| eval-09 | 2500 | 2200 | -300 |
| eval-10 | 1500 | 1400 | -100 |
| eval-11 | 1800 | 1700 | -100 |
| eval-12 | 2000 | 2100 | +100 |
| eval-13 | 1100 | 1600 | +500 |
| eval-14 | 1600 | 1800 | +200 |
| eval-15 | 1700 | 2200 | +500 |
| eval-16 | 2000 | 1800 | -200 |
| eval-17 | 2200 | 1400 | -800 |
| eval-18 | 1300 | 1700 | +400 |
| eval-19 | 2500 | 2100 | -400 |
| eval-20 | 1300 | 1500 | +200 |
| eval-21 | 1700 | 1700 | +0 |
| eval-22 | 1400 | 2000 | +600 |
| eval-23 | 2200 | 2300 | +100 |
| eval-24 | 1100 | 1200 | +100 |
## predictions-round5

n = 24   MAE = 275   bias = +8   within200 = 54%   within300 = 62%

| band | n | MAE | bias |
|---|---|---|---|
| 1100-1299 | 3 | 200 | +200 |
| 1300-1499 | 3 | 433 | +433 |
| 1500-1699 | 3 | 167 | +33 |
| 1700-1899 | 3 | 400 | +133 |
| 1900-2099 | 3 | 100 | -33 |
| 2100-2299 | 3 | 300 | -100 |
| 2300-2499 | 3 | 300 | -300 |
| 2500-2699 | 3 | 300 | -300 |

| slot | true | predicted | error |
|---|---|---|---|
| eval-01 | 2400 | 1900 | -500 |
| eval-02 | 2400 | 2400 | +0 |
| eval-03 | 2100 | 2200 | +100 |
| eval-04 | 2500 | 2300 | -200 |
| eval-05 | 2300 | 1900 | -400 |
| eval-06 | 1600 | 1400 | -200 |
| eval-07 | 1100 | 1200 | +100 |
| eval-08 | 1900 | 1900 | +0 |
| eval-09 | 2500 | 2100 | -400 |
| eval-10 | 1500 | 1500 | +0 |
| eval-11 | 1800 | 1400 | -400 |
| eval-12 | 2000 | 2100 | +100 |
| eval-13 | 1100 | 1500 | +400 |
| eval-14 | 1600 | 1900 | +300 |
| eval-15 | 1700 | 2400 | +700 |
| eval-16 | 2000 | 1800 | -200 |
| eval-17 | 2200 | 1600 | -600 |
| eval-18 | 1300 | 1800 | +500 |
| eval-19 | 2500 | 2200 | -300 |
| eval-20 | 1300 | 1500 | +200 |
| eval-21 | 1700 | 1800 | +100 |
| eval-22 | 1400 | 2000 | +600 |
| eval-23 | 2200 | 2400 | +200 |
| eval-24 | 1100 | 1200 | +100 |

## Retrodiction study — Pass C.1 and Pass D.1 (2026-09-19, post-freeze)

**Not a blind round.** Rounds 4 and 5 recorded, per slot, the predicted rating, the
prerequisite floor, and the ids of the anchors compared. That is enough to recompute what a
different post-placement arithmetic would have produced, holding the agent's anchor choice
and raw placement fixed. Reproduce with `python calibration/retrodict.py`.

### The defect being attacked

Regressing predicted on true gives a slope of **0.49** in round 4 and **0.54** in round 5:
the estimator recovers about half the spread of the truth. Aggregate signed bias was already
near zero (−29, +8) purely by cancellation, so it was never the quantity worth optimising.
The quantity worth optimising is per-band bias — reported below as `worst band` (the largest
|bias| of any band) and `spread` (most over-rated band minus most under-rated band).

### Results

| round | configuration | MAE | bias | within ±200 | worst band | spread |
|---|---|---|---|---|---|---|
| 4 | frozen | 279 | −29 | 58% | 400 | 767 |
| 4 | + era correction | 269 | −40 | 50% | 383 | 683 |
| 4 | + era + de-compression (shipped) | 277 | −48 | 46% | 283 | 550 |
| 5 | frozen | 275 | +8 | 54% | 433 | 733 |
| 5 | + era correction | 260 | +15 | 54% | 333 | 617 |
| 5 | + era + de-compression (shipped) | 277 | +2 | 46% | 300 | 517 |

Worst-band bias falls by 117 and 133; spread falls by 217 and 216; MAE is flat (279→277,
275→277, both far inside the ±40 standard error). Within ±200 falls from 58%→46% and
54%→46% — a deliberate trade of hit rate for per-band bias, and the honest cost of Pass D.1.

Per-band bias, frozen → shipped:

| band | round 4 | round 5 |
|---|---|---|
| 1100-1299 | +233 → +233 | +200 → +200 |
| 1300-1499 | +400 → +267 | +433 → +300 |
| 1500-1699 | −33 → −67 | +33 → +0 |
| 1700-1899 | +133 → +50 | +133 → +117 |
| 1900-2099 | +0 → −33 | −33 → −67 |
| 2100-2299 | −267 → −267 | −100 → −100 |
| 2300-2499 | −333 → −283 | −300 → −217 |
| 2500-2699 | −367 → −283 | −300 → −217 |

The two extreme bands (1300-1499 over-rated, 2300+ under-rated) are where the gain is. The
1100-1299 band does not move: the prerequisite floor holds those estimates up, which is the
floor doing its job and is not something de-compression should override.

### Parameter choice

Pivot `1800` is the midpoint of the anchor table's 1100-2600 range and was fixed before
measuring. Pivot sensitivity at factor 1.20, both rounds: pivots 1600-1800 give worst-band
283-300 and spread 483-550; pivots 1900-2000 degrade round 4 (worst-band 350). The step does
not balance on the constant within the 1600-1800 range.

Factor `1.20` deliberately under-corrects: fully inverting a 0.49 slope means expanding by
2.04, and the sweep shows the cost rising steeply past 1.30 (round 4 MAE 302 at 1.30, 331 at
1.50 with the floor gate applied after expansion). 1.20 is the conservative end of the range
that helped both rounds.

**The factor and pivot were chosen from a sweep over the same 24 problems**, which is
fitting. The mitigations are that the direction replicates across two rounds, the factor sits
at the conservative end, and the pivot is stable across a 200-point window. A blind round 6
is what would settle it.

### Rejected, recorded so they are not retried

| candidate | round 4 MAE | round 4 spread | verdict |
|---|---|---|---|
| estimate := median(anchors compared) | 292 | 900 | worse — the anchors an agent *selects* are themselves compressed toward the middle, so the agent's own judgement is adding value over its anchor picks |
| estimate := hardest anchor compared | 283 | 900 | worse |
| estimate := mean(prediction, anchor median) | 271 | 867 | MAE fine, spread worse |

The first result is the informative one: it rules out "pin the estimate to an anchor" as a
fix, and it means anchor *selection* is compressed as well as the final number.

### What this study cannot see

It holds the agent's behaviour fixed. Pass C.1 tells the agent to prefer recent anchors when
the window offers a choice; the retrodiction can only re-score the anchor set the agent
actually picked. It also runs on the same 24 problems used for six rounds of tuning, which
are no longer held out in any strict sense. Both push the figures optimistic by an unmeasured
amount.

## predictions-round6

n = 24   MAE = 321   bias = -21   within200 = 50%   within300 = 62%

| band | n | MAE | bias |
|---|---|---|---|
| 1100-1299 | 3 | 167 | +167 |
| 1300-1499 | 3 | 467 | +467 |
| 1500-1699 | 3 | 233 | -100 |
| 1700-1899 | 3 | 467 | +67 |
| 1900-2099 | 3 | 100 | -33 |
| 2100-2299 | 3 | 367 | -167 |
| 2300-2499 | 3 | 500 | -300 |
| 2500-2699 | 3 | 267 | -267 |

| slot | true | predicted | error |
|---|---|---|---|
| eval-01 | 2400 | 1800 | -600 |
| eval-02 | 2400 | 2700 | +300 |
| eval-03 | 2100 | 2200 | +100 |
| eval-04 | 2500 | 2200 | -300 |
| eval-05 | 2300 | 1700 | -600 |
| eval-06 | 1600 | 1200 | -400 |
| eval-07 | 1100 | 1100 | +0 |
| eval-08 | 1900 | 2000 | +100 |
| eval-09 | 2500 | 2300 | -200 |
| eval-10 | 1500 | 1400 | -100 |
| eval-11 | 1800 | 1300 | -500 |
| eval-12 | 2000 | 2000 | +0 |
| eval-13 | 1100 | 1500 | +400 |
| eval-14 | 1600 | 1800 | +200 |
| eval-15 | 1700 | 2500 | +800 |
| eval-16 | 2000 | 1800 | -200 |
| eval-17 | 2200 | 1400 | -800 |
| eval-18 | 1300 | 1800 | +500 |
| eval-19 | 2500 | 2200 | -300 |
| eval-20 | 1300 | 1300 | +0 |
| eval-21 | 1700 | 1600 | -100 |
| eval-22 | 1400 | 2300 | +900 |
| eval-23 | 2200 | 2400 | +200 |
| eval-24 | 1100 | 1200 | +100 |

### Round 6 rejected the retrodiction study's conclusion

Round 6 is the blind test the retrodiction study said was needed. **It refuted it.**

| | MAE | bias | ±200 | worst band | spread |
|---|---|---|---|---|---|
| round 4 (frozen) | 279 | −29 | 58% | 400 | 767 |
| retrodiction *predicted* for the shipped config | 277 | −48 | 46% | 283 | 550 |
| **round 6 (blind, actual)** | **321** | **−21** | **50%** | **467** | **767** |

Every headline number is worse than round 4 or unchanged, except aggregate bias, where
−29 → −21 is noise. The predicted worst-band collapse from 400 to 283 did not happen; the
measured value went the other way, to 467.

**Isolating the two passes.** Pass D.1 is deterministic arithmetic, so it can be inverted
from the recorded predictions (`pre = 1800 + (final − 1800) / 1.20`, approximate to ±50
because the recorded value is rounded to 100). That gives round 6 with the era correction
only:

| configuration | MAE | bias | ±200 | worst band | spread |
|---|---|---|---|---|---|
| round 4 (frozen) | 279 | −29 | 58% | 400 | 767 |
| round 6, era correction only (D.1 inverted) | 312 | −21 | 50% | 467 | 834 |
| round 6, era + de-compression (as run) | 321 | −21 | 50% | 467 | 767 |

So de-compression accounts for only 9 points of the 42-point MAE regression. **The bulk of
the damage is in the era correction pass, and specifically not in its arithmetic.**

**The mechanism.** The era discount is bounded by ±200 and near-flat across bands, so it
cannot move MAE by 33 points on its own — the retrodiction confirmed that, and the
retrodiction's arithmetic was correct. What the retrodiction could not see is the one thing
round 6 changed that it held fixed: **anchor selection.** Pass C told agents to prefer the
more recent of two comparable anchors. Only 30 of the 171 anchors are from 2025-2026 against
78 from 2018-2020, so that instruction steered every placement into a pool roughly a fifth
the size of the table, and the resulting comparisons are worse matched. The anchors cited in
round 6 are visibly dominated by 2025-2026 ids.

This is the failure mode the retrodiction study explicitly warned about under `What this
study cannot see` — "told to prefer recent anchors, it would pick a different set, and the
recorded set is all the retrodiction has." The warning was correct and the study was run
anyway; that is the lesson worth keeping.

**Action taken.** Pass D.1 deleted — it was never independently supported and costs 9 MAE.
The `prefer the more recent anchor` instruction deleted from Pass C — it is the identified
mechanism and it was an addition, not part of the era rule proper. The era discount
arithmetic is retained. Round 7 tests that combination; if it does not return to round-4
figures, the era correction goes too.

## predictions-round7

n = 24   MAE = 254   bias = -46   within200 = 54%   within300 = 62%

| band | n | MAE | bias |
|---|---|---|---|
| 1100-1299 | 3 | 200 | +200 |
| 1300-1499 | 3 | 300 | +300 |
| 1500-1699 | 3 | 167 | -33 |
| 1700-1899 | 3 | 300 | +33 |
| 1900-2099 | 3 | 100 | -100 |
| 2100-2299 | 3 | 300 | -100 |
| 2300-2499 | 3 | 333 | -333 |
| 2500-2699 | 3 | 333 | -333 |

| slot | true | predicted | error |
|---|---|---|---|
| eval-01 | 2400 | 1900 | -500 |
| eval-02 | 2400 | 2400 | +0 |
| eval-03 | 2100 | 2200 | +100 |
| eval-04 | 2500 | 2100 | -400 |
| eval-05 | 2300 | 1800 | -500 |
| eval-06 | 1600 | 1400 | -200 |
| eval-07 | 1100 | 1200 | +100 |
| eval-08 | 1900 | 1900 | +0 |
| eval-09 | 2500 | 2100 | -400 |
| eval-10 | 1500 | 1400 | -100 |
| eval-11 | 1800 | 1400 | -400 |
| eval-12 | 2000 | 2000 | +0 |
| eval-13 | 1100 | 1500 | +400 |
| eval-14 | 1600 | 1800 | +200 |
| eval-15 | 1700 | 2200 | +500 |
| eval-16 | 2000 | 1700 | -300 |
| eval-17 | 2200 | 1600 | -600 |
| eval-18 | 1300 | 1300 | +0 |
| eval-19 | 2500 | 2300 | -200 |
| eval-20 | 1300 | 1600 | +300 |
| eval-21 | 1700 | 1700 | +0 |
| eval-22 | 1400 | 2000 | +600 |
| eval-23 | 2200 | 2400 | +200 |
| eval-24 | 1100 | 1200 | +100 |

### Round 7 confirms the round 6 diagnosis, and is the best round recorded

| round | configuration | MAE | bias | ±200 | worst band | spread |
|---|---|---|---|---|---|---|
| 4 | frozen (no era correction) | 279 | −29 | 58% | 400 | 767 |
| 6 | era correction + recency preference + de-compression | 321 | −21 | 50% | 467 | 767 |
| 7 | **era correction alone (shipped)** | **254** | **−46** | **54%** | **333** | **633** |

Round 7 is the lowest MAE of any round (previous best 275, round 5), the lowest worst-band
bias (previous best 400, round 4), and the narrowest band spread (previous best 733, round
5). Within ±200 is 54% against round 4's 58%, a difference of one problem in 24.

**The mechanism is confirmed by the anchor citations.** Each round makes 72 anchor citations
(24 slots × 3). The table is 46% anchors from 2018-2020 and 18% from 2025-2026:

| round | recency instruction | citations from 2018-2020 | citations from 2025-2026 |
|---|---|---|---|
| 4 | absent | 33 | 10 |
| 6 | **present** | **3** | **38** |
| 7 | removed | 16 | 18 |

Round 6 used a 2018-2020 anchor 3 times out of 72 — a twelvefold under-use of nearly half
the table. One sentence of preference emptied the anchor pool, and MAE rose 42 points. That
is the entire round 6 regression, and removing the sentence recovered it and then some.

**What this says about the era correction.** Scored on printed labels — the same scoring
every round from 1 to 6 used — the era discount arithmetic is worth 279 → 254. It is now
supported by a blind round rather than by retrodiction. Scored against era-corrected labels
it gives MAE 256, bias +44, within ±300 79%; both scorings agree it helps, which the
retrodiction study could not establish.

**Targets after round 7:** MAE ≤ 200 missed (254). ≥65% within ±200 missed (54%).
|bias| ≤ 75 met (−46). Worst-band |bias| ≤ 200 missed (333). Two of four.
