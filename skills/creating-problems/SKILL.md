---
name: creating-problems
description: >
  Prepare a competitive-programming problem end to end — an idea and an
  intended solution to a Codeforces Polygon package: statement, checker,
  solution suite, input validator, generator config, test script, generator,
  Vietnamese editorial. Triggers on create a problem, prepare a
  problem, set a problem, chuẩn bị đề, sinh test, generate tests, take this
  idea to a Polygon package, run the workflow, execute the problem-preparation
  workflow. Umbrella over validate-solution, polygon-statement,
  polygon-checker, generator-config, generating-tests,
  polygon-validator, writing-editorials, tex-statement and the generator-*
  skills; it owns the step order and the gates between them. Use this whenever
  the request spans two or more of those steps, or when the user hands over
  problem sources and wants the package built.
---

# Creating problems

Sequence the whole preparation pipeline, from `source/` to a Polygon-ready
`outputs/`. **You run every step in this conversation.** When a step starts,
read that step's skill and follow it. Do not preload every skill up front.

This skill owns the **order, the gates, and the loop-backs**. The work of each
step lives in the step's own skill.

## Am I the right skill?

If the request is only one step, use that step's skill directly. If it names two
or more — "statement + tests", "sinh test", "idea to package" — stay here.

| If it's really about | Use |
|---|---|
| Only the numbers — originality, `N`, the subtask ladder, scoring | `shaping-problems` |
| Only auditing the official solution | `validate-solution` |
| Only the statement prose | `polygon-statement` |
| Only a printable/PDF statement | `tex-statement` |
| Only the checker | `polygon-checker` |
| Only the test plan | `generator-config` |
| Only the test script or the generator | `generating-tests` |
| Only the input validator | `polygon-validator` |
| Only an HTML editorial | `writing-editorials` |
| Only pushing a finished package to Codeforces Polygon | `uploading-to-polygon` |

## Purpose

The working repository is a problem-preparation workspace for Codeforces Polygon.

Execute these workflow steps in this exact order:

0. create `outputs/problem.json` — the shaped numbers;
1. validate `source/solution.cpp` when it is present;
2. create `outputs/statement.txt`;
3. decide the checker, and create `outputs/checker.cpp` when that decision is custom;
4. create and validate the solution suite in `outputs/solution/`;
5. create `outputs/validator.cpp`;
   - then Step 5b: create the statement's example tests in `outputs/example-test/`;
6. create `outputs/generator-config.md`;
7. create `outputs/test-script.txt` and `outputs/gentest.cpp` from that config;
8. create `outputs/editorial.html`.

Do not reorder or skip a mandatory gate unless the user explicitly changes the workflow.

Two steps are optional and are skipped by default; produce them only on explicit request:

- Step 2b `outputs/tex-statement.tex` — only after Step 2 has produced
  `outputs/statement.txt`.
- Step 9 upload to Codeforces Polygon — only after Step 8 and the cross-artifact
  consistency gate below have passed.

Skipping an optional step is never a workflow failure.

Every other step is mandatory. In particular **Step 3 always produces a checker
decision** in `outputs/problem.json`: `{ "kind": "stock", "name": "<token>" }` or
`{ "kind": "custom", "file": "checker.cpp" }`. The file itself follows only on the
custom branch — an output that is unique for every valid input and that a Polygon
standard checker compares exactly binds that token instead, and `outputs/checker.cpp`
is not written. The uniqueness analysis is what decides the branch.

## Source-of-truth files

At the working repository root:

- `source/problem-context.md` — REQUIRED. Concise authoritative description of the problem, **including its constraints and its subtask limits and scoring**. There is no separate `source/subtask.md`; if an older repository still has one, treat it as stale and report it rather than reading it.
- `source/solution.cpp` — OPTIONAL. Intended official/full solution.

Generated artifacts must never silently redefine the problem. If generated files disagree with source-of-truth files, the source-of-truth files win and the inconsistency must be reported.

Do not edit `source/problem-context.md` or `source/solution.cpp` unless the user explicitly asks. The local-I/O normalization rule in Step 1 is the sole standing exception for `source/solution.cpp`.

## Rigor profile

The workflow has two rigor profiles, defined in `preference.yml`: `heavy` (this document
exactly as written — the default) and `light-weight` (quick generation; `preference.yml`
lists the specific overrides it applies on top of this document, and the things it never
relaxes). Read `preference.yml` from the working repository root when it has one,
otherwise the copy shipped with this pack (beside the pack's `skills/` directory). Before
starting work, pick the active profile: honor whatever the user's request implies per
that file's `selection` rules, otherwise use its `default`. State the active profile at
the start of the work and again in the final summary. A profile only changes how much
validation/coverage is gathered — it never authorizes skipping a rule listed under
`never_relaxed`.

## Always-on skill

Use the `yagni-principles` skill on every task, in every workflow step, and alongside every other skill. It is never optional and never skipped.

## C++ I/O convention

Every C++ file written or edited must use the C++ stream API instead of the C stdio API. This covers `outputs/gentest.cpp`, `outputs/validator.cpp`, `outputs/checker.cpp`, every file in `outputs/solution/`, and any scratch/brute-force program used during validation.

- Use `cin`/`cout` (or the appropriate `testlib` stream) for reading and writing; do not use `scanf`, `printf`, `fscanf`, `fprintf`, `puts`, `getchar`, or `putchar`.
- In solution-style programs that read from standard input, start `main` with `ios_base::sync_with_stdio(false);` and `cin.tie(nullptr);`, and end lines with `'\n'` rather than `endl`.
- Inside `testlib` programs the testlib readers still win: use `inf.read*`, `ouf.read*`, and `ans.read*` for input, and `cout` for generator output. Never mix `scanf`/`printf` into them.
- Formatted output that would be natural with `printf` must use stream manipulators instead, e.g. `cout << fixed << setprecision(9) << x << '\n';` from `<iomanip>`.
- `freopen` remains allowed solely for the guarded local `.inp`/`.out` redirection described in Step 1 and Step 4; it redirects the standard streams, so `cin`/`cout` keep working.
- When reviewing or fixing an existing generated file that uses `scanf`/`printf`, convert it. Do not rewrite `source/solution.cpp` for this reason — it is a source-of-truth file, and this convention is not one of the allowed exceptions to the no-edit rule. Report the deviation instead.

## Mandatory ambiguity gate

Before generating or modifying any Step 1-8 artifact, read every available source-of-truth file.

Do not guess material facts. Ask the user before continuing when any fact that can change correctness is missing or ambiguous. Prefer one compact group of concrete questions.

In particular, verify when relevant:

- exact meaning and scope of every input variable;
- type/domain of every value;
- inclusive lower/upper bounds;
- relations between variables, including sum-of-sizes constraints across test cases;
- whether input has one test or multiple tests;
- for graphs: directed/undirected, simple/multigraph, loops, parallel edges, connectivity;
- for trees: rooted/unrooted, root if meaningful, parent/edge representation;
- for arrays: indexing, sign, duplicates, ordering/permutation requirements;
- for strings: alphabet, case sensitivity, empty strings, length bounds;
- whether output is unique or may have multiple valid answers;
- floating-point tolerance if any;
- existence/degenerate cases and what must be printed;
- subtask scoring and constraints if subtasks are expected.
- the time limit when solution verdicts or TLE classification depend on it.

If `source/problem-context.md` states no subtasks, never invent a subtask ladder. If the problem should have no partial subtasks, ask/confirm whether to treat it as one full-score subtask.

## Step 0 — `outputs/problem.json`

Use the `shaping-problems` skill.

It settles the numbers before anything is generated from them: whether the idea
is a known problem, whether the declared `N` actually separates the intended
solution from the naive one, OI vs ICPC, and which rungs of the ladder in
`source/problem-context.md` earn points. The result is `outputs/problem.json`.

This runs first because its findings invalidate later work rather than adding
to it. A ladder whose rungs collapse into one, or a bound at which the naive
solution already passes, is cheaper to fix here than after a statement, a
validator, and a test plan have been built on it.

Two of its outcomes are **blocking**, and both are the user's call, not yours:

- the problem is already known — proceed, retarget, or drop;
- the separation table contradicts `source/problem-context.md` (rungs merge, or
  a simpler algorithm clears the full constraints).

Stop and ask. Do not edit `source/problem-context.md` to match the finding.

## Step 1 — Validate solution: HARD STOP

Use the `validate-solution` skill.

If `source/solution.cpp` exists:

- before compiling, inspect its local-file redirection. If it calls `freopen` for the task `.inp`/`.out` files but has no `fopen` existence guard, modify only that redirection block to use `if (fopen(TASK ".inp", "r")) { ... }`. Keep the same task basename and `.inp`/`.out` behavior. The standard-input/standard-output fallback when the local input file is absent is mandatory so the solution can be run locally. This narrow normalization is allowed even without a separate request to edit source files; do not change algorithmic code under this exception;
- check that it solves exactly the problem in `source/problem-context.md`;
- check it against every constraint and subtask stated in `source/problem-context.md`;
- compile it;
- inspect complexity, integer widths, indexing, corner cases, input/output format;
- run targeted tests and differential/brute-force checks when feasible.

If a correctness issue, compile error, undefined behavior, complexity violation, overflow risk that can affect valid inputs, or source mismatch is found:

**STOP immediately. Do not create or modify Step 2-8 artifacts.**

Report:
1. the failing requirement;
2. the relevant code location;
3. why it is wrong;
4. a counterexample when possible;
5. what must be fixed.

If `source/solution.cpp` does not exist, mark the official-solution audit as skipped and continue only if the remaining artifacts can be written unambiguously.

## Step 2 — `outputs/statement.txt`

Use the `polygon-statement` skill.

Requirements:

- Vietnamese by default unless the user requests another language.
- Polygon simpleTex only; do not write a vnolymp standalone LaTeX document.
- Required order:
  1. problem name;
  2. statement/legend;
  3. input;
  4. output;
  5. subtasks.
- Be precise, transparent, and easy to understand.
- Add only a very short story/flavor sentence or paragraph. It must not obscure the mathematical task.
- Do not invent constraints, behavior, samples, or edge-case rules.
- Keep notation consistent with `source/problem-context.md` and `source/solution.cpp` when it exists. The statement is written before the checker and the solution suite, so it is the notation the later steps must follow; if Step 3 or Step 4 later reveals that the statement is wrong or ambiguous, return here and fix the statement rather than diverging from it.
- State variable scopes explicitly enough that a contestant cannot reasonably misread them.

## Step 2b (optional) — `outputs/tex-statement.tex`

Use the `tex-statement` skill. Run it only on explicit request, and never before Step 2.

This step renders the finished `outputs/statement.txt` into a standalone vnolymp LaTeX
document at `outputs/tex-statement.tex` and compiles it. It does not author prose: the
wording, notation, constraints, and subtask ladder must match `outputs/statement.txt`
exactly, and any disagreement with the source-of-truth files is reported rather than
resolved in the `.tex`. Time and memory limits come from `source/problem-context.md`; if
they are absent, ask instead of inventing them. `\Examples` is wired to the files in
`outputs/example-test/` (Step 5b) with `\exmpfile`; if this step runs before Step 5b, omit
the block and re-run the step once the examples exist. Never author sample data here.
Completion requires a clean compile log and a PDF verified against the statement, not
merely a zero exit code.

## Step 3 — the checker decision (`outputs/checker.cpp` when custom)

Use the `polygon-checker` skill. This step is mandatory, but its deliverable is the
**decision**, not necessarily a file.

Classify the output from the finished `outputs/statement.txt`, then record the result in
`outputs/problem.json`:

- unique for every valid input, and one Polygon standard checker compares it exactly →
  `{ "kind": "stock", "name": "<token>" }`. No file is written. The allowlist of tokens
  lives in `polygon-checker`; nothing outside it may be named here.
- non-unique witnesses, optimization/construction objectives, any special comparison
  protocol, any semantic validation, or a unique output that no token fits →
  `{ "kind": "custom", "file": "checker.cpp" }`, and `outputs/checker.cpp` is written
  per the requirements below. A doubtful case is custom.

Step 0 wrote a provisional `checker` before the statement existed; Step 3 overwrites it
and reports any disagreement.

Requirements for a custom checker:

- C++ + `testlib.h`.
- Must follow Codeforces Polygon checker conventions.
- For non-unique output, write a semantic checker. Never compare a valid witness structurally to the jury witness.
- For optimization/construction problems, validate the participant witness and compare the required objective against the jury optimum when appropriate.
- Consume all required participant output and reject invalid extra output unless the statement permits it.
- Take output semantics from the finished `outputs/statement.txt` together with the source-of-truth files. If they disagree, report it and fix Step 2 instead of encoding a different rule here.

## Step 4 — `outputs/solution/`

Create a small solution suite based on `source/problem-context.md` (constraints and subtasks included) and the finished `outputs/statement.txt`.

Design candidate algorithms independently from the problem specification. Do not obtain a “different” solution by copying `source/solution.cpp` and making cosmetic edits. Use the optional source solution only after the candidate design is fixed, for validation and comparison.

### Required directory contract

Create `outputs/solution/manifest.md` and self-contained GNU C++17 sources using these names. Every source must follow the C++ I/O convention above: `cin`/`cout` with `ios_base::sync_with_stdio(false); cin.tie(nullptr);`, never `scanf`/`printf` — including the TLE candidates, whose slowness must come from the algorithm and not from I/O.

- `ac-full-<slug>.cpp` — correct for all constraints;
- `ac-subtask-<id>-<slug>.cpp` — correct exactly for the declared subtask scope; generate this only when the user explicitly asks for a subtask-scoped solution — see below;
- `wa-<slug>.cpp` — plausible wrong-answer candidate;
- `tle-<slug>.cpp` — logically plausible candidate expected to exceed the time limit on adversarial valid tests.

If `source/solution.cpp` contains a guarded local-file block using `fopen` and `freopen`, every generated suite source must include an equivalent guarded block with the same task basename and `.inp`/`.out` behavior. Preserve the standard-input/standard-output fallback when the local input file is absent. Apply this consistently to AC, WA, and TLE candidates.

Generate one or two correct solutions, with at least one required, and default both to full-scope (`ac-full-<slug>.cpp`). Do not generate a subtask-scoped AC (`ac-subtask-<id>-<slug>.cpp`) unless the user explicitly asks for one; a subtask-scoped candidate is opt-in, not part of the default suite. When two correct solutions are generated, they must differ materially in algorithm, state representation, data structure, or complexity—not merely coding style. If no meaningful second approach exists, generate one and record that decision in the manifest.

Also generate representative WA and/or TLE candidates when a realistic contestant approach exists. These candidates must be “potential” submissions:

- they must compile, follow the exact input/output format, and attempt the real problem;
- a WA should stay close to a valid idea but contain a localized misconception, missed edge case, unsafe width, incorrect greedy assumption, or similar realistic defect;
- a TLE should be logically correct on the stated semantic scope but have complexity that can pass weak/random/small tests and fail sufficiently strong valid tests;
- do not use trivial constant answers, deliberate syntax errors, malformed output, undefined behavior as the intended failure, or obviously unrelated code;
- preserve the defect in the candidate source and document it; do not “fix” an intentional WA/TLE during validation.

`outputs/solution/manifest.md` must contain, for every source:

- filename and intended verdict (`AC`, `WA`, or `TLE`);
- claimed scope: full constraints or one exact subtask id;
- independently chosen algorithm and expected time/memory complexity;
- material difference from `source/solution.cpp` when it exists;
- for AC: validation evidence and supported domain;
- for WA: the precise defect and at least one valid counterexample when feasible;
- for TLE: the bottleneck, the limit used for classification, and an adversarial profile that exposes it;
- profiles or properties that Steps 6-7 should use to distinguish/kill the candidate.

### Time-limit policy

Treat a time limit in `source/problem-context.md` as authoritative. Otherwise the limit lives in `outputs/problem.json` as `limits.time_ms_published`, proposed by Step 0. When measurement here shows that proposal is wrong — the intended solution does not fit it, or it is loose enough that a TLE candidate survives — update `limits.time_ms_published` and announce the change. `outputs/problem.json` is the single home for the limit: do not record a second one in `outputs/solution/manifest.md`, and do not put a limit in the statement that the source files do not state.

### Validation requirements

For every suite source:

- compile with GNU C++17 and warnings enabled;
- inspect integer widths, indexing, memory, recursion depth, input handling, and output semantics;
- run targeted cases and brute-force/differential tests when feasible.

For each AC candidate:

- validate only inside its claimed full/subtask domain, but cover the boundaries of that domain;
- compare against `source/solution.cpp` when it exists, using identical results for unique output and `outputs/checker.cpp` for non-unique output;
- compare algorithms, assumptions, complexity, and edge-case behavior;
- investigate every discrepancy. If an AC candidate is wrong, revise it and repeat validation. If it exposes a defect in `source/solution.cpp`, reopen Step 1 and apply its hard stop.

For each WA/TLE candidate:

- confirm that its documented failure classification is real rather than assumed;
- minimize and record a WA counterexample when feasible;
- benchmark or complexity-check a TLE on adversarial valid inputs, while also confirming that it succeeds on at least some weak/small inputs;
- never use an intentional WA/TLE as a jury solution or correctness oracle.

Step 4 passes when the manifest is complete, every source compiles, every AC is correct on its declared scope, and every intentional WA/TLE is plausible and accurately characterized. An intentional WA/TLE verdict is not a Step 4 failure. Do not proceed to Step 5 while an AC is mislabeled, a candidate's scope is ambiguous, or the manifest and source suite disagree.

## Step 5 — `outputs/validator.cpp`

Use the `polygon-validator` skill.

Create a Codeforces Polygon input validator in C++17 using `testlib.h`. It must:

- call `registerValidation(argc, argv);`;
- read fields in exactly the order defined by the source-of-truth files and `outputs/statement.txt`;
- enforce every numeric bound, cross-field relation, multi-test total, and structural invariant;
- use typed `inf.read*` methods with variable names, plus `readSpace`, `readEoln`, and `readEof` for strict formatting;
- validate the full legal input domain and, when Polygon group handling is configured, the selected subtask/group constraints;
- accept the example inputs Step 5b writes to `outputs/example-test/` and any inputs already used during Step 4 validation;
- reject temporary invalid mutations covering bounds, missing/extra tokens, malformed layout, and broken structural invariants;
- contain no contestant-output checking logic; that belongs in `outputs/checker.cpp`.

The validator is written before the generator, so it is the machine-readable definition of
the legal input domain that Steps 6-7 must respect. If it reveals that the statement or a
source-of-truth file is inconsistent, return to the earliest responsible step and fix it
there rather than weakening the validator.

## Step 5b — `outputs/example-test/`

No separate skill; this step is specified here.

Write **one or two small example tests** — the samples a contestant sees in the statement.
They are not part of the generated test set and `outputs/gentest.cpp` never produces them.

```
outputs/example-test/
├─ test_1.inp
├─ test_1.out
├─ test_2.inp        # only if a second example earns its place
└─ test_2.out
```

**Inputs are hand-written and deliberately easy.** Pick the smallest input that still
exercises the problem's core operation, so a contestant can trace the answer by hand. A
second example is worth writing only when it shows something the first cannot — a
degenerate case the statement calls out, a second output format branch, a multi-test
layout. Do not add one for volume. Never use a stress or maximum-bound case as an example.

**Outputs are produced by running a program, never written by hand.** Even an answer that
is obvious by inspection must come from an execution, because a hand-written `.out` is the
one error a statement can carry that looks authoritative and contradicts the real tests.
In order:

1. `source/solution.cpp`, when it exists and passed Step 1;
2. otherwise a validated full-scope `outputs/solution/ac-full-<slug>.cpp` from
   `outputs/solution/manifest.md`.

A subtask-scoped AC may generate an example only if that example lies inside its declared
scope, and the report must say which file was used. If neither source is available — Step 1
is under a hard stop, or the suite has no validated full-scope AC — **stop**; do not write
an example test from reasoning.

Run the program on standard input and capture standard output verbatim, e.g.

```powershell
Get-Content outputs/example-test/test_1.inp | ./sol.exe | Set-Content -Encoding utf8 outputs/example-test/test_1.out
```

Then check each example before leaving this step:

- `outputs/validator.cpp` accepts every `.inp`;
- the checker accepts the produced `.out` against itself as the answer — `outputs/checker.cpp` on the custom branch, the named `std::` token's comparison on the stock branch;
- the input's field order and the output's format match `outputs/statement.txt` exactly;
- the `.out` files are the raw program output — no editing, no reformatting, no trailing
  commentary.

If the validator rejects an example, the example is wrong or the validator is; fix the
responsible artifact rather than loosening either one.

Downstream consumers: Step 2b wires these files into `\Examples` via `\exmpfile`, and
Step 9 uploads them as the statement samples. Both read this directory and neither
invents sample data of its own. If Step 2b already ran without examples, re-run it after
this step so the `.tex` gains its `\Examples` block.

## Step 6 — `outputs/generator-config.md`

Use the `generator-config` skill.

This file is the design contract for both `outputs/test-script.txt` and `outputs/gentest.cpp`. Generate it before either artifact. It must contain:

- normalized input schema and constraints;
- subtask-specific limits;
- invariants that every generated test must satisfy;
- interpretation of `rate`;
- generation profiles and edge cases;
- parameter definitions used by `test-script.txt`;
- special generation logic required by individual subtasks;
- distribution goals for difficult/adversarial cases;
- applicable specialized generator skills;
- a solution-kill matrix mapping every WA/TLE candidate in `outputs/solution/manifest.md` to its failure mechanism, adversarial profile, relevant subtasks, and planned script coverage;
- the AC candidate(s) that can serve as differential oracles for each subtask scope.

Do not put unexplained magic constants into `outputs/gentest.cpp`; important choices belong here.

## Step 7 — `outputs/test-script.txt` and `outputs/gentest.cpp`

Use the `generating-tests` skill plus all applicable specialized generator skills. The
script and the generator are one design act: write `outputs/test-script.txt` first, then
`outputs/gentest.cpp`, then validate them together. Read and follow the completed
`outputs/generator-config.md`; every profile and named option in the script must be
defined there and parsed by the generator. If the config cannot express required test
coverage, revise Step 6 before writing the script.

### Script defaults

Default when the user gives no special request:

- exactly 100 generated tests;
- every generator call has a positional reproducibility seed immediately after `gentest`, followed by `--subtask <S>` and `--rate <R>`;
- `rate` is always in `[0.70, 1.00]`;
- approximately 50% of tests have `rate >= 0.90`;
- still include meaningful coverage in `[0.70, 0.80)` and `[0.80, 0.90)`;
- do not pass exact input sizes such as exact `n` or `m`; `outputs/gentest.cpp` chooses those values;
- for multi-test problems, add a semantic parameter such as `numtest` when needed;
- add only optional parameters that describe a generation profile, not literal final input values;
- never write `--seed`; the seed must be the first positional argument after the generator name;
- include semantic profiles aimed at the documented WA/TLE candidates in `outputs/solution/manifest.md` when those profiles are expressible by the generator config.

Canonical form:

```text
gentest 84GOGVE16X --subtask 1 --rate 0.87 > $
```

Every line must end in `> $` when the jury answer comes from the official solution.

### Generator requirements

- C++ + `testlib.h`;
- Polygon-compatible generator;
- start with `registerGen(argc, argv, 1);`;
- use `prepareOpts(argc, argv);` when named options are used;
- always accept `subtask` and `rate`;
- use the positional seed supplied immediately after the generator name for deterministic regeneration; do not define a named `--seed` option;
- generate all actual input values inside the generator;
- use the shared `genValue` distribution strategy for numeric values unless the problem requires a more specific generator;
- print input fields in exactly the statement's order;
- no missing or surplus spaces/newlines;
- enforce all structural invariants before printing;
- never generate an input outside the chosen subtask;
- implement the adversarial profiles promised by the solution-kill matrix;
- during validation, run every AC only on tests within its declared scope and require agreement with the official solution/checker;
- confirm that every documented WA is rejected by at least one generated test when feasible;
- confirm that every documented TLE is stressed by at least one near-limit adversarial test under the authoritative or announced provisional time limit;
- record the killing seed/profile in `outputs/generator-config.md` or report why a candidate could not be distinguished safely;
- run `outputs/validator.cpp` on every generated test and require that all of them are accepted.

If the validator rejects a generated test, the generator, script, or config is at fault:
return to the earliest responsible artifact among the config, the script, and the
generator, and fix it. Do not weaken
`outputs/validator.cpp` to make a test pass; only revise it when the statement or a
source-of-truth file proves the validator itself wrong, in which case fix the responsible
earlier step too.

## Specialized generator routing

Load specialized skills only when applicable:

- `generator-tree` — trees, rooted trees, parent arrays, tree-shaped graph subtasks.
- `generator-graph` — general directed/undirected graphs, connectivity, density, DAGs, multigraph constraints.
- `generator-array` — arrays, permutations, monotone/duplicate/adversarial numeric sequences.
- `generator-number-theory` — primes, divisors, multiples, gcd/lcm, prime powers, factorization-shaped values.
- `generator-string` — strings, alphabets, repetitions, periodicity, palindrome/border/run patterns.

If a problem needs another specialized structure, create a focused skill under `skills/` rather than bloating the generic generator skill.

## Step 8 — `outputs/editorial.html`

Use the `writing-editorials` skill. The editorial must be written in Vietnamese. Prefer the validated `source/solution.cpp` as the implementation to explain; when the source solution is absent, use a validated full-scope AC solution declared in `outputs/solution/manifest.md`. A subtask-only AC may support that subtask's section but must not be presented as a full solution.

## Step 9 (optional) — upload to Codeforces Polygon

Use the `uploading-to-polygon` skill. Run it only on explicit request, and never
before Step 8 and the cross-artifact consistency gate below have passed. It
pushes the finished `outputs/` package to Polygon through the external
cf-polygon-mcp server and records the problem id in `outputs/polygon.json`; it
never regenerates or repairs an artifact. A package that fails the gate is fixed
in its own step, not during upload.

## Cross-artifact consistency gate

Before finishing, cross-check:

- every file declared in `outputs/solution/manifest.md` exists, compiles, and has the documented verdict/scope;
- every AC solution's input/output semantics == source-of-truth files;
- every AC solution agrees with `source/solution.cpp` or the semantic checker on all cross-checks within its declared scope when the source solution exists;
- every WA/TLE has a documented failure mechanism and corresponding test-generation target;
- statement input order == `outputs/gentest.cpp` output order;
- validator read order and accepted domain == statement input semantics;
- statement bounds == `outputs/generator-config.md` bounds == generator bounds;
- each `outputs/test-script.txt` option exists in `outputs/gentest.cpp`;
- each subtask index/constraint agrees across `source/problem-context.md`, `outputs/problem.json`, statement, config, script, and generator;
- `outputs/problem.json` subtask points sum to `polygon.total_points`, and its `format`, limits, and bounds match the statement;
- `outputs/problem.json` carries a `checker` in one of its two shapes, and it matches reality: on `"custom"`, `outputs/checker.cpp` exists and its semantics == statement output semantics, including tokenization and any floating-point tolerance; on `"stock"`, the named token's comparison == the statement's output format and no `outputs/checker.cpp` is present;
- the decision is `"custom"` whenever the statement admits more than one correct output;
- `outputs/statement.txt`'s limits, I/O mode, and problem name agree with `outputs/problem.json`;
- multi-test format agrees everywhere;
- `outputs/editorial.html` describes in Vietnamese the algorithm and complexity of the validated `source/solution.cpp` when it exists, otherwise a validated full-scope AC from `outputs/solution/manifest.md`;
- every generated test can be consumed by every suite source whose declared scope contains that test and by `source/solution.cpp` when the latter exists;
- every generated test is accepted by `outputs/validator.cpp`, while representative malformed/invalid inputs are rejected;
- `outputs/example-test/` holds 1-2 `test_<i>.inp`/`test_<i>.out` pairs, each `.inp` is accepted by `outputs/validator.cpp`, each `.out` is the verbatim output of the validated solution named in the report, and both match the statement's input/output format;
- `outputs/tex-statement.tex`, when it exists, points its `\exmpfile` lines at exactly the files in `outputs/example-test/`;
- every generated test remains valid regardless of whether a WA/TLE candidate accepts, rejects, times out, or prints a wrong answer.

Compile every `outputs/solution/*.cpp` file as GNU C++17. Compile `outputs/gentest.cpp`, `outputs/validator.cpp`, and `outputs/checker.cpp` when Step 3 produced one, with the same `testlib.h` environment used by Polygon when available.

## Communication

When blocked by ambiguity, explain exactly what is unknown and why it changes one or more generated artifacts.

When Step 1 fails, stop and report the failure; do not continue “for convenience”.

When Step 4 reveals a defect in an AC candidate, fix it and repeat its scoped validation. When the observed behavior of a WA/TLE does not match its manifest entry, repair the candidate classification/design or manifest and repeat the relevant kill checks.

When all requested artifacts are complete, summarize:
- files created/changed;
- validation performed;
- any assumptions explicitly confirmed by the user;
- any remaining risks.
