---
name: polygon-checker
description: Create or review outputs/checker.cpp for Codeforces Polygon using testlib.h, including deterministic, constructive, optimization, and special-output checkers.
---

# Write the checker

Read the source-of-truth files and the finished `outputs/statement.txt`. Classify the output before writing anything: deterministic, non-unique witness, optimization, floating-point, or special protocol.

## Decide first — stock or custom

Not every problem needs a written checker. Record the decision in `outputs/problem.json`, in one of exactly two shapes:

```jsonc
"checker": { "kind": "stock",  "name": "std::ncmp.cpp" }
"checker": { "kind": "custom", "file": "checker.cpp" }
```

`name` is the verbatim Polygon token — `std::` prefix and `.cpp` suffix included — so `uploading-to-polygon` hands it straight to `set_problem_checker` without assembling a string.

**Stock is permitted only when the output is unique for every valid input *and* one of these tokens compares it exactly:**

| Token | Use when |
|---|---|
| `std::ncmp.cpp` | one or more int64 in sequence; whitespace-insensitive |
| `std::wcmp.cpp` | sequence of tokens/words; whitespace-insensitive |
| `std::lcmp.cpp` | line by line, tokens compared within each line; whitespace-insensitive |
| `std::fcmp.cpp` | line by line, lines compared exactly; whitespace significant |
| `std::rcmp4.cpp` / `std::rcmp6.cpp` / `std::rcmp9.cpp` | sequence of doubles, abs/rel error 1e-4 / 1e-6 / 1e-9 |
| `std::yesno.cpp` / `std::nyesno.cpp` | one / a sequence of case-insensitive YES-NO answers |
| `std::hcmp.cpp` | one huge signed integer |

Write `outputs/checker.cpp` — `"kind": "custom"` — whenever any of these hold:

- more than one output is accepted (non-unique witness);
- the objective is optimization or construction;
- the output follows a special comparison protocol;
- the checker must validate anything semantic about the answer;
- the output is unique but no token fits it — an unusual tolerance, a mixed-format line, a leading count whose value constrains the rest of the output.

A doubtful case is custom. An unnecessary `checker.cpp` costs one file; a stock token that accepts a wrong answer costs the problem.

`shaping-problems` wrote a provisional `checker` at Step 0 from `problem-context.md` alone, before the statement existed. You decide it here, against the finished statement, and overwrite it. **If your decision differs from that proposal, say so in your report** — an output that turned out to accept several answers is a fact the author should hear.

When the decision is stock you are done: no file is written, and `outputs/checker.cpp` must not exist in the package. Everything below applies to the custom branch only.

## Writing a custom checker

When the output is unique but no token fits, write the minimal comparison the statement implies — token by token for numbers and words, line by line when whole lines matter, or a documented tolerance for floating point. Keep it that small: no semantic validation the statement does not ask for, no re-solving the problem. Everything below applies to every custom checker, minimal or semantic.

Use C++17, `testlib.h`, and `registerTestlibCmd(argc, argv)`. Treat participant data as untrusted. Report participant errors with `_wa`, jury/package inconsistencies with `_fail`, and success with `_ok`.

## Token readers required by Polygon

Whenever reading a token from `ouf` or `ans`, use the named pattern overload:

```cpp
string token = ouf.readToken("[0-9]+", "answer");
string expected = ans.readToken("-?[0-9]+", "jury answer");
```

Do not use unnamed `readToken()` or `readWord()` calls. Polygon emits review warnings for them because they provide neither an accepted format nor a variable name. Choose the narrowest testlib pattern that matches the statement. Give repeated values meaningful names such as `path vertex`, `edge endpoint`, or `answer token`.

Prefer typed readers such as `readInt(min, max, variableName)` or `readLong(min, max, variableName)` when numeric normalization is intended. If textual representation itself matters, use `readToken(pattern, variableName)` and validate/convert it safely.

To reject extra output, checking `!ouf.seekEof()` is sufficient; do not consume the extra token with an unnamed reader merely for diagnostics.

## Deterministic output

Compare every required participant token with the corresponding jury token, reject missing or differing tokens, then require participant EOF. Use named pattern readers for both streams.

## Non-unique and optimization output

Parse the original input, validate the participant witness semantically, recompute all claimed properties, and consume the complete participant output. Never compare a valid witness structurally with the jury witness. Use the jury output only for information genuinely required from it, such as a validated optimum.

For paths and other bounded sequences, validate counts before allocation, validate every element range, verify adjacency/constraints, guard accumulated arithmetic against overflow, and reject surplus output.

## C++ I/O convention

Read exclusively through the `inf`/`ouf`/`ans` testlib streams and report through `quitf`. Do not use `scanf`, `printf`, `fscanf`, `getchar`, or any other C stdio call anywhere in the checker; use `cout` if a plain stream write is ever needed.

## Final review

- `outputs/problem.json` carries the decision, and `outputs/checker.cpp` exists if and only if `kind` is `"custom"`.
- Every `ouf`/`ans` token reader has both a pattern and variable name, or is a typed bounded reader with a variable name.
- Missing and extra output are rejected.
- Jury failures use `_fail`; participant failures use `_wa`.
- Checker semantics exactly match the statement, including non-unique witnesses and optimization priority.
