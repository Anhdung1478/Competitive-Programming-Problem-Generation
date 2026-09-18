# Session prompt — extend the calibration corpus to 200

Paste everything in the fenced block below into a fresh session, from the repository root.

---

```
Extend the `calculating-difficulties` calibration corpus from 80 problems to 200, then
re-measure. Use superpowers:subagent-driven-development. You are authorised to spawn
subagents for the summarisation and eval work.

Read these three, in this order, before doing anything:
  1. docs/superpowers/specs/2026-09-04-difficulty-recognition-design.md   (why, and the shape)
  2. docs/superpowers/plans/2026-09-16-calculating-difficulties.md        (how the first
     generation was built — you are not re-running it, you are extending its result)
  3. docs/superpowers/process/calculating-difficulties.md                 (run state; read
     the "Next generation" section last, it holds the decisions already made)

Work on a new branch off `feat/calculating-difficulties`. Do not merge and do not push
without asking.

## Where things stand

The skill is built, calibrated and frozen. Blind on 24 held-out problems:

    baseline (no rubric)   MAE 658   bias +625   25% within 200
    round 1                MAE 408   bias +192   29%
    round 2 (floors -200)  MAE 475   bias +250   33%    <- made it worse, reverted
    round 3 (shipped)      MAE 329   bias +179   50%

All three targets were missed (MAE <= 200, >= 65% within 200, |bias| <= 75), so the skill
ships a range rather than a +- interval and says so in `## Calibration status`.

## What you are changing, and what you must not

ADD 120 problems, 15 per band, to the frozen 80. All 120 become anchors.
Final state: **175 anchors / 24 eval / 1 excluded = 200**.

The eval set stays exactly the same 24 problems. This is deliberate and is not yours to
revisit: a different `n` produces an MAE that cannot be compared with the four numbers
above, and that comparison is the only evidence any tuning has ever helped. Your round 4
must land in the same table and answer one question — did more anchors help?

NEVER re-run `sample`. NEVER re-run `split`. NEVER edit an existing row of
`calibration/corpus.md`. Each would re-select or re-assign the frozen 80 and silently
invalidate every measurement in `metrics.md`. The extension is append-only. Both verbs
already refuse by design; do not work around the refusal.

`1181C` stays excluded — Codeforces serves its statement as a native PDF, verified.

## Why anchors, and not the floors

Do not start by tuning floors. The evidence says they are not the binding constraint:
round 2 shifted every floor by one constant and MAE got *worse*, while round 3 left the
floors alone and corrected Pass C's placement instruction, producing the best result.
Three symptoms say the anchor table is the limit — eval agents repeatedly widened Pass C's
window for want of a comparable anchor, 4 of the 8 floor rows have zero anchors, and 25 of
55 anchors are `none`-tagged.

## Shape of the work

1. Add an `extend` verb to `calibration/fetch-corpus.py`. It reads the existing corpus,
   samples 15 more per band from candidates not already present, appends them with role
   `anchor`, and refuses to run once the corpus has 200 rows. Use a fresh seed constant so
   the selection is reproducible and distinct from `SEED`. `PER_BAND` becomes 25. Every
   band has 320+ candidates, so this is not tight.
2. Update `cmd_check`: 200 rows, 175 anchors, 24 eval, 1 excluded, eval still 3 per band.
   Keep every existing assertion, including the label-integrity check that compares each
   `anchors.md` row's rating and div against the corpus. Do not weaken any guard to make a
   check pass. Prove each changed assertion still fires by perturbing a scratchpad copy.
3. `fetch` the 120 new statements (it skips what is cached). Expect a few to fail; retry.
   Any that serve a PDF instead of HTML get excluded the same way `1181C` was — by a
   constant, never by editing the corpus.
4. Summarise the 120 new anchors with ~12 subagents, same row format and same rules as
   before: one tag from `tag-floors.md`'s left column copied verbatim, `none` when nothing
   genuinely applies, one sentence under 25 words naming the actual mechanism, and
   `<!-- unsure -->` appended rather than bending a summary to fit a rating. Have each
   batch write to its own file; do not let 120 rows through your own context.
5. Assemble `references/anchors.md` to 175 rows, sorted by rating then id. `check` must
   pass, which now also proves no eval id leaked in and no label disagrees with the corpus.
6. Run round 4 on the same 24 eval slots, blind, 5 subagents, `SKILL.md` unchanged. Score
   it and APPEND to `metrics.md` — never overwrite a round.
7. Report round 4 beside the four existing numbers. A 24-problem eval carries about +-40
   standard error: a move smaller than that is not an improvement, and you must say so
   rather than describe it as one.

## The one question to rule on

Whether to extend `tag-floors.md`'s 8-row vocabulary. The evidence: 45% of anchors match
no row; untagged anchors span 1200-2600, so `none` does not mean easy; a `none` problem
floors at 1100 and can then reach at most ~2200 (the "Known ceiling" documented in
`SKILL.md`); and the two largest under-estimates in round 3 were both floor-1100 problems.
Recurring gaps the summarisers named: interactive query design, constructive/ad-hoc
casework, number theory, binary lifting, small-to-large merging.

Extending it changes floors, which changes measurements, so it cannot be a patch on top of
round 4 — it needs its own round with its own recorded numbers. Decide with the evidence,
record the ruling and what it costs if wrong, and do not quietly do both at once: if you
change the vocabulary and add 120 anchors in the same round you will not know which one
moved the number.

## Honesty requirements

These are the point of the project, not ceremony.

- `SKILL.md` must never name `calibration/`. The runtime half cannot see the build-time
  half or the blind split is fake.
- Eval agents get the statement and the runtime references, never a true rating, never the
  calibration directory.
- If round 4 does not beat 329 by more than 40, say that plainly and leave 329 shipped.
  A negative result is a finding. Round 2 is already recorded as one.
- Whatever you ship, `## Calibration status` states the measured numbers, the eval `n` and
  the date, and claims no interval it did not earn.
```

---

## Notes for whoever runs it

- Rough cost: ~12 summariser agents + 5 eval agents + implementer/reviewer pairs. The
  fetching is curl only and costs no model tokens.
- The first generation's full audit trail — ledger, 18 rulings, every agent report — is in
  `.superpowers/sdd/2026-09-16-calculating-difficulties/`. It is gitignored, so it exists
  only in the working tree it was produced in.
- If round 4 does beat 329 materially, the next lever after that is the `tag-floors.md`
  vocabulary, then a larger eval set — but a larger eval set breaks comparability with
  everything recorded so far, so it should start a fresh generation with its own baseline.
