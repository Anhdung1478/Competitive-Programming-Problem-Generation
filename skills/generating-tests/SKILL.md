---
name: generating-tests
description: Create or review outputs/test-script.txt and outputs/gentest.cpp for Codeforces Polygon using testlib.h. Use for workflow Step 7 after outputs/generator-config.md is complete, covering the generated test set, subtask/rate/seed options, adversarial profile coverage, concrete input generation, and the solution-kill matrix run.
---

# Write `test-script.txt` and `gentest.cpp`

These two artifacts are one design act split across two files. The script says **which
tests exist** in semantic terms; the generator turns each of those semantic descriptions
into **concrete input**. Both are derived from the completed `outputs/generator-config.md`
and must agree with each other option for option.

Write the script first, then the generator, then validate them together.

Read, in order:

1. `source/problem-context.md`;
2. `source/subtask.md` when present;
3. `outputs/statement.txt`;
4. `outputs/solution/manifest.md` and referenced suite sources when present;
5. `outputs/generator-config.md` — required and authoritative for profile/option design;
6. `outputs/validator.cpp` — the legal input domain every generated test must satisfy;
7. `source/solution.cpp` when present.

If these artifacts disagree materially, stop and report the mismatch instead of choosing
one silently. If the config omits a profile, parameter, subtask rule, or WA/TLE target
needed for adequate coverage, return to Step 6 and revise the config first. Do not
introduce an undocumented script option locally.

## The invocation contract

Both files are bound by one command form:

```text
gentest <SEED> --subtask <S> --rate <R> > $
```

For example:

```text
gentest 84GOGVE16X --subtask 1 --rate 0.87 > $
```

- `<SEED>` is the **first positional argument**, never a named option. **Never write
  `--seed`**, and never read it with `opt<...>("seed")` — `registerGen(argc, argv, 1)`
  seeds testlib from the whole invocation. The seed exists so an exact random test can be
  regenerated; it is not a problem input variable.
- `subtask` and `rate` are the two mandatory semantic parameters. The script always passes
  them; the generator always accepts them.
- Lines end in `> $` when the jury answer comes from the official solution.
- Use the actual Polygon generator source name if it differs from `gentest`.

Multi-test problems may add a semantic count/profile option:

```text
gentest <SEED> --subtask <S> --rate <R> --numtest <PROFILE_OR_COUNT> > $
```

## Part 1 — `outputs/test-script.txt`

The script describes the test set. It does not choose exact problem input sizes such as
exact `n` or `m`.

### Default test count

Unless the user specifies otherwise, generate exactly **100** script lines/tests.

### `rate` policy

Always keep `0.70 <= rate <= 1.00`. Default 100-test distribution target:

| Band | Tests |
|---|---|
| `0.70 <= rate < 0.80` | 15 |
| `0.80 <= rate < 0.90` | 35 |
| `0.90 <= rate <= 1.00` | 50 |

This makes approximately 50% of tests large while retaining meaningful medium/lower-scale
cases. Include representative values near `0.70`, `0.80`, `0.90`, `0.95`, `0.99`, `1.00`.

Do not make every rate unique just for appearance — boundary and repeated values are
useful. Randomize/mix profiles instead of placing all large tests in one block, unless
Polygon scoring groups require grouping.

### Subtask allocation

Every subtask must have meaningful coverage. When point weights are known, use them as a
starting signal for allocation, but still ensure each subtask has:

- boundary-size tests;
- structural edge cases;
- random/general tests;
- adversarial patterns relevant to the intended solution.

If subtasks overlap, a test tagged with one subtask must obey that subtask's exact
generator contract. If there are no partial subtasks and the user confirms a single
full-score group, use `subtask=1` consistently.

When `outputs/solution/manifest.md` contains WA/TLE candidates, allocate meaningful tests
to their documented adversarial profiles. Prefer profiles that distinguish several
realistic candidates at once, but ensure each candidate has at least one planned killing
test when feasible. Do not run a subtask-only AC outside its declared scope when
validating those plans.

### No literal final sizes

Do not write script parameters such as `--n=100000`, `--m=200000`, or `--k=17` when those
names are literal problem input values. Pass semantic profiles instead:

```text
--shape=line
--density=dense
--value-profile=maximum
--answer-profile=no
--tree-type=focused
```

`gentest.cpp` then chooses valid concrete values from `subtask` and `rate`. An exact
numeric option is acceptable only when it is genuinely a generation-control parameter
(for example a requested `numtest`) rather than a way to bypass generator logic.

### Seeds

Use deterministic distinct seeds by default — alphanumeric strings suitable for
Polygon/testlib command lines, e.g. `84GOGVE16X`, `807H8KNO3J`. Never rely on wall-clock
time.

```text
# WRONG
gentest --seed ABC123 --subtask 1 --rate 0.90 > $

# CORRECT
gentest ABC123 --subtask 1 --rate 0.90 > $
```

### Multi-test problems

Consider a `numtest` option/profile so tests cover one large case, many small cases, mixed
sizes, and the maximum allowed sum-of-sizes condition. `gentest.cpp` must still enforce the
global constraints.

## Part 2 — `outputs/gentest.cpp`

Load all applicable specialized generator skills alongside this one.

### Skeleton

```cpp
#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

using ll = long long;

int main(int argc, char* argv[]) {
    registerGen(argc, argv, 1);
    prepareOpts(argc, argv);

    int subtask = opt<int>("subtask");
    double rate = opt<double>("rate");

    ensure(0.70 <= rate && rate <= 1.00);

    // Generate a valid test.

    return 0;
}
```

Parse every named script option intentionally, and leave the seed as the fixed positional
argument.

### Shared numeric generation

Use this distribution helper as a base:

```cpp
vector<string> valueTypes({"minimum", "uniform", "logarit", "maximum"});

ll genValue(ll low, ll high, const string &type, int param = 3) {
    ensure(low <= high);

    if (low != 1) {
        return low - 1 + genValue(1, high - low + 1, type, param);
    }

    if (type == "uniform") {
        return rnd.next(low, high);
    }

    if (type == "logarit") {
        double x = low * exp(rnd.next() * log(1.0 * high / low));
        return (ll)max<double>(low, min<double>(high, x + 0.5));
    }

    if (type == "maximum") {
        return rnd.wnext(low, high, param);
    }

    if (type == "minimum") {
        ll ans = genValue(low, high, "logarit");
        for (int i = 0; i < param; ++i) {
            ans = min(ans, genValue(low, high, "logarit"));
        }
        return ans;
    }

    quitf(_fail, "unknown value type: %s", type.c_str());
}
```

Do not use `rnd.next(low, high)` for every variable. Mix boundary-biased, logarithmic,
uniform, and adversarial profiles as defined by `generator-config.md`.

### Applying `rate`

Do not treat `rate` as a blind multiplier for all fields. Recommended pattern for a
positive maximum `L`:

```cpp
ll scaledMax(ll L, double rate) {
    ensure(L >= 1);
    ll x = (ll)floor(L * rate + 0.5);
    return max(1LL, min(L, x));
}
```

Then generate inside the scaled legal interval. For constraints with nonzero lower bounds
or structural relations, derive a problem-specific formula. Examples:

- tree: `m` must remain `n - 1`;
- simple graph: `m <= n(n-1)/2`;
- queries: ensure each endpoint stays valid after `n` is chosen;
- multi-test: enforce total sum limits globally.

### Exact input order

Mirror the statement's Input section literally. For each line, decide explicitly: fields,
separator, trailing newline.

```cpp
cout << n << ' ' << q << '\n';

for (int i = 0; i < n; ++i) {
    if (i) cout << ' ';
    cout << a[i];
}
cout << '\n';
```

Avoid trailing spaces unless the format intentionally allows them.

### Multi-test generation

If the problem has `T`:

1. generate `T` or interpret `numtest`;
2. maintain remaining global budgets;
3. generate each case within both per-case and remaining-total constraints;
4. print `T` first;
5. reset per-case state correctly.

### Structural generators

Load specialized skills only when applicable:

- tree → `generator-tree`;
- general graph → `generator-graph`;
- arrays/permutations → `generator-array`;
- primes/divisibility → `generator-number-theory`;
- strings → `generator-string`.

Copy only the utilities actually needed into the final self-contained `gentest.cpp` unless
the Polygon package explicitly includes additional resource headers.

### Required validation before printing

Use `ensure(...)` for generator-internal invariants, including:

- subtask id valid;
- rate valid;
- every size within the selected subtask;
- vector/string lengths match declared sizes;
- endpoints/indices in range;
- all special structural constraints hold;
- global multi-test sums hold.

Prefer detecting a bad generator immediately to emitting an invalid Polygon test.

### C++ I/O convention

Emit the test with `cout` only. Do not use `printf`, `puts`, `putchar`, or any other C
stdio call; testlib's own helpers (`println`, `rnd`) remain fine. Separate tokens with
`' '` and end lines with `'\n'`.

## Common implementation traps

Avoid:

- declaring `rate` as `bool`;
- multiplying constraints in a way that violates special equalities;
- using exact `n`/`m` script parameters instead of generating them;
- accidental duplicate edges where forbidden;
- infinite rejection loops near maximum graph density;
- `for (auto x : container)` when intending to mutate elements — use `auto&`;
- overflow in `n * (n - 1) / 2` — promote to `long long`;
- shuffling labels in a way that destroys a required root/ordering property;
- printing extra spaces/newlines;
- time-based random seeds;
- adding `--seed` instead of using the required positional seed;
- writing the test with `printf`/`puts` instead of `cout`.

## Final verification

Script/generator consistency, before running anything:

- every named option used by the script is parsed by `gentest.cpp` and defined in
  `generator-config.md`;
- the first positional argument after `gentest` is always the seed;
- all subtask ids exist;
- every rate is in `[0.70, 1.00]`, about half `>= 0.90`;
- exactly 100 tests exist by default;
- lines use `> $` when the jury answer comes from the official solution;
- every WA/TLE target in `outputs/solution/manifest.md` is covered by a script profile or
  has a documented reason it cannot be targeted safely.

Then, for multiple representative script lines:

1. compile the generator;
2. run it;
3. run `outputs/validator.cpp` on the generated input and require acceptance;
4. run `source/solution.cpp` on the generated input when present;
5. repeat the same command and confirm identical generated input;
6. test subtask/rate boundaries.

Then execute the solution-kill matrix from `generator-config.md`:

1. run AC candidates only on generated tests inside their declared scopes and compare them
   with the official solution or semantic checker;
2. confirm each WA candidate is killed by at least one valid generated test when feasible;
3. stress each TLE candidate on its documented near-limit adversarial profile under the
   authoritative or announced provisional time limit;
4. record concrete killing seeds/profiles back in `generator-config.md`, or report why a
   safe distinction was not achieved.

When the validator rejects a generated test, the script, generator, or config is at fault.
Fix the earliest responsible one. Never weaken `outputs/validator.cpp`, weaken input
validity, rely on undefined behavior, or alter expected output semantics merely to kill a
candidate.
