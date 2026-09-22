# calculating-difficulties — review of 2026-09-22

Review of the skill as it stood after round 9A (commit "calibration: round 8-9 record and
held-out eval set 2"). This document is the authority the fix plan argues from.

## Verdict

The skill is in good shape: blind-measured on a held-out set, shipped under pre-registered
rules, honest about its limits. The findings below are consistency defects, statement-of-limits
gaps, and prompt-craft issues. None of them changes a pass's arithmetic; the method changes the
review proposed are pre-registered for round 10, not shipped.

## Findings applied by the fix plan

| # | finding | evidence | fix |
|---|---|---|---|
| D1 | `README.md` row 8a is stale: MAE 254, n=24, unaided 658 | `SKILL.md` says 210, 48, 277 | README carries no figure; it points at `Calibration status` |
| D2 | Pass C says the recency preference "cost 33 points of MAE"; `anchors.md` and `fetch-corpus.py` say 42 | metrics.md: round 6 MAE 321 against round 4's 279 | correct to 42 |
| D3 | Five `anchors.md` rows (`2109C1`, `1129A2`, `2196C1`, `1063C`, `1783F`) have a seventh cell holding `<!-- unsure -->` | table misaligns in any renderer | move the marker inside the sixth cell |
| D4 | The gate requires that the solution "passed the complete Step 1 audit", but `validate-solution` writes no file, so a standalone Step 8a has nothing to check | validate-solution SKILL.md reports PASS/FAIL to the conversation only | the gate lists what counts as proof: a Step 1 `PASS` in this session, the user's word, or a validated full-scope AC in the manifest |
| L1 | The effective placement range is about 1100–2900, not 800–3500 | anchors span 1100–2600; lowest floor 1100; window starts at the floor; the top three floor rows have no anchor | state the range; add a `Confidence` clause for floor ≥ 2100 or placement ≥ 2500 |
| C1 | Five copies of the round figures (README, Pass E step 4, template coverage sentence, template `Source:` line, `Calibration status`); one already stale | D1 | `Calibration status` is the single home; every other place points there |
| C2 | `Calibration status` is ~450 words of build-time narrative; the metrics log already complained once that 40% of the prompt was history | word count | cut to the table plus one paragraph; the record stays in `calibration/metrics.md` |
| C3 | No overview: the agent reads ~2,900 words with no map | — | a seven-row "procedure at a glance" table |
| C4 | The round-6 story in Pass C is a paragraph of narrative | — | three sentences |
| C5 | Wording edits made after 9A (the Pass C.1 "level shift" paragraph, the Pass B 1100-cap paragraph) are not in the drift record | metrics.md "Configuration drift since round 7" is the last drift entry | a "Configuration drift since round 9A" table listing every runtime-wording change from this plan and those two |

## Findings deferred to round 10 (pre-registered, not applied)

| # | finding | evidence | why not applied now |
|---|---|---|---|
| R1 | The `two pointers, prefix sums, sorting + greedy` floor of 1200 is not a lower bound | anchors `1923B` and `2245B` carry the tag at rating 1100; round 9 floored `fresh-04` and `fresh-05` (true 1100) at 1200; round 8 floored `eval-14` (true 1600) at 1700 | a floor change moves numbers; the skill's rule is that such changes ship only on a blind round |
| R2 | The era correction has never been measured in isolation on held-out data | round 7 led round 4 by 25 inside ±40; round 9 ran no arm without C.1 | ablation arm |
| R3 | Widening is used to reach for hard anchors: 30 of 142 citations in 9A sat outside the base window; the worst miss (`fresh-11`, +800) took all three anchors from the top of the widened window | predictions-round9a.md | a structural rule (at least one base-window anchor) changes selection; blind arm |
| R4 | The floor-1100 ceiling binds: 4 of 19 floor-1100 problems in 9A had true ratings 2300–2400 and were estimated 1700–2200; whole-table selection (9B) removed the ceiling but was net worse | metrics.md round 9 ruling | a conditional widening keyed to Pass A evidence is a candidate for a later round; not pre-registered here because round 10 already carries four arms |

## Out of scope

- No version bump; the user decides.
- `docs/superpowers/process/calculating-difficulties.md` and the other process docs keep their historical figures; they are run records, not the skill's current claim.
- `fetch-corpus.py` is not generalised to build a third eval set; round 10 names that as its prerequisite.
