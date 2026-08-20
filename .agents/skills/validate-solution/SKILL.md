---
name: validate-solution
description: Validate solution.cpp against problem-context.md and subtask.md before any Polygon artifacts are generated. Use as workflow step B1 or whenever the official solution, constraints, input semantics, complexity, overflow, or correctness must be audited. A failure is a hard stop for downstream generation.
---

# Validate the official solution

## Inputs

Read:

1. `problem-context.md` — required.
2. `subtask.md` — when present.
3. `solution.cpp` — when present.

Never edit these files during validation unless the user separately asks for a fix, except for the local-I/O normalization below.

## Normalize unconditional local-file redirection

Before compilation, inspect `solution.cpp` for `freopen` calls targeting the task `.inp` and `.out` files. If those calls are unconditional and there is no `fopen` existence guard, edit only the redirection block into this guarded form:

```cpp
#define TASK "taskname"
if (fopen(TASK ".inp", "r")) {
    freopen(TASK ".inp", "r", stdin);
    freopen(TASK ".out", "w", stdout);
}
```

Preserve the existing task basename and file extensions. Do not alter algorithmic code. The program must continue using standard input/output when the local input file is absent, so automated local tests can pipe input normally. This is a standing, narrowly scoped exception to the usual prohibition on editing source-of-truth files.

## If `solution.cpp` is absent

Report `B1: SKIPPED — solution.cpp is not present`.

Do not pretend correctness was verified. Continue only if later artifacts are otherwise well-defined.

## Normalize the specification first

Write a private checklist of:

- input fields and their order;
- variable types/domains;
- lower/upper bounds;
- cross-variable relations;
- one-test vs multi-test structure;
- global sum constraints;
- structural invariants;
- exact output semantics;
- all subtasks.

If a correctness-relevant fact is ambiguous, ask the user and stop before making downstream files.

## Static audit

Check the solution for:

- reading every input field in the correct order;
- handling the correct number of test cases;
- producing exactly the required result;
- asymptotic time against full constraints;
- asymptotic memory against full constraints;
- recursion depth/stack risk;
- signed overflow and intermediate-expression overflow;
- array/vector bounds;
- off-by-one and indexing assumptions;
- uninitialized state between test cases;
- invalid assumptions about sorting, uniqueness, connectivity, positivity, etc.;
- corner cases: minimum sizes, maximum sizes, all-equal, duplicates, empty/degenerate states when legal.

Subtasks do not weaken the full-solution requirement. Use them to expose constraint assumptions and to guide tests.

## Compile

Compile with a modern C++ standard compatible with the repository/Polygon, preferably:

```bash
g++ -std=gnu++17 -O2 -pipe -Wall -Wextra -Wshadow -Wconversion solution.cpp -o /tmp/official_solution
```

Warnings are evidence to inspect, not automatic failures.

A compile error is a B1 failure.

## Dynamic validation

When feasible:

1. construct direct edge cases from the normalized specification;
2. create a small brute-force/reference implementation in temporary scratch space;
3. generate many small valid inputs;
4. compare the official solution against the brute/reference output;
5. minimize any counterexample.

Do not add temporary brute/stress files to the project unless the user asks.

For non-unique outputs, compare semantics rather than raw witness text.

## Result

### PASS

Report briefly:

- algorithm understood;
- expected complexity;
- compile result;
- dynamic checks performed;
- notable corner cases checked.

Then the pipeline may proceed to B2.

### FAIL — HARD STOP

Immediately stop B2-B6.

Report:

- requirement violated;
- code location;
- technical explanation;
- smallest counterexample available;
- actual vs expected behavior;
- required direction of repair.

Do not silently patch `solution.cpp` and continue.
