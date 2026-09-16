# Report: building a difficulty-recognition skill

Adding calibrated Codeforces-rating estimation to this pack as one new skill,
`calculating-difficulties`.

**Fixed decisions:** CF rating scale (800-3500); full problem only, not per-subtask;
offline at runtime (network only while building the corpus); full 60-80 problem corpus,
agent-built; delivered as a **new skill** rather than as method spread through the
existing ones; all of its data under that skill's own directory, never at the plugin root.

*Revised 2026-09-16: retargeted from a single problem workspace to this skill pack.*

---

## 1. Why

`Expected rating` is already a promised output with no method behind it.
`skills/writing-editorials/SKILL.md:177` requires the field; `:180` immediately permits
skipping the thinking (*"Do not block completion merely to ask for an estimated
difficulty"*); `:104` fixes the label, so the row is reader-facing on every editorial the
pack produces. Nothing here says how the number is reached — no skill computes it, no
reference file lists a comparison, nothing records the reasoning. The number arrives bare:
no confidence, nothing to audit.

This is a pack, not a workspace, and that changes the shape of the defect rather than its
size. It is not one bad number in one editorial; it is one unmethodical number per
downstream problem repository, realized in an artifact this repository never sees and
cannot inspect afterwards. The fix has to travel with the pack.

Nothing upstream supplies it either. `shaping-problems` settles the numbers that look
adjacent — originality, the `N` that separates the intended solution from the naive one,
OI vs ICPC, the subtask ladder's weights — and none of them is a difficulty. The
`source/problem-context.md` template in the README has no such field. `rate` in
`generating-tests` is a test-scale knob — the fraction of a subtask's bound a generated
test reaches — not a difficulty knob. Greenfield capability, one obvious consumer.

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

- **A — Extract features, no numbers.** From the validated `source/solution.cpp` and
  `source/problem-context.md`: prerequisite techniques; count of *independent* non-obvious
  observations; the single hardest step; implementation weight; whether constraints force
  the intended solution; whether the statement disguises a standard object. Separating
  extraction from scoring stops the model working backwards from a number it already has.
- **B — Prerequisite floor.** Hardest required technique → floor (two pointers ~1200, DSU
  ~1400, segment tree ~1700, digit DP ~1900, lazy/SOS ~2000, FFT ~2100, centroid
  decomposition ~2200, suffix automaton ~2300+), read from `references/tag-floors.md`. Not
  an estimate — a floor. It blocks the common failure of rating a digit-DP problem 1400
  because the code looked clean. These numbers are a draft; Phase 5 calibrates them.
- **C — Anchor placement.** In the window `[floor, floor+600]`, compare against 2-3 real
  anchors from `references/anchors.md`: harder / similar / easier, one sentence each.
  **This produces the estimate and carries the accuracy.**
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

All of this is **build-time work, run once, in this repository**. A problem repository
that installs the pack never touches the network for an estimate; it reads the anchors
that shipped inside the skill.

Anchor "intended solution" lines are derived by subagents from statement + tags, then
cross-checked against the known true rating; anchors where the two disagree are dropped,
not guessed at. This is legitimate for anchors — they carry their labels by design. The
eval set gets no such treatment.

## 5. Calibration

Phases 0-5 run here, in the pack, never in a problem repository.

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
- **Phase 6 — freeze and ship.** Record the final metrics in `calibration/metrics.md` and
  their headline in `SKILL.md`, then **write the new skill** and wire it in by pointer (§6).

## 6. Structure and integration

**The deliverable is one new skill, not an edit to several.** Everything the method needs
— prose, floors, anchors, and the evidence behind them — lives in
`skills/calculating-difficulties/`. Nothing that exists today learns to estimate anything;
`writing-editorials` in fact loses a responsibility rather than gaining one.

`.claude-plugin/plugin.json` already declares `"skills": ["./skills"]`, so a new directory
registers itself — no manifest edit. The skill is invocable on its own as
`cp-problem-generation:calculating-difficulties`, exactly like the other single-step
skills, and as Step 8a from `creating-problems`.

**Approach C — runtime split from build-time**, both halves inside the skill folder:

```
skills/calculating-difficulties/
├── SKILL.md                 # Passes A-E, output contract, failure mode, headline metrics
├── references/              # runtime; loaded only where SKILL.md points at them
│   ├── tag-floors.md
│   └── anchors.md
└── calibration/             # build-time only; never read at runtime
    ├── corpus.md
    ├── eval-set.md
    └── metrics.md
```

Both directories sit **under the skill, not at the plugin root**. The root is
`preference.yml`, `.claude-plugin/`, `skills/`, `mcp-server/`, `docs/` — a top-level
`references/` or `calibration/` there would read as pack-wide data, get opened by someone
working on an unrelated skill, and lose the one property the split exists for. Under the
skill, ownership is obvious and the boundary documents itself: `references/` is what
`SKILL.md` names, `calibration/` is what nothing at runtime names. It also matches what
the pack already does — `writing-editorials/references/themes/`,
`polygon-statement/references/theme.md`, `generating-tests/references/`.

This is the minimum that satisfies two hard requirements simultaneously: the eval set must
survive (or measurability is lost permanently) and must stay out of the skill's context (or
the blind split is fake).

`calibration/` ships with the pack — 60-80 plain-text summaries, cheap — because a
measurement that cannot be repeated on the next tuning pass is not a measurement. If its
weight ever becomes a real cost it moves to `docs/`; it does not get deleted.

**Frontmatter.** Skills here are found by their `description`, so it carries the triggers a
user actually types, Vietnamese included: *estimate difficulty, expected rating, Codeforces
rating, độ khó, ước lượng rating, bài này rating bao nhiêu*. It states plainly that the
skill does not produce per-subtask ratings and does not map to VN tiers, so it is not
invoked for work it cannot do.

**Placement:** Step 8a, mandatory whenever Step 8 runs, and directly invocable at any point
after Step 1. It reads the Step 1 solution and writes `outputs/difficulty.md` **in the
problem repository** (estimate, band, prerequisites, anchors compared, deltas applied,
confidence) so a bad number can be audited later. The pack writes nothing into itself at
runtime. `yagni-principles` is always-on and binds here too: that file is the short record
above, not an essay.

**Failure behaviour: degrade, don't block.** No validated solution ⇒ `not estimable` plus
reason in `outputs/difficulty.md`; Step 8 renders `chưa xác định` and continues. The
honest absence stays visible instead of being papered over with a guess.

**Edits to existing files — pointer-sized, by design.** Four of them:

| File | Edit |
|---|---|
| `skills/creating-problems/SKILL.md` | one Step 8a section, one line in the optional-steps list, one row in *Am I the right skill?* |
| `skills/writing-editorials/SKILL.md` | `:177`/`:180` stop saying "estimate it if you responsibly can" and start saying "copy `outputs/difficulty.md` when it exists, `chưa xác định` when it does not" |
| `preference.yml` | one `step8a_difficulty` block: `light-weight` compares one anchor instead of three; `never_relaxed` keeps the `not estimable` rule, since a guess must never stand in for a missing validation |
| `README.md` | one row in the skills table, one line in the Layout block |

That is the whole integration surface. No generator skill, no `uploading-to-polygon` (a
difficulty is not part of a Polygon package), and no new `problem.json` field — that file
is the interface between steps that *act* on numbers, and nothing acts on this one.

## 7. Limits

- **The corpus is the whole cost.** 60-80 statements summarized; everything else is cheap.
- **The number is a comparability figure, not a prediction.** It tells a CF-literate reader
  what class of problem this is. It does not predict how your students will perform. The
  skill's output must say so, or it will be over-trusted.
- **Small eval set.** Do not report gains smaller than the ±40 error bar as gains.
- **Anchors age.** They are frozen text inside the pack while CF's distribution drifts.
  Re-running Phases 1-4 on a fresh sample is the maintenance task, and `calibration/`
  exists so that costs one pass instead of a rebuild.
- **Excluded by YAGNI:** per-subtask ratings (nothing consumes them today); VN tier mapping
  (no labeled data, so it would be a judgement call wearing a number's clothes); runtime CF
  lookup (slow, non-deterministic, fails mid-workflow); a `difficulty` field in
  `outputs/problem.json` (the editorial reads the markdown, and a second home is drift).

## 8. Route

| Phase | Action | Artifact |
|---|---|---|
| 0 | Measure today's free-form guess on ~15 problems | baseline MAE — the number to beat |
| 1 | Select + fetch + summarize 60-80 problems | `calibration/corpus.md` |
| 2 | Split 2/3 – 1/3 | `references/anchors.md`, `calibration/eval-set.md` |
| 3 | Rubric v0 (Passes A-E), run blind on eval | predictions vs truth |
| 4 | MAE, ±200%, ±300%, bias, error-by-band | `calibration/metrics.md` |
| 5 | Tune bias, then band compression | rubric v1, `references/tag-floors.md` |
| 6 | Freeze; write `skills/calculating-difficulties/`; wire Step 8a | the new skill + the four pointer edits in §6 |

Paths in phases 1-5 are relative to `skills/calculating-difficulties/`.

**Stop condition:** MAE ≤ 200 with |bias| ≤ 75, or two consecutive iterations failing to
beat the ±40 error bar — whichever comes first.
