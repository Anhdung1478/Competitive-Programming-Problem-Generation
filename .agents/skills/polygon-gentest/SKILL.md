---
name: polygon-gentest
description: Create or review outputs/gentest.cpp for Codeforces Polygon using testlib.h. Use for workflow Step 7 after generator-config.md and test-script.txt exist. Generates all concrete input values from semantic options such as subtask/rate/seed and delegates special structures to generator-tree/graph/array/number-theory/string skills.
---

# Write `gentest.cpp`

Read, in order:

1. `problem-context.md`;
2. `subtask.md` when present;
3. `outputs/solution/manifest.md` and referenced suite sources when present;
4. `statement.txt`;
5. `generator-config.md`;
6. `test-script.txt`;
7. `solution.cpp` when present.

If these artifacts disagree materially, stop and report the mismatch instead of choosing one silently.

## Required testlib form

The Polygon script must call the generator in this form:

```text
gentest <SEED> --subtask <S> --rate <R> > $
```

For example:

```text
gentest 84GOGVE16X --subtask 1 --rate 0.87 > $
```

`<SEED>` is the first positional command-line argument. It is deliberately **not** a named option; never introduce `--seed`.

Start from:

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

`registerGen(argc, argv, 1)` uses the command-line invocation to initialize testlib's deterministic random generator. The positional seed therefore does not need a separate `opt<...>("seed")` read.

Parse every named script option intentionally, but leave the seed as the fixed positional argument used by the generator invocation.

## Shared numeric generation

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

Do not use `rnd.next(low, high)` for every variable. Mix boundary-biased, logarithmic, uniform, and adversarial profiles as defined by `generator-config.md`.

## Applying `rate`

Do not treat `rate` as a blind multiplier for all fields.

Recommended pattern for a positive maximum `L`:

```cpp
ll scaledMax(ll L, double rate) {
    ensure(L >= 1);
    ll x = (ll)floor(L * rate + 0.5);
    return max(1LL, min(L, x));
}
```

Then generate inside the scaled legal interval.

For constraints with nonzero lower bounds or structural relations, derive a problem-specific formula.

Examples:

- tree: `m` must remain `n - 1`;
- simple graph: `m <= n(n-1)/2`;
- queries: ensure each endpoint stays valid after `n` is chosen;
- multi-test: enforce total sum limits globally.

## Exact input order

Mirror the Input section literally.

For each line, decide explicitly:

- fields;
- separator;
- trailing newline.

Preferred output pattern:

```cpp
cout << n << ' ' << q << '\n';

for (int i = 0; i < n; ++i) {
    if (i) cout << ' ';
    cout << a[i];
}
cout << '\n';
```

Avoid trailing spaces unless the format intentionally allows them.

## Multi-test generation

If the problem has `T`:

1. generate `T` or interpret `numtest`;
2. maintain remaining global budgets;
3. generate each case within both per-case and remaining-total constraints;
4. print `T` first;
5. reset per-case state correctly.

Profiles should include one-large, many-small, and mixed cases when legal.

## Structural generators

Load applicable specialized skills:

- tree → `generator-tree`;
- general graph → `generator-graph`;
- arrays/permutations → `generator-array`;
- primes/divisibility → `generator-number-theory`;
- strings → `generator-string`.

Copy only the utilities actually needed into the final self-contained `gentest.cpp` unless the Polygon package explicitly includes additional resource headers.

## Required validation before printing

Use `ensure(...)` for generator-internal invariants, including:

- subtask id valid;
- rate valid;
- every size within selected subtask;
- vector/string lengths match declared sizes;
- endpoints/indices in range;
- all special structural constraints hold;
- global multi-test sums hold.

Prefer detecting a bad generator immediately to emitting an invalid Polygon test.

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
- adding `--seed` instead of using the required positional seed.

## Final verification

For multiple representative script lines:

1. compile the generator;
2. run it;
3. validate input shape manually or with a temporary validator;
4. run `solution.cpp` on the generated input when present;
5. repeat the same command and confirm identical generated input;
6. test subtask/rate boundaries.

Then execute the solution-kill matrix from `generator-config.md`:

1. run AC candidates only on generated tests inside their declared scopes and compare them with the official solution or semantic checker;
2. confirm each WA candidate is killed by at least one valid generated test when feasible;
3. stress each TLE candidate on its documented near-limit adversarial profile under the authoritative or announced provisional time limit;
4. record concrete killing seeds/profiles back in `generator-config.md`, or report why a safe distinction was not achieved.

Never weaken input validity, rely on undefined behavior, or alter expected output semantics merely to kill a candidate.
