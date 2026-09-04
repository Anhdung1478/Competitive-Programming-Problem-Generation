---
name: shaping-problems
description: >
  Use when a problem idea needs its numbers settled before prose or code
  exists — is this already a known problem, what N separates the intended
  solution from the naive one, OI or ICPC, which subtask rungs earn points,
  and what goes in outputs/problem.json. Triggers on shape this problem,
  pick N, chọn N, subtask ladder, thang subtask, partial scoring, chấm từng
  phần, point weights, is this problem already known, problem.json. Do not
  use for statement wording (polygon-statement) or test data
  (generator-config).
---

# Shaping problems

Turn `source/problem-context.md` into `outputs/problem.json` — the numbers,
made machine-readable, before any artifact is generated from them.

Four judgements, in this order: **known problem? → N that separates →
format and scoring → the JSON.** The order matters: there is no point
costing a subtask ladder for a problem you are about to retarget.

## Am I the right skill?

| If it's really about | Use |
|---|---|
| The statement's wording | `polygon-statement` |
| Auditing `source/solution.cpp` | `validate-solution` |
| Which tests to generate | `generator-config` |
| The whole pipeline | `creating-problems` |

## Read first

1. `source/problem-context.md` — required; it carries the constraints and the
   subtask ladder as prose. This skill converts them, it does not replace them.
2. `source/solution.cpp` when present — the intended complexity in the flesh.
3. `preference.yml` — `polygon.total_points` is the score the rungs must sum
   to (default 100). Do not re-ask what it answers.

`problem.json` never overrides `problem-context.md`. When they disagree, the
markdown wins and you report the conflict.

## 1. Originality

Strip the flavor. Name the operation in one sentence — "count pairs in a range
whose sum is divisible by k", not "shortest flight path". Then say whether you
recognize it from Codeforces, AtCoder, or the OI archives.

**State a verdict either way.** One of:

- `Originality: not recognized as a known problem.`
- `Originality: this is <name/shape>, a standard <technique> exercise.`

If it is known, stop and let the user choose: proceed anyway, retarget the
constraints, or drop the idea. Do not make that call yourself.

## 2. The separation table — REQUIRED

Fill in one row per rung **before** writing any points. This table is the
deliverable of this step; a ladder proposed without it is not shaped.

| Rung | Bound | Naive ops here | Intended ops here | First algorithm fast enough |
|---|---|---|---|---|
| g1 | n,q ≤ 1000 | q·n² = 10⁹ ✗ | q·n = 10⁶ ✓ | per-query residue counting |
| g2 | n,q ≤ 2·10⁵ | q·n = 4·10¹⁰ ✗ | (n+q)√n = 10⁸ ✓ | Mo's algorithm |

Rules for filling it in:

- Budget is ~10⁸ operations/second against the **published time limit**. Write
  the arithmetic, not an adjective: `4·10¹⁰` beats "too slow".
- Do this for **every** size parameter that moves — `n`, `q`, `k`, alphabet,
  value range — not only `n`.
- The last column names a **concrete algorithm**. "Brute force", "an optimized
  approach", or "rules out the naive ones" is not a name; if you cannot name
  it, you have not checked the rung.

**Two rows naming the same algorithm are one rung.** Merge them, or move a
bound until a different algorithm appears in that column. A rung that only
shrinks `N` while the same algorithm still clears it pays a contestant for
typing.

This applies to the ladder already written in `problem-context.md`. It arrives
as a proposal, not as a fact: if two of its rungs collapse into one row here,
say so and propose the merge. Report what the table shows even when it
contradicts the source — that is the finding, not a problem with the table.

The table can also show that the **stated intended solution is not needed** —
a simpler algorithm clears the last rung too. That is the same finding one
column over, and it is worth more than the ladder: report it, propose the
constraint change that would restore the separation, and let the user decide.
Do not edit `source/problem-context.md` yourself; it is a source-of-truth file.

## 3. Format and scoring

**ICPC** — binary. One subtask `g1` worth the full total, no subtask table in
the statement. `"format": "icpc"`.

**OI** — partial points. `"format": "oi"`.

Write the answer into the file. A single 100-point group cannot tell OI-without-
subtasks from ICPC, so OI with one group must say `"format": "oi"` explicitly.

For the points themselves, key off what the source says:

- **`problem-context.md` states weights** — copy them. Check they sum to
  `polygon.total_points`; if not, report the gap and stop.
- **It does not** — propose a split, show the table from §2 with a `points`
  column added, and **wait for approval before writing the file**. Bias toward
  the insight you want to reward: the first non-trivial insight often earns
  20-40, the intended rung takes the remainder. An even split across rungs is
  a sign you skipped this judgement.

A rung that only catches overflow or `n = 1` is a **test**, not a subtask. Say
so and drop it from the ladder.

## 4. `outputs/problem.json`

You write this file; the user never types JSON.

```jsonc
{
  "schema": 1,
  "name": "flight",                    // short ascii slug, no spaces
  "title": { "vi": "Chuyến bay đầu tiên" },
  "tags": ["number theory"],
  "format": "oi",                      // "oi" | "icpc"
  "limits": { "time_ms_published": 1000, "memory_mb": 256 },
  "io": { "input": "stdin", "output": "stdout" },
  "checker": { "kind": "stock", "name": "std::ncmp.cpp" },   // provisional; Step 3 confirms
  "constraints": [
    { "id": "n", "expr": "1 \\le n \\le 200000", "min": 1, "max": 200000 }
  ],
  "subtasks": [
    { "id": "g1", "points": 40,
      "bounds": { "n": { "max": 1000 } },
      "constraints_text": ["$n \\le 1000$"],
      "depends_on": [] },
    { "id": "g2", "points": 60,
      "bounds": {},
      "constraints_text": ["Không có ràng buộc gì thêm"],
      "depends_on": [] }
  ],
  "examples": []
}
```

**Write these keys and no others.** An invented key (`titleVi`,
`derived_facts`, `notes`) is invisible to every consumer and silently loses
whatever you put in it. Findings go in your report, not in the document.

| Field | Rule |
|---|---|
| `constraints[].min` / `max` | JSON **integers**. Never a float — these become C++ `long long`, where `2.9` truncates to `2` and `0.5` to `0` |
| `constraints[].expr` | The bound as statement math, backslashes escaped for JSON |
| `subtasks[].bounds` | Narrows a **global constraint by its `id`**; an id not in `constraints` is an error. `{}` means "no extra bounds" |
| `subtasks[].constraints_text` | Contest-language prose for the statement's subtask table. Required, even when `bounds` is `{}` |
| `subtasks[].points` | Sums to `polygon.total_points` across all rungs |
| `depends_on` | Polygon **scoring order** — not "this rung's bounds are a superset". Empty is normal and is the right default |
| `checker` | Exactly two shapes: `{ "kind": "stock", "name": "<token>" }` or `{ "kind": "custom", "file": "checker.cpp" }`. **Provisional here** — `polygon-checker` re-decides it at Step 3 against the finished statement and overwrites |
| `checker.name` | The verbatim Polygon token, `std::` prefix and `.cpp` suffix included: `std::ncmp.cpp` int64 sequences, `std::wcmp.cpp` tokens, `std::lcmp.cpp` lines of tokens, `std::fcmp.cpp` exact lines, `std::rcmp4.cpp`/`std::rcmp6.cpp`/`std::rcmp9.cpp` reals to 1e-4/1e-6/1e-9, `std::yesno.cpp`/`std::nyesno.cpp` YES-NO, `std::hcmp.cpp` one bignum. Pick by what the output **is**, and check the magnitude — an answer reaching 2·10¹⁰ is not a 32-bit token. An output no token compares exactly is `"custom"` |
| `io.input` / `io.output` | Both sentinels (`"stdin"`/`"stdout"`) or both bare filenames (`flight.inp`/`flight.out`). Never one of each, never a path separator |
| `limits.time_ms_published` | The proposal from §2, and the single home for the limit. Workflow Step 4 may raise or lower it once it has measured one, and announces the change |
| `examples` | `[]` here. Workflow Step 5b fills the sample files |

`uploading-to-polygon` records the Polygon id in `outputs/polygon.json` and never
writes `problem.json`. No key here tracks the upload.

### Bounds the source does not state

A necessary bound that `problem-context.md` omits — the legal range of a query
`l, r`, say — is a **finding, not a blank to fill**.

Do exactly this: **leave it out of `constraints[]`, and ask about it in your
report.** An absent entry makes the next step ask you; a guessed entry makes
the validator enforce your guess. `1 ≤ l < r ≤ n` looks harmless and silently
rejects every legal `l = r` query.

Do not split the difference by encoding a "safe" superset and mentioning the
doubt underneath — a bound in the file is a bound the validator will use,
whatever the report says about it.

## Self-check before reporting

- [ ] Originality verdict stated, either way
- [ ] Separation table filled, with arithmetic and a named algorithm per rung
- [ ] No two rungs name the same algorithm
- [ ] `points` sum to `polygon.total_points`
- [ ] Every `subtasks[].bounds` key exists in `constraints[]`
- [ ] Every `min`/`max` is an integer
- [ ] `checker` uses one of the two shapes, and any `name` is a token from the table
- [ ] No keys outside the table above
- [ ] Unstated bounds raised as questions, not encoded as guesses

## Common mistakes

| Mistake | What it looks like |
|---|---|
| Justifying a rung instead of testing it | "raises n so the naive is too slow" — with no operation count, and the naive was already too slow one rung earlier |
| Transcribing the source ladder | The markdown's rungs copied straight into `subtasks[]` without a separation table |
| Equal splits | 25/25/25/25 across four rungs — the arithmetic of not deciding |
| Vague algorithm names | "brute force" and "optimized brute force" as two different rungs |
| Silent invention | A query-range bound, a tag, or a time limit that appears in the JSON but nowhere in the source |
