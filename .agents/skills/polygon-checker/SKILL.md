---
name: polygon-checker
description: Create or review checker.cpp for a Codeforces Polygon problem using testlib.h. Use for workflow step B2, unique-output comparison, non-unique constructive outputs, optimization witnesses, or any custom output validation.
---

# Write `checker.cpp`

Read `problem-context.md`, `solution.cpp` when present, `subtask.md` when relevant, and the current output specification.

Before coding, classify output as one of:

1. **unique/deterministic** — every correct solution should produce equivalent fixed tokens;
2. **non-unique witness** — many outputs may be valid;
3. **optimization** — many witnesses, but an objective must match/beat a required value;
4. **floating-point** — tolerance matters;
5. **special protocol** — output contains conditional branches, certificates, or unusual formatting.

If the classification or tolerance is not determined by the source files, ask the user before writing the checker.

## Polygon/testlib skeleton

A checker must:

```cpp
#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    // inf: input
    // ouf: participant output
    // ans: jury answer

    // validation...

    quitf(_ok, "accepted");
}
```

Use `quitf(_wa, ...)` for a wrong participant answer and `_fail` for an inconsistent/broken jury package.

Checker diagnostics should be short and in English.

## Default checker: unique output

When the statement has no special output semantics, use a token-wise comparison that ignores irrelevant whitespace but rejects any missing, different, or extra token:

```cpp
#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char* argv[]) {
    registerTestlibCmd(argc, argv);

    int tokenId = 0;
    while (!ans.seekEof()) {
        ++tokenId;

        if (ouf.seekEof()) {
            quitf(_wa, "participant output ended before token %d", tokenId);
        }

        string expected = ans.readToken();
        string found = ouf.readToken();

        if (found != expected) {
            quitf(
                _wa,
                "token %d differs: expected '%s', found '%s'",
                tokenId,
                expected.c_str(),
                found.c_str()
            );
        }
    }

    if (!ouf.seekEof()) {
        string extra = ouf.readToken();
        quitf(_wa, "extra participant output starts with '%s'", extra.c_str());
    }

    quitf(_ok, "outputs match");
}
```

If numeric textual normalization matters (for example `01` versus `1`), parse typed numbers instead of comparing strings. Do not choose this silently; it changes semantics.

## Non-unique witness checker

Never require the participant witness to equal the jury witness.

Instead:

1. parse the original input from `inf`;
2. parse participant output from `ouf`;
3. validate ranges/counts/format;
4. validate every constraint of the witness;
5. recompute the property claimed by the participant;
6. reject extra tokens unless permitted;
7. use `ans` only for information that truly must come from the jury, such as a known optimum.

Examples of non-unique outputs:

- any valid path;
- any valid matching;
- any edge orientation;
- any permutation meeting constraints;
- any partition/coloring;
- any certificate.

## Optimization checker

Typical pattern:

1. read input;
2. read/recompute the participant objective and witness;
3. validate the witness;
4. read the jury optimum from `ans`;
5. compare objectives exactly or with the specified tolerance;
6. return `_fail` if the jury package is internally inconsistent.

Never accept an invalid witness merely because its objective matches the jury.

## Floating point

Tolerance must come from the problem specification or user confirmation.

Use testlib numeric readers and an explicit absolute/relative error rule. Document that rule in `statement.txt`.

## Final self-review

Ensure:

- no legal output is rejected because of irrelevant ordering unless ordering is required;
- participant cannot exploit missing tokens, duplicates, out-of-range indices, NaN/Inf, overflow, or extra output;
- all data read from `ouf` is treated as untrusted;
- jury inconsistencies lead to `_fail`, not `_wa`;
- checker semantics exactly match `statement.txt`.
