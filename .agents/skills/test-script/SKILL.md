---
name: test-script
description: Create or review outputs/test-script.txt for Codeforces Polygon. Use for workflow Step 6 after outputs/generator-config.md is complete, deriving the generated test set, subtask/rate/seed options, and adversarial profile coverage from that contract.
---

# Write `test-script.txt`

The script describes the test set. It does not directly choose exact problem input sizes such as exact `n` or `m`.

Read, in order:

- `problem-context.md`;
- `subtask.md` when present;
- `outputs/solution/manifest.md` when present;
- `outputs/generator-config.md` — required and authoritative for profile/option design;
- current generator option names if `gentest.cpp` already exists.

If the config omits a profile, parameter, subtask rule, or WA/TLE target needed for adequate coverage, return to Step 5 and revise the config before writing the script. Do not introduce an undocumented script option locally.

## Default test count

Unless the user specifies otherwise, generate exactly **100** script lines/tests.

## Required generation options

Every generated test must carry:

- a positional reproducibility seed immediately after the generator name;
- `--subtask <S>`;
- `--rate <R>`.

`subtask` and `rate` are the two mandatory semantic generator parameters.

The seed is a fixed positional argument, not a named option. **Never write `--seed`.**
It exists so the exact random test can be regenerated and is not a problem input variable.

Canonical lines:

```text
gentest 84GOGVE16X --subtask 1 --rate 0.87 > $
gentest 807H8KNO3J --subtask 1 --rate 0.94 > $
```

General form:

```text
gentest <SEED> --subtask <S> --rate <R> > $
```

For a multi-test problem, when useful:

```text
gentest <SEED> --subtask <S> --rate <R> --numtest <PROFILE_OR_COUNT> > $
```

Use the actual Polygon generator source name if it differs from `gentest`.

## `rate` policy

Always keep:

```text
0.70 <= rate <= 1.00
```

Default 100-test distribution target:

- 15 tests: `0.70 <= rate < 0.80`;
- 35 tests: `0.80 <= rate < 0.90`;
- 50 tests: `0.90 <= rate <= 1.00`.

This makes approximately 50% of tests large (`rate >= 0.90`) while retaining meaningful medium/lower-scale cases.

Do not make every rate unique just for appearance. Boundary and repeated values are useful.

Include representative values near:

- `0.70`;
- `0.80`;
- `0.90`;
- `0.95`;
- `0.99`;
- `1.00`.

Randomize/mix test profiles instead of placing all large tests in one block unless Polygon scoring groups require grouping.

## Subtask allocation

Every subtask must have meaningful coverage.

When point weights are known, use them as a starting signal for test allocation, but still ensure each subtask has:

- boundary-size tests;
- structural edge cases;
- random/general tests;
- adversarial patterns relevant to the intended solution.

If subtasks overlap, a test tagged with one subtask must obey that subtask's exact generator contract.

If there are no partial subtasks and the user confirms a single full-score group, use `subtask=1` consistently.

When `outputs/solution/manifest.md` contains WA/TLE candidates, allocate meaningful tests to their documented adversarial profiles. Prefer profiles that distinguish several realistic candidates at once, but ensure each candidate has at least one planned killing test when feasible. Do not call a subtask-only AC outside its declared scope when validating those plans.

## No literal final sizes

Do not write script parameters such as:

```text
--n=100000
--m=200000
--k=17
```

when those names are literal problem input values.

Instead pass semantic profiles, for example:

```text
--shape=line
--density=dense
--value-profile=maximum
--answer-profile=no
--tree-type=focused
```

Then `gentest.cpp` chooses valid concrete values based on `subtask` and `rate`.

An exact numeric option is acceptable only when it is genuinely a generation-control parameter (for example a requested `numtest`) rather than bypassing generator logic.

## Multi-test problems

Consider a `numtest` option/profile so tests cover:

- one large case;
- many small cases;
- mixed sizes;
- the maximum allowed sum-of-sizes condition.

`gentest.cpp` must still enforce the global constraints.

## Seeds

The seed is always the **first positional argument after `gentest`**:

```text
gentest <SEED> ...
```

It must not have an option name or prefix:

```text
# WRONG
gentest --seed ABC123 --subtask 1 --rate 0.90 > $

# CORRECT
gentest ABC123 --subtask 1 --rate 0.90 > $
```

Use deterministic distinct seeds by default. Seeds may be alphanumeric strings suitable for Polygon/testlib command lines, for example:

```text
84GOGVE16X
807H8KNO3J
```

Do not rely on wall-clock time.

## Consistency checks

Before finishing:

- every named option used by the script is parsed/recognized by `gentest.cpp` or planned in `generator-config.md`;
- the first positional argument after `gentest` is always the seed;
- all subtask ids exist;
- every rate is in `[0.70, 1.00]`;
- about half are `>= 0.90`;
- exactly 100 tests exist by default;
- lines use `> $` when the jury answer should come from the official solution.
- every WA/TLE target in `outputs/solution/manifest.md` is covered by a script profile or has a documented reason it cannot be targeted safely.
