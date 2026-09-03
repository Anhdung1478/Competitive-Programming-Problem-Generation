# Report: building a difficulty-recognition skill

Adding calibrated Codeforces-rating estimation to this repository's workflow.

**Fixed decisions:** CF rating scale (800-3500); full problem only, not per-subtask;
offline at runtime (network only while building the corpus); full 60-80 problem corpus,
agent-built.

---

## 1. Why

`Expected rating` is already a promised output with no method behind it.
`writing-editorials/SKILL.md:133` requires it; `:135` immediately permits skipping the
thinking (*"Do not block completion merely to ask for an estimated difficulty"*). The one
realized instance, `outputs/editorial.html:362`, is `2300 (ước lượng)` — no reasoning, no
confidence, nothing to audit.

Nothing upstream mentions difficulty: `problem-context.md` has no such field,
`subtask.md` carries percentage weights only, and `rate` is a test-size knob explicitly
documented as not a difficulty knob. Greenfield capability, one obvious consumer.

## 2. The premise correction

The original plan was to "see the rating calculation" of observed CF problems and derive
rules. **There is no such calculation.** CF ratings are fitted from contest performance —
roughly the rating at which in-contest solve probability is 50%. Tags, constraints, and
algorithms are not inputs to that fit.

This is a better position, not a worse one. Ratings are a free, abundant, public
**label**, which turns the job from reverse-engineering a hidden formula into supervised
calibration — and makes the skill *scoreable* instead of arguable. Every decision below
follows from that.

Two consequences that constrain everything:

- **±150 is the noise floor.** Ratings quantize to 100 and carry that much inherent noise.
  Targeting better than ±200 is chasing noise.
- **The target must be defined**, since our problems are subtask-scored and untimed while
  CF problems are all-or-nothing under time pressure. Adopt verbatim: *the rating the
  full-constraint version would receive as a single all-or-nothing problem in a modern
  Div1/Div2 round.*

## 3. Method: anchor-dominant, not additive

A pure additive rubric (base + deltas) fails three ways: with 8-9 deltas almost any number
is reachable; "uses digit DP" / "non-obvious state" / "huge constraints" is one fact
charged three times; and a model with an intuition just picks the deltas that reach it.

The fix rests on a real asymmetry: judging absolute magnitude on an unfamiliar scale is
something models are bad at, while comparing two concrete things is something they are
good at. **The method converts the first into the second.** Real problems of known rating
do the work; deltas are a small capped correction.

- **A — Extract features, no numbers.** From the validated `solution.cpp` and
  `problem-context.md`: prerequisite techniques; count of *independent* non-obvious
  observations; the single hardest step; implementation weight; whether constraints force
  the intended solution; whether the statement disguises a standard object. Separating
  extraction from scoring stops the model working backwards from a number it already has.
- **B — Prerequisite floor.** Hardest required technique → floor (two pointers ~1200, DSU
  ~1400, segment tree ~1700, digit DP ~1900, lazy/SOS ~2000, FFT ~2100, centroid
  decomposition ~2200, suffix automaton ~2300+). Not an estimate — a floor. It blocks the
  common failure of rating a digit-DP problem 1400 because the code looked clean. These
  numbers are a draft; Phase 5 calibrates them.
- **C — Anchor placement.** In the window `[floor, floor+600]`, compare against 2-3 real
  anchors: harder / similar / easier, one sentence each. **This produces the estimate and
  carries the accuracy.**
- **D — Capped adjustments, max ±300.** `+100` per extra independent insight (cap `+200`);
  `+100` disguise; `+100` genuinely heavy implementation; `-100..-200` textbook exercise;
  `-100` constraint leakage (`n ≤ 20` broadcasts "bitmask"). The cap is the point — deltas
  must not be able to override the anchors.
- **E — Gates.** Round to 100; clamp `[800, 3500]`; emit as `2300 ± 200 (ước lượng)`; emit
  `not estimable` if Step 1 did not validate the solution.

## 4. Feasibility — verified, not assumed

- CF API reachable, unauthenticated. **11,087 rated problems**; `contest.list` gives
  division via contest name and date via `startTimeSeconds`.
- Applying the full filter (Div1/Div2, post-2018, rated) leaves **4,696 candidates, with
  150-200 in every 200-point band** — stratified sampling of 10 per band is trivially
  satisfiable.
- Statements fetch by plain curl, HTTP 200, no bot wall.
- Ratings, tags, division, date are free. **Only the summarizing costs tokens**, which is
  what parallel subagents are for.

Anchor "intended solution" lines are derived by subagents from statement + tags, then
cross-checked against the known true rating; anchors where the two disagree are dropped,
not guessed at. This is legitimate for anchors — they carry their labels by design. The
eval set gets no such treatment.

## 5. Calibration

- **Phase 0 — baseline.** Measure today's free-form guess on ~15 held-out problems. Not
  go/no-go any more, but the number to beat: without it, "MAE 190" means nothing.
- **Phase 1 — corpus.** 60-80 problems, ~10 per band, 1100-2700, per the §4 filter.
- **Phase 2 — split.** 2/3 anchors, 1/3 held-out eval. Anchoring on your eval set makes
  the accuracy number simply false.
- **Phase 3 — blind eval.** Subagents that are **not told the true rating**, or the run
  measures rationalization rather than estimation.
- **Phase 4 — metrics.** MAE; ±200%; ±300%; **signed bias**; **error-by-band**. The last
  two matter because MAE hides both, and rubrics characteristically over-rate easy
  problems and under-rate hard ones. Targets: MAE ≤ 200, ≥65% within ±200, |bias| ≤ 75.
- **Phase 5 — tune.** Bias first (shift the floors as a block — one parameter, improves
  everything else free), then band compression. Max 2-3 iterations: a 25-problem eval set
  carries ±40 standard error, so smaller gains are indistinguishable from luck.
- **Phase 6 — freeze**, record final metrics in the skill, integrate.

## 6. Structure and integration

**Approach C — runtime split from build-time**, inside the skill folder:

- `references/` — loaded at runtime: `anchors.md`, `tag-floors.md`;
- `calibration/` — build-time only, never read at runtime: `corpus.md`, `eval-set.md`,
  `metrics.md`.

This is the minimum that satisfies two hard requirements simultaneously: the eval set must
survive (or measurability is lost permanently) and must stay out of the skill's context (or
the blind split is fake). A directory boundary enforces both and documents itself.

**Placement:** Step 9a, mandatory whenever Step 9 runs. Reads the Step 1 solution, writes
`outputs/difficulty.md` (estimate, band, prerequisites, anchors compared, deltas applied,
confidence) so a bad number can be audited later.

**Failure behaviour: degrade, don't block.** No validated solution ⇒ `not estimable` plus
reason in `outputs/difficulty.md`; Step 9 renders `chưa xác định` and continues. The
honest absence stays visible instead of being papered over with a guess.

**Edits:** `AGENTS.md` Purpose list gains Step 9a; `writing-editorials/SKILL.md:133-135`
consumes rather than re-estimates; `preference.yml` gains `step9a_difficulty` with a
light-weight override of one anchor instead of three.

## 7. Limits

- **The corpus is the whole cost.** 60-80 statements summarized; everything else is cheap.
- **The number is a comparability figure, not a prediction.** It tells a CF-literate reader
  what class of problem this is. It does not predict how your students will perform. The
  skill's output must say so, or it will be over-trusted.
- **Small eval set.** Do not report gains smaller than the ±40 error bar as gains.
- **Excluded by YAGNI:** per-subtask ratings (nothing consumes them today); VN tier mapping
  (no labeled data, so it would be a judgement call wearing a number's clothes); runtime CF
  lookup (slow, non-deterministic, fails mid-workflow).

## 8. Route

| Phase | Action | Artifact |
|---|---|---|
| 0 | Measure today's free-form guess on ~15 problems | baseline MAE — the number to beat |
| 1 | Select + fetch + summarize 60-80 problems | `calibration/corpus.md` |
| 2 | Split 2/3 – 1/3 | `references/anchors.md`, `calibration/eval-set.md` |
| 3 | Rubric v0 (Passes A-E), run blind on eval | predictions vs truth |
| 4 | MAE, ±200%, ±300%, bias, error-by-band | `calibration/metrics.md` |
| 5 | Tune bias, then band compression | rubric v1 |
| 6 | Freeze, write skill, integrate as Step 9a | skill + edits to `AGENTS.md`, `writing-editorials`, `preference.yml` |

**Stop condition:** MAE ≤ 200 with |bias| ≤ 75, or two consecutive iterations failing to
beat the ±40 error bar — whichever comes first.
