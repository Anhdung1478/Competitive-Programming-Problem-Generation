# Design: enforcement tools, a fresh-eyes audit, and a solving-problems skill

Date: 2026-09-04
Status: approved (design); implementation plan to follow

Addresses findings 8, 1, 7, 9, 3, 4, 5 from the pack review. Findings 2 and 6 are
explicitly out of scope (see §9).

---

## 1. Problem

The pack's rigor is prose. Every cross-artifact rule is a bullet in
`skills/creating-problems/SKILL.md` that the same agent, in the same context, checks
against artifacts it just wrote. Three consequences:

- **Constraints exist in five hand-written copies.** `problem-context.md`,
  `problem.json`, `statement.txt`, `validator.cpp`, `generator-config.md` and
  `gentest.cpp` each restate the bounds. The cross-artifact gate asks the author to
  confirm they agree.
- **Nothing reads the package with fresh eyes.** Statement ambiguity is precisely the
  defect the author cannot see, because the author knows what the sentence was meant
  to say.
- **The OI ladder's central claim is never tested.** `shaping-problems` proves each
  rung needs a distinct algorithm, then Step 4 makes scoped ACs opt-in, so no rung is
  ever run against a stronger rung's tests.

Python tools are now permitted, which makes mechanical enforcement available for the
first time.

## 2. Approach

Enforce drift **by medium**:

- **C++ artifacts get codegen.** A generated `outputs/constraints.h`, included by the
  validator and the generator, makes a bound a single definition. A stale bound stops
  being a review item and becomes a missing symbol.
- **Prose artifacts get detection.** `statement.txt`, `generator-config.md`,
  `test-script.txt` and `tex-statement.tex` cannot include a header, so a checker
  diffs their numbers against `problem.json` and reports.

Rejected: codegen alone (leaves the statement free to contradict the validator — the
one drift that reaches contestants) and detection alone (a linter over free text, with
false positives on every number that is not a bound, and no power to prevent anything).

## 3. Tools layer

Location: `tools/` at the pack root, because several tools are cross-step and
per-skill ownership would force duplication.

**Each tool is a standalone script with no intra-package imports**, invoked as:

```
python "$PLUGIN_ROOT/tools/<name>.py" <args>
```

from any working directory. This is a deliberate departure from the reference pack,
whose `python3 -m tools.X` form requires cwd = plugin root on every call — a
requirement that breaks as soon as the pack is installed into the marketplace cache.

`PLUGIN_ROOT` is the directory containing `preference.yml` and `tools/`. It is derived
once from the skill base directory Claude Code prints when a skill loads, and the
derivation is stated in the umbrella's Bootstrap section. On Windows the interpreter is
`python`, not `python3`.

### 3.1 The five tools

| Tool | Finding | Input | Output / effect |
|---|---|---|---|
| `gen_constraints_header.py` | 1 | `outputs/problem.json` | Writes `outputs/constraints.h`: one `long long` const per global bound, plus per-subtask narrowed bounds. Idempotent; regenerated whenever `problem.json` changes |
| `drift_check.py` | 1 | `problem.json` + prose artifacts | Reports numeric and subtask/points disagreements. Never edits an artifact |
| `scan_solutions.py` | 4, 9 | `outputs/solution/*.cpp` + `problem.json` | Parses header metadata blocks, enforces required tags per verdict class, checks every `@scope` names a real subtask id, checks at least one full-scope AC exists. Also emits the **subtask containment relation** derived from `subtasks[].bounds` (§5.4), so the one implementation serves both `generating-tests` and `review_checks.py`. Emits JSON for downstream steps |
| `bootstrap_testlib.py` | 7 | `CP_TESTLIB` env, else network | Prints a path to a directory containing `testlib.h`; clones a cached checkout only if needed. Never copies `testlib.h` into a problem folder |
| `review_checks.py` | 3 | the whole package | The mechanical half of the audit (§6). Exit non-zero on any finding |

### 3.2 Tests

`tools/tests/` with a `mini` package fixture, run by `python -m unittest discover`.

These tools are correctness infrastructure: a wrong `constraints.h` silently corrupts
every downstream bound, and a permissive `review_checks.py` reports a clean package
that is not clean. They are therefore tested despite `yagni-principles`, which governs
generated artifacts rather than the pack's own enforcement code.

## 4. `constraints.h` contract

`problem.json` already carries what is needed — `constraints[]` with integer `min` and
`max`, and `subtasks[].bounds` narrowing a global constraint by its id — so no schema
change is required. The existing rule that `min`/`max` are JSON integers becomes
load-bearing: these values are emitted as C++ `long long`.

Generated shape, for a constraint `n` with a subtask narrowing:

```cpp
// Generated from outputs/problem.json. Do not edit.
static const long long N_MIN = 1;
static const long long N_MAX = 200000;
static const long long N_MAX_G1 = 1000;
```

**`outputs/validator.cpp` and `outputs/gentest.cpp` must include it and must contain no
literal numeric bound.** A literal bound in either file is a defect the audit reports.
Bounds that `problem-context.md` never stated remain absent from `problem.json`, and so
absent from the header — `shaping-problems` already forbids guessing them, and the
header inherits that rule rather than inventing a default.

## 5. Skill changes

### 5.1 New skill: `solving-problems`

Step 4's entire contract moves out of the umbrella into
`skills/solving-problems/SKILL.md`: suite naming, WA/TLE plausibility rules, the
independent-design rule, per-candidate validation, and the scoped-AC requirement of
§5.4.

**`outputs/solution/manifest.md` is removed.** Per-solution facts live in a doc-comment
block at the head of each source, where they cannot drift from the file they describe
and are machine-readable:

```cpp
/**
 * @tag        wrong-answer
 * @scope      full
 * @algorithm  Sort by value/weight, pack greedily.
 * @complexity O(n log n)
 * @why-wrong  Optimal knapsack is not greedy on density.
 * @kill-profile  w_i near W/2, values anti-correlated
 */
```

Required tags: `@tag`, `@scope`, `@algorithm`, `@complexity` on every source;
`@why-wrong` and `@kill-profile` additionally on every WA and TLE. `@scope` is `full`
or a subtask id from `problem.json`. `scan_solutions.py` enforces this.

Consumers move from the manifest to the scanner's output: Step 5b's fallback AC, Step
6's solution-kill matrix, Step 7's per-scope validation, Step 8's editorial source, and
the cross-artifact gate.

The provisional time-limit policy needs no new home. `shaping-problems` already writes a
proposed `limits.time_ms_published` into `problem.json`, so Step 4 classifies TLE against
that value and asks the user when it is absent, rather than choosing one and recording it
in a document.

### 5.2 New skill: `reviewing-problems` (Step 8a)

Runs after Step 8, before the optional upload. Two halves:

- **Mechanical** — `review_checks.py`, run inline. Fix findings, re-run.
- **Judgement** — dispatched to a **subagent with fresh context** that reads only
  `source/` and `outputs/`, and is given no account of how the package was built.
  Checks: statement ambiguity, assumed definitions, unproven `@algorithm` claims,
  checker/validator versus the stated output format, and declared bounds no test
  reaches.

The subagent is the point of the step: an inline audit is performed by an agent that
already knows what every sentence was meant to mean, which is exactly the blind spot.
This is a narrow, stated exception to the umbrella's "you run every step in this
conversation" rule, and applies to the judgement half only.

The step carries the same single hard stop as the rest of the pack: an unresolvable
HIGH statement ambiguity — two readings that change the answers, with nobody having
picked one — stops the workflow rather than being resolved by a guess.

### 5.3 `creating-problems` shrinks

Keeps: step order, the gates and loop-backs, the source-of-truth rules, the rigor-profile
selection, the C++ I/O convention, the mandatory ambiguity gate, Step 5b, and the
cross-artifact gate. Loses: Step 4's contract (to `solving-problems`), and the
restatements of Step 3/5/6/7 rules that already live in the step skills — each replaced
by a pointer, so every rule has exactly one home.

Roughly a dozen cross-artifact gate bullets become mechanical and collapse into a
`review_checks.py` invocation. The bullets that remain are the judgement ones.

Bootstrap gains the `PLUGIN_ROOT` derivation and the tool invocation form.

### 5.4 Ladder separation (finding 4)

When `problem.json` has `"format": "oi"` and two or more subtasks, one
`ac-subtask-<id>-<slug>.cpp` is **required for every rung whose scope is narrower than
the full constraint domain** — no longer opt-in. A rung that narrows nothing
(`bounds: {}`) is already covered by the mandatory `ac-full-<slug>.cpp` and gets no
scoped duplicate: a two-rung ladder therefore needs one `ac-full` plus one
`ac-subtask-g1`, not two scoped files. ICPC and single-group OI are unaffected. Under the `light-weight` profile the suite stays
full-scope only, so this requirement is a `heavy`-profile rule and `preference.yml`
records it as such.

Responsibility splits across three steps, because a rung's separation cannot be checked
before tests exist:

- **`solving-problems`** authors the scoped ACs and declares each `@scope`.
- **`generating-tests`** verifies it, under a containment precondition. For two
  subtasks `A` and `B` where **`A`'s scope covers `B`'s scope** (`scope(B)` is
  contained in `scope(A)`), `B`'s scoped AC must score **zero points on `A`**. Where
  neither scope contains the other, no such requirement exists and none is checked —
  two incomparable rungs (tree-shaped inputs versus general inputs at a small bound,
  say) may legitimately share an algorithm on their overlap.

  Containment is **computed, not declared**: for every constraint id, `B`'s interval
  must lie inside `A`'s, strictly for at least one. It is derived from
  `subtasks[].bounds`, and deliberately **not** from `depends_on`, which
  `shaping-problems` defines as Polygon scoring order and explicitly not a superset
  claim. A subtask with `bounds: {}` covers every rung that narrows anything.

  When `B`'s AC does score points on a covering `A`, diagnose in this order: first
  `A`'s test group, which is failing to reach outside `B`'s domain hard enough — fixable
  in Step 7; then, only if `A`'s tests genuinely max `A`'s bounds and `B`'s algorithm
  still clears them, the ladder itself has collapsed, which is a Step 0 finding to
  report by reopening shaping rather than by patching tests.
- **`reviewing-problems`** signs it off.

### 5.5 Stress doctrine (finding 5)

No tool. A generic stress harness cannot work at Step 4: `gentest.cpp` does not exist
until Step 7, so the tool would have nothing to generate inputs with.

`solving-problems` replaces "brute-force/differential tests **when feasible**" with:

- a round count from `preference.yml` (`stress.rounds`, default 80);
- a sizing rule — tiny `N` (readable diffs) **and** the largest `N` at which the oracle
  still finishes. An `O(N^2)` oracle against an `O(N log N)` intended solution means
  also stressing `N` around 1000-2000, not only `N <= 20`. There is no global cap; the
  band is per problem;
- the pitfall: a comparison that looks only at exit code and elapsed time hides every
  wrong answer. Compare captured stdout against the oracle's output, normalizing
  line endings on Windows.

### 5.6 Shared testlib reference (finding 7)

New `skills/creating-problems/references/testlib.md`, pointed at by `polygon-checker`,
`polygon-validator` and `generating-tests` rather than duplicated into each:

- `bootstrap_testlib.py` and the `CP_TESTLIB` override;
- the canonical compile line for a testlib program;
- the rule that `testlib.h` is never copied into a problem folder;
- a documented local fallback: if a testlib binary segfaults at `-O2` on the local
  toolchain, rebuild it at `-O0` for local validation. This is recorded as a local
  workaround, not a default — Polygon builds optimized, so making `-O0` the pack's
  default would validate a binary unlike the one Polygon runs.

### 5.7 `preference.yml`

Adds `stress.rounds: 80`, and records the scoped-AC requirement of §5.4 as relaxed
under `light-weight` (the `heavy` profile keeps it).

## 6. `review_checks.py` scope

Mechanical, exit non-zero on any finding:

- every artifact the workflow declares complete exists;
- `scan_solutions.py` passes; every scanned source compiles as GNU C++17;
- `outputs/validator.cpp` accepts every generated test and every example input, and
  rejects a representative set of invalid mutations;
- `outputs/checker.cpp` exists and accepts each example `.out` against itself;
- subtask points sum to `polygon.total_points`;
- every option used in `test-script.txt` exists in `outputs/gentest.cpp`;
- `validator.cpp` and `gentest.cpp` include `constraints.h` and carry no literal bound;
- `drift_check.py` is clean.

Deliberately excluded: anything requiring judgement. Those are the subagent's.

## 7. README (finding 8)

- Remove the one-line installer section and the `install.sh` / `install.ps1` entries
  from the Layout tree; both files are already deleted and the pack is
  marketplace-installed. The documented paths become `/plugin marketplace add` and the
  `~/.claude/skills/` clone.
- Add `tools/`, the two new skills, and a Checks section covering
  `claude plugin validate` and the unittest suite.

## 8. Data flow

```
problem-context.md ──► shaping-problems ──► problem.json
                                              │
                        ┌─────────────────────┼──────────────────────┐
                        ▼                     ▼                      ▼
            gen_constraints_header      statement.txt         drift_check
                        │                (prose)                    ▲
                        ▼                     └────────────────────-┘
                  constraints.h
                        │
            ┌───────────┴───────────┐
            ▼                       ▼
      validator.cpp            gentest.cpp
                                    │
   solution/*.cpp ──► scan_solutions ──► kill matrix, per-scope runs
                                    │
                                    ▼
                            review_checks ──► reviewing-problems
                                                  │
                                                  ▼
                                        fresh-context subagent
```

## 9. Out of scope

- **Finding 2** (`package_status.py`, resume-from-disk). Deferred; the pipeline still
  resumes from conversation state.
- **Finding 6** beyond `stress.rounds`. The 100-test default and the `rate` split stay
  hardcoded in `generating-tests` prose.

## 10. Success criteria

- A bound changed in `problem.json` and not regenerated makes the validator or the
  generator fail to compile.
- A bound that disagrees between `problem.json` and `statement.txt` is reported by
  `drift_check.py`.
- A missing or malformed solution header block, or a `@scope` naming no real subtask, is
  reported by `scan_solutions.py`.
- On a `heavy` OI package with two or more rungs, every rung below the full-constraint
  rung has a scoped AC, and for every pair where one rung's scope contains another's,
  the narrower rung's AC is demonstrated to score zero points on the covering rung.
- The judgement audit is performed by an agent with no authoring context.
- `python -m unittest discover` passes over `tools/tests/`.
- `claude plugin validate . --strict` passes.
- The README documents no file that is absent from the repository.
