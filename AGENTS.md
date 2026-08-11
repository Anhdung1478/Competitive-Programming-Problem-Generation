# Competitive Programming Problem Package Agent

## Purpose

This repository is a problem-preparation workspace for Codeforces Polygon.

The agent must convert the source-of-truth problem files into these generated artifacts, in this exact order:

1. validate the official solution;
2. `outputs/checker.cpp`;
3. `outputs/statement.txt`;
4. `outputs/test-script.txt`;
5. `outputs/generator-config.md`;
6. `outputs/gentest.cpp`.

Do not reorder or skip a gate unless the user explicitly changes the workflow.

## Source-of-truth files

At repository root:

- `source/problem-context.md` — REQUIRED. Concise authoritative description of the problem.
- `source/solution.cpp` — OPTIONAL. Intended official/full solution.
- `source/subtask.md` — OPTIONAL. Subtask limits and scoring.

Generated artifacts must never silently redefine the problem. If generated files disagree with source-of-truth files, the source-of-truth files win and the inconsistency must be reported.

Do not edit `source/problem-context.md`, `source/solution.cpp`, or `source/subtask.md` unless the user explicitly asks.

## Mandatory ambiguity gate

Before generating or modifying B1-B6 artifacts, read every available source-of-truth file.

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

If `source/subtask.md` is absent, never invent a subtask ladder. If the problem should have no partial subtasks, ask/confirm whether to treat it as one full-score subtask.

## Step 1 — Validate solution: HARD STOP

Use the `validate-solution` skill.

If `solution.cpp` exists:

- check that it solves exactly the problem in `source/problem-context.md`;
- check it against every constraint/subtask in `source/subtask.md` when present;
- compile it;
- inspect complexity, integer widths, indexing, corner cases, input/output format;
- run targeted tests and differential/brute-force checks when feasible.

If a correctness issue, compile error, undefined behavior, complexity violation, overflow risk that can affect valid inputs, or source mismatch is found:

**STOP immediately. Do not create or modify B2-B6.**

Report:
1. the failing requirement;
2. the relevant code location;
3. why it is wrong;
4. a counterexample when possible;
5. what must be fixed.

If `source/solution.cpp` does not exist, mark the solution audit as skipped and continue only if the remaining artifacts can be written unambiguously.

## Step 2 — `outputs/checker.cpp`

Use the `polygon-checker` skill.

Requirements:

- C++ + `testlib.h`.
- Must follow Codeforces Polygon checker conventions.
- For unique/deterministic output with no special semantics, compare participant and jury output token-by-token and reject missing/extra tokens.
- For non-unique output, write a semantic checker. Never compare a valid witness structurally to the jury witness.
- For optimization/construction problems, validate the participant witness and compare the required objective against the jury optimum when appropriate.
- Consume all required participant output and reject invalid extra output unless the statement permits it.

## Step 3 — `outputs/statement.txt`

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
- Keep notation consistent with `source/problem-context.md`, `source/solution.cpp`, and `source/subtask.md`.
- State variable scopes explicitly enough that a contestant cannot reasonably misread them.

## Step 4 — `outputs/test-script.txt`

Use the `test-script` skill.

Default when the user gives no special request:

- exactly 100 generated tests;
- every generator call has a positional reproducibility seed immediately after `gentest`, followed by `--subtask <S>` and `--rate <R>`;
- `rate` is always in `[0.70, 1.00]`;
- approximately 50% of tests have `rate >= 0.90`;
- still include meaningful coverage in `[0.70, 0.80)` and `[0.80, 0.90)`;
- do not pass exact input sizes such as exact `n` or `m`; `outputs/gentest.cpp` chooses those values;
- for multi-test problems, add a semantic parameter such as `numtest` when needed;
- add only optional parameters that describe a generation profile, not literal final input values;
- never write `--seed`; the seed must be the first positional argument after the generator name.

Canonical form:

```text
gentest 84GOGVE16X --subtask 1 --rate 0.87 > $
```

Every line must end in `> $` when the jury answer comes from the official solution.

## Step 5 — `outputs/generator-config.md`

Use the `generator-config` skill.

This file is the design contract for `outputs/gentest.cpp`. It must contain:

- normalized input schema and constraints;
- subtask-specific limits;
- invariants that every generated test must satisfy;
- interpretation of `rate`;
- generation profiles and edge cases;
- parameter definitions used by `test-script.txt`;
- special generation logic required by individual subtasks;
- distribution goals for difficult/adversarial cases;
- applicable specialized generator skills.

Do not put unexplained magic constants into `outputs/gentest.cpp`; important choices belong here.

## Step 6 — `outputs/gentest.cpp`

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
- never generate an input outside the chosen subtask.

## Specialized generator routing

Load specialized skills only when applicable:

- `generator-tree` — trees, rooted trees, parent arrays, tree-shaped graph subtasks.
- `generator-graph` — general directed/undirected graphs, connectivity, density, DAGs, multigraph constraints.
- `generator-array` — arrays, permutations, monotone/duplicate/adversarial numeric sequences.
- `generator-number-theory` — primes, divisors, multiples, gcd/lcm, prime powers, factorization-shaped values.
- `generator-string` — strings, alphabets, repetitions, periodicity, palindrome/border/run patterns.

If a problem needs another specialized structure, create a focused skill under `.agents/skills/` rather than bloating the generic generator skill.

## Cross-artifact consistency gate

Before finishing, cross-check:

- statement input order == `outputs/gentest.cpp` output order;
- statement bounds == `outputs/generator-config.md` bounds == generator bounds;
- each `outputs/test-script.txt` option exists in `outputs/gentest.cpp`;
- each subtask index/constraint agrees across `source/subtask.md`, statement, config, script, and generator;
- checker semantics == statement output semantics;
- multi-test format agrees everywhere;
- every generated test can be consumed by `source/solution.cpp` when it exists.

Compile `outputs/checker.cpp` and `outputs/gentest.cpp` with the same `testlib.h` environment used by Polygon when available.

## Communication

When blocked by ambiguity, explain exactly what is unknown and why it changes one or more generated artifacts.

When Step 1 fails, stop and report the failure; do not continue “for convenience”.

When all requested artifacts are complete, summarize:
- files created/changed;
- validation performed;
- any assumptions explicitly confirmed by the user;
- any remaining risks.
