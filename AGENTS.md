# Competitive Programming Problem Package Agent

## Purpose

This repository is a problem-preparation workspace for Codeforces Polygon.

The agent must execute these workflow steps in this exact order:

1. validate `source/solution.cpp` when it is present;
2. create `outputs/checker.cpp`;
3. create and validate the solution suite in `outputs/solution/`;
4. create `outputs/statement.txt`;
5. create `outputs/generator-config.md`;
6. create `outputs/test-script.txt` from that config;
7. create `outputs/gentest.cpp`;
8. create `outputs/validator.cpp`;
9. create `outputs/editorial.html`.

Do not reorder or skip a gate unless the user explicitly changes the workflow.

## Source-of-truth files

At repository root:

- `source/problem-context.md` — REQUIRED. Concise authoritative description of the problem.
- `source/solution.cpp` — OPTIONAL. Intended official/full solution.
- `source/subtask.md` — OPTIONAL. Subtask limits and scoring.

Generated artifacts must never silently redefine the problem. If generated files disagree with source-of-truth files, the source-of-truth files win and the inconsistency must be reported.

Do not edit `source/problem-context.md`, `source/solution.cpp`, or `source/subtask.md` unless the user explicitly asks.

## Mandatory ambiguity gate

Before generating or modifying any Step 1-9 artifact, read every available source-of-truth file.

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

If `source/subtask.md` is absent, never invent a subtask ladder. If the problem should have no partial subtasks, ask/confirm whether to treat it as one full-score subtask.

## Step 1 — Validate solution: HARD STOP

Use the `validate-solution` skill.

If `source/solution.cpp` exists:

- check that it solves exactly the problem in `source/problem-context.md`;
- check it against every constraint/subtask in `source/subtask.md` when present;
- compile it;
- inspect complexity, integer widths, indexing, corner cases, input/output format;
- run targeted tests and differential/brute-force checks when feasible.

If a correctness issue, compile error, undefined behavior, complexity violation, overflow risk that can affect valid inputs, or source mismatch is found:

**STOP immediately. Do not create or modify Step 2-9 artifacts.**

Report:
1. the failing requirement;
2. the relevant code location;
3. why it is wrong;
4. a counterexample when possible;
5. what must be fixed.

If `source/solution.cpp` does not exist, mark the official-solution audit as skipped and continue only if the remaining artifacts can be written unambiguously.

## Step 2 — `outputs/checker.cpp`

Use the `polygon-checker` skill.

Requirements:

- C++ + `testlib.h`.
- Must follow Codeforces Polygon checker conventions.
- For unique/deterministic output with no special semantics, compare participant and jury output token-by-token and reject missing/extra tokens.
- For non-unique output, write a semantic checker. Never compare a valid witness structurally to the jury witness.
- For optimization/construction problems, validate the participant witness and compare the required objective against the jury optimum when appropriate.
- Consume all required participant output and reject invalid extra output unless the statement permits it.

## Step 3 — `outputs/solution/`

Create a small solution suite based on `source/problem-context.md` and `source/subtask.md` when present. This replaces the legacy single file `outputs/codex-solution.cpp`; do not create that legacy file. If it already exists from an older run, treat it as stale and do not use it unless the user explicitly asks to migrate it into the suite.

Design candidate algorithms independently from the problem specification. Do not obtain a “different” solution by copying `source/solution.cpp` and making cosmetic edits. Use the optional source solution only after the candidate design is fixed, for validation and comparison.

### Required directory contract

Create `outputs/solution/manifest.md` and self-contained GNU C++17 sources using these names:

- `ac-full-<slug>.cpp` — correct for all constraints;
- `ac-subtask-<id>-<slug>.cpp` — correct exactly for the declared subtask scope;
- `wa-<slug>.cpp` — plausible wrong-answer candidate;
- `tle-<slug>.cpp` — logically plausible candidate expected to exceed the time limit on adversarial valid tests.

If `source/solution.cpp` contains a guarded local-file block using `fopen` and `freopen`, every generated suite source must include an equivalent guarded block with the same task basename and `.inp`/`.out` behavior. Preserve the standard-input/standard-output fallback when the local input file is absent. Apply this consistently to AC, WA, and TLE candidates.

Generate one or two correct solutions, with at least one required. A correct solution may cover the full problem or exactly one declared subtask. When two correct solutions are generated, they must differ materially in algorithm, state representation, data structure, or complexity—not merely coding style. If no meaningful second approach exists, generate one and record that decision in the manifest.

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
- profiles or properties that Steps 5–7 should use to distinguish/kill the candidate.

### Time-limit policy

Treat a time limit in `source/problem-context.md` as authoritative. If it is absent, choose a provisional solution-testing time limit based on the constraints, intended complexity, and local benchmark evidence. Announce that choice to the user and record it prominently in `outputs/solution/manifest.md`. A provisional limit is only a testing assumption: do not add it to the statement or present it as authoritative metadata.

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

Step 3 passes when the manifest is complete, every source compiles, every AC is correct on its declared scope, and every intentional WA/TLE is plausible and accurately characterized. An intentional WA/TLE verdict is not a Step 3 failure. Do not proceed to Step 4 while an AC is mislabeled, a candidate's scope is ambiguous, or the manifest and source suite disagree.

## Step 4 — `outputs/statement.txt`

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
- Keep notation consistent with `source/problem-context.md`, `source/subtask.md`, the AC solutions declared in `outputs/solution/manifest.md`, and `source/solution.cpp` when it exists.
- State variable scopes explicitly enough that a contestant cannot reasonably misread them.

## Step 5 — `outputs/generator-config.md`

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

## Step 6 — `outputs/test-script.txt`

Use the `test-script` skill. Read and follow the completed `outputs/generator-config.md`; every profile and named option in the script must be defined there. If the config cannot express required test coverage, revise Step 5 before writing the script.

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

## Step 7 — `outputs/gentest.cpp`

Use the `polygon-gentest` skill plus all applicable specialized generator skills.

Requirements:

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
- record the killing seed/profile in `outputs/generator-config.md` or report why a candidate could not be distinguished safely.

## Specialized generator routing

Load specialized skills only when applicable:

- `generator-tree` — trees, rooted trees, parent arrays, tree-shaped graph subtasks.
- `generator-graph` — general directed/undirected graphs, connectivity, density, DAGs, multigraph constraints.
- `generator-array` — arrays, permutations, monotone/duplicate/adversarial numeric sequences.
- `generator-number-theory` — primes, divisors, multiples, gcd/lcm, prime powers, factorization-shaped values.
- `generator-string` — strings, alphabets, repetitions, periodicity, palindrome/border/run patterns.

If a problem needs another specialized structure, create a focused skill under `.agents/skills/` rather than bloating the generic generator skill.

## Step 8 — `outputs/validator.cpp`

Use the `polygon-validator` skill.

Create a Codeforces Polygon input validator in C++17 using `testlib.h`. It must:

- call `registerValidation(argc, argv);`;
- read fields in exactly the order defined by the source-of-truth files and `outputs/statement.txt`;
- enforce every numeric bound, cross-field relation, multi-test total, and structural invariant;
- use typed `inf.read*` methods with variable names, plus `readSpace`, `readEoln`, and `readEof` for strict formatting;
- validate the full legal input domain and, when Polygon group handling is configured, the selected subtask/group constraints;
- accept representative tests from every `outputs/test-script.txt` profile generated by `outputs/gentest.cpp`;
- reject temporary invalid mutations covering bounds, missing/extra tokens, malformed layout, and broken structural invariants;
- contain no contestant-output checking logic; that belongs in `outputs/checker.cpp`.

If the validator reveals that the statement, config, script, or generator emits/permits inconsistent input, return to the earliest responsible step, fix it, and repeat validation. Do not proceed while any generated test is rejected.

## Step 9 — `outputs/editorial.html`

Use the `writing-editorials` skill. The editorial must be written in Vietnamese. Prefer the validated `source/solution.cpp` as the implementation to explain; when the source solution is absent, use a validated full-scope AC solution declared in `outputs/solution/manifest.md`. A subtask-only AC may support that subtask's section but must not be presented as a full solution.

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
- each subtask index/constraint agrees across `source/subtask.md`, statement, config, script, and generator;
- checker semantics == statement output semantics;
- multi-test format agrees everywhere;
- `outputs/editorial.html` describes in Vietnamese the algorithm and complexity of the validated `source/solution.cpp` when it exists, otherwise a validated full-scope AC from `outputs/solution/manifest.md`;
- every generated test can be consumed by every suite source whose declared scope contains that test and by `source/solution.cpp` when the latter exists;
- every generated test is accepted by `outputs/validator.cpp`, while representative malformed/invalid inputs are rejected;
- every generated test remains valid regardless of whether a WA/TLE candidate accepts, rejects, times out, or prints a wrong answer.

Compile every `outputs/solution/*.cpp` file as GNU C++17. Compile `outputs/checker.cpp`, `outputs/gentest.cpp`, and `outputs/validator.cpp` with the same `testlib.h` environment used by Polygon when available.

## Communication

When blocked by ambiguity, explain exactly what is unknown and why it changes one or more generated artifacts.

When Step 1 fails, stop and report the failure; do not continue “for convenience”.

When Step 3 reveals a defect in an AC candidate, fix it and repeat its scoped validation. When the observed behavior of a WA/TLE does not match its manifest entry, repair the candidate classification/design or manifest and repeat the relevant kill checks.

When all requested artifacts are complete, summarize:
- files created/changed;
- validation performed;
- any assumptions explicitly confirmed by the user;
- any remaining risks.
