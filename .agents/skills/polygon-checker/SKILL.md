---
name: polygon-checker
description: Create or review outputs/checker.cpp for Codeforces Polygon using testlib.h, including deterministic, constructive, optimization, and special-output checkers.
---

# Write `outputs/checker.cpp`

Read the source-of-truth files and final output specification. Classify the output as deterministic, non-unique witness, optimization, floating-point, or special protocol before implementing the checker.

## Skip this file when output is unique

If the output is unique/deterministic for every valid input — a single correct answer, exact required formatting, no accepted alternative, no floating-point tolerance beyond exact match — do not write `outputs/checker.cpp`. Use a Polygon standard checker instead (`wcmp`/`ncmp`/`hcmp` for token-by-token numeric/word comparison, `lcmp`/`fcmp` for exact line/text comparison, or an `rcmp*` variant when a documented floating-point tolerance applies) and report the chosen standard checker instead of producing a file. Only continue past this point when the output is non-unique, an optimization/construction objective, or otherwise needs custom comparison logic.

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

- Every `ouf`/`ans` token reader has both a pattern and variable name, or is a typed bounded reader with a variable name.
- Missing and extra output are rejected.
- Jury failures use `_fail`; participant failures use `_wa`.
- Checker semantics exactly match the statement, including non-unique witnesses and optimization priority.
