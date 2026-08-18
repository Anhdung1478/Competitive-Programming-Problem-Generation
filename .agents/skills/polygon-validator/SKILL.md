---
name: polygon-validator
description: Create or review outputs/validator.cpp for a Codeforces Polygon problem using testlib.h. Use for workflow Step 8, generated-input validation, strict input-format enforcement, bounds checks, subtask/group constraints, or structural invariant validation. This validates input files, not contestant output.
---

# Write `outputs/validator.cpp`

Read, in order:

1. `source/problem-context.md`;
2. `source/subtask.md` when present;
3. `outputs/statement.txt`;
4. `outputs/generator-config.md`;
5. `outputs/test-script.txt`;
6. `outputs/gentest.cpp`.

If these files disagree on input order, bounds, test-case structure, or invariants, stop and report the mismatch. Source-of-truth files win.

## Required testlib form

Create a standalone C++17 Polygon input validator:

```cpp
#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char* argv[]) {
    registerValidation(argc, argv);

    // Read and validate the complete input with inf.

    inf.readEof();
    return 0;
}
```

Use `inf.readInt`, `inf.readLong`, `inf.readToken`, or another typed `testlib` reader with explicit bounds and variable names. Use `readSpace`, `readEoln`, and `readEof` to enforce the declared layout. Do not use `cin`, random generation, jury answers, or participant output.

Write large C++ numeric literals with digit separators so Polygon's validator review does not warn about unreadable constants. Use forms such as `1'000`, `1'000'000`, and `-1'000'000` instead of `1000`, `1000000`, and `-1000000`; preserve the exact numeric value and type.

## Validate all semantics

Check every property required by the source specification, including when applicable:

- field order and exact number of tokens/lines;
- inclusive numeric bounds with overflow-safe intermediate arithmetic;
- array/string lengths and alphabets;
- index and interval relations;
- distinctness, ordering, permutation, or frequency constraints;
- graph direction, loops, parallel edges, simplicity, connectivity, and edge count;
- tree/root/parent invariants;
- per-test and global sum limits in multi-test input;
- EOF-terminated record formats;
- every other structural invariant promised by the statement.

Use `ensuref(...)` for relations that typed readers cannot express. Include concise failure messages naming the violated invariant and relevant values.

For Polygon groups/subtasks, use `validator.group()` only when group-specific validator invocation is configured and supported by the repository's `testlib.h`. Otherwise validate the full legal input domain here and keep subtask-membership checks in generator validation. Never reject a globally valid test merely because no group argument was supplied.

## Strict formatting

Mirror `outputs/statement.txt` exactly:

- call `readSpace()` between fields on one line;
- call `readEoln()` after each required line;
- read exactly the declared number of repeated records;
- call `readEof()` after the final record;
- for EOF-terminated formats, loop only while another complete record is present, then require EOF and reject partial records or extra tokens.

Do not silently accept a second test case, trailing garbage, missing values, or surplus values.

## Validation workflow

Compile with the same `testlib.h` environment used by Polygon when available. Then:

1. run the validator on representative outputs from every `test-script.txt` profile;
2. cover minimum, maximum, each subtask, and structural edge cases;
3. confirm every sampled generated test is accepted;
4. create temporary invalid mutations for below/above-bound values, missing/extra tokens, malformed spacing/lines, and broken structural invariants;
5. confirm each invalid mutation is rejected;
6. run every script command twice when useful to ensure validation does not depend on external state.

Do not add temporary invalid files to the repository. If `testlib.h` is unavailable, perform static review and clearly report that compilation/runtime validation remains pending.

## Final checks

Verify:

- validator semantics equal the source-of-truth input semantics;
- statement input order equals validator read order;
- generator output is accepted for every sampled profile;
- selected-subtask constraints are checked somewhere in the validation workflow;
- all required input is consumed exactly once;
- numeric bounds and constants use readable C++ digit separators where applicable;
- no output-validation logic from `checker.cpp` appears in this file.
