# Metrics

Every round is appended, never overwritten: the comparison between rounds is the
only evidence that tuning helped. A 24-problem eval set carries roughly ±40
standard error — a movement smaller than that is not an improvement.

Targets: MAE ≤ 200, ≥65% within ±200, |bias| ≤ 75.

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
