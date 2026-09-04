# Design: the checker contract and `problem.json` as an interface

Date: 2026-09-04
Status: approved (design); implementation plan to follow

Addresses findings 1, 2 and 4 from the 2026-09-04 pack review, plus the half of
finding 3 that does not belong to the tools-layer spec. Bounds codegen, Polygon test
groups and difficulty recognition are out of scope (§8).

---

## 1. Problem

Three defects, one root cause: `outputs/problem.json` is written by `shaping-problems`
and then read by nobody.

- **The checker decision contradicts itself.** `shaping-problems` writes
  `"checker": { "kind": "stock", "name": "ncmp" }`, while `creating-problems` Step 3,
  `polygon-checker` and `uploading-to-polygon` all state that every package ships its
  own `outputs/checker.cpp` and that no stock checker path exists. Step 0 records a
  decision Step 3 is forbidden to honor. Every problem therefore pays for a hand-written
  checker, including the ones whose output a one-line Polygon standard checker compares
  exactly.
- **The upload's state file is misdescribed.** `shaping-problems`' field table says
  `uploading-to-polygon` adds a `polygon` key to `problem.json`. It does not; it writes a
  separate `outputs/polygon.json` and never touches `problem.json`.
- **`preference.yml` is a step behind.** Its overrides cover `step1`…`step8`. Step 0 and
  Step 9 did not exist when it was written, so it is undefined whether the separation
  table survives the `light-weight` profile, and its `never_relaxed` checker line asserts
  the unconditional-`checker.cpp` rule this design removes.

The unifying fix is to give `problem.json` real consumers, so a field that disagrees with
the pipeline is a defect somebody notices.

## 2. Approach

`problem.json` becomes the machine-readable interface between the steps that decide
numbers and the steps that act on them — but only where no other design already owns the
field.

- **Decided here:** the checker choice, the time and memory limits, `io`, `format`, the
  problem name, and the subtask ids and points.
- **Deliberately not decided here:** the numeric bounds in `validator.cpp` and
  `gentest.cpp`. The approved tools-layer spec routes those through generated
  `outputs/constraints.h`. Two designs owning one field is the drift this one is trying
  to remove.

Rejected: making every skill read `problem.json` now (rewrites prose the `constraints.h`
work would rewrite again), and leaving `problem.json` write-only while fixing the three
defects in place (the contradictions come back the next time a field is added).

## 3. The checker contract

### 3.1 Schema

`problem.json.checker` takes exactly one of two shapes:

```jsonc
"checker": { "kind": "stock",  "name": "std::ncmp.cpp" }
"checker": { "kind": "custom", "file": "checker.cpp" }
```

`name` is the **verbatim Polygon token**, `std::` prefix and `.cpp` suffix included, so
the upload passes it straight to `set_problem_checker` and never assembles a string from
a bare `ncmp`.

### 3.2 The stock allowlist

These are the only tokens the pack may choose. A requirement that no row satisfies is a
custom checker.

| Token | Use when |
|---|---|
| `std::ncmp.cpp` | one or more int64 in sequence; whitespace-insensitive |
| `std::wcmp.cpp` | sequence of tokens/words; whitespace-insensitive |
| `std::lcmp.cpp` | line by line, tokens compared within each line; whitespace-insensitive |
| `std::fcmp.cpp` | line by line, lines compared exactly; whitespace significant |
| `std::rcmp4.cpp` / `rcmp6.cpp` / `rcmp9.cpp` | sequence of doubles, abs/rel error 1e-4 / 1e-6 / 1e-9 |
| `std::yesno.cpp` / `std::nyesno.cpp` | one / a sequence of case-insensitive YES-NO answers |
| `std::hcmp.cpp` | one huge signed integer |

### 3.3 The rule

**Stock is permitted only when the output is unique for every valid input *and* one of
the tokens above compares it exactly.**

Custom — `outputs/checker.cpp` is written — whenever any of these hold:

- more than one output is accepted (non-unique witness);
- the objective is optimization or construction;
- the output follows a special comparison protocol;
- the checker must validate anything semantic about the answer;
- the output is unique but no token fits it — an unusual tolerance, a mixed-format line,
  a count whose value constrains the rest of the output.

A doubtful case is custom. The cost of an unnecessary `checker.cpp` is one file; the cost
of a stock token that accepts a wrong answer is a broken problem.

### 3.4 Ownership: Step 0 proposes, Step 3 confirms

`shaping-problems` writes a provisional `checker` from `problem-context.md`, so
`problem.json` is complete from Step 0 and a partially-run pipeline has something to read.
The schema documents the field as provisional.

`polygon-checker` (Step 3) is authoritative. It re-classifies against the finished
`outputs/statement.txt` — the wording is what determines uniqueness, and it does not exist
at Step 0 — then overwrites the field. **A disagreement between the two is reported, not
silently resolved**, because a statement that turned out to accept several answers is a
fact the author should hear.

## 4. Skill changes

### 4.1 `polygon-checker`

The "Always write this file" section becomes **"Decide first — stock or custom"**,
carrying §3.2's table and §3.3's rule, and requiring the decision be written into
`problem.json.checker`.

Everything from "Use C++17, `testlib.h`, and `registerTestlibCmd`" onward — token readers,
`_wa`/`_fail`/`_ok`, deterministic and non-unique sections, the C++ I/O convention, the
final review — applies **only on the custom branch** and is otherwise unchanged.

The existing unique-output paragraph survives as the fallback custom-minimal-checker case
of §3.3's last bullet, not as the default it is today.

### 4.2 `creating-problems`

Step 3 no longer "always produces `outputs/checker.cpp`". It always produces a **checker
decision**; the file follows only when `kind == "custom"`. The paragraph asserting that the
package never binds a `std::` checker is replaced by §3.3's rule.

Step 4's time-limit policy stops recording a provisional limit in
`outputs/solution/manifest.md` and instead updates `limits.time_ms_published` in
`problem.json`, announcing the change. The limit then has one home, which is also what the
tools-layer spec §5.1 assumes.

Cross-artifact gate, changed bullets:

- the `checker.cpp` bullet splits on `kind` — **custom**: the file exists and its semantics
  match the statement's output semantics, tokenization and tolerance included; **stock**:
  the token's comparison matches the statement's output format, and the output really is
  unique;
- new: `kind` is `"custom"` whenever the output is non-unique;
- new: statement limits, `io` and problem name agree with `problem.json`;
- the existing points-sum bullet is unchanged.

### 4.3 `shaping-problems`

- The `polygon` row leaves the field table. New sentence: `uploading-to-polygon` writes
  `outputs/polygon.json` and never writes `problem.json`.
- The `checker.kind` and `checker.name` rows are rewritten for §3.1-§3.3 and marked
  provisional per §3.4.
- `limits.time_ms_published` keeps its "proposal from §2" wording, extended: Step 4 may
  raise or lower it once measured.

### 4.4 `polygon-statement`

Reads `problem.json` for the problem name and title, `io`, the limits, and the subtask ids
and points behind the `\item Subtask $i$ ($x\%$ số điểm)` lines. `problem-context.md`
remains source of truth: on disagreement the markdown wins and the conflict is reported.

### 4.5 `uploading-to-polygon`

Reads `problem.json` for limits (Phase 2), `io`, `format` (whether to call
`enable_problem_points`), `checker` (Phase 4) and the points-sum check.

Phase 4.3 becomes two branches:

- `custom` → `save_problem_file(file_type="source", file_name="checker.cpp",
  local_path="outputs/checker.cpp")` then `set_problem_checker(problem_id, "checker.cpp")`,
  exactly as today, self-containment rule included;
- `stock` → `set_problem_checker(problem_id, <name>)` with the token passed through
  verbatim. No file upload, and `outputs/checker.cpp` is expected **not** to exist.

A missing or malformed `checker` key stops the upload and sends the user to
`polygon-checker`. The precondition table changes `outputs/checker.cpp` from *required* to
*required only when `problem.json` says `custom`*.

**Unverified at design time:** whether cf-polygon-mcp's `set_problem_checker` accepts a
`std::` name. The skill will require reading the tool schema at call time and, if the MCP
tool rejects it, falling back to the raw Polygon HTTP API `problem.setChecker` with
`checker=std::ncmp.cpp`, which does accept standard checker names.

Phase 5 is unchanged in behavior and clarified in wording: **ungrouped, equal per-test
points stays.** `subtasks[].points` drives the statement's Scoring prose and the sum check
and is never mirrored into Polygon group points; Polygon awards by tests passed, so a
contestant who clears only `g1` scores the g1 tests, not the g1 weight. The "Never enable
test groups" block keeps its instruction and gains that consequence in writing, so the
divergence is a stated decision rather than something a reader has to infer.

### 4.6 `preference.yml`

- New `step0_shaping` overrides under `light-weight`: the originality verdict may be one
  sentence from recall rather than a systematic archive pass; where `problem-context.md`
  states no point weights, propose a split and proceed under a stated assumption instead
  of blocking on approval.
- `never_relaxed` gains Step 0's separation table and its two blocking findings — the
  problem is already known, and the ladder collapses or the intended solution is not
  needed.
- `never_relaxed`'s checker line is rewritten: the checker **decision** is recorded in
  `problem.json` in both profiles, and non-unique, optimization or special-protocol output
  always gets a written `outputs/checker.cpp`. Only a genuinely unique output that a
  listed token compares exactly may go stock.
- `step3_checker`'s override changes from "lean toward the minimal comparison checker" to
  "lean toward a stock token", with the same reasoning and the same "ask only if it is
  genuinely unclear whether output is unique" escape.
- One line recording that Step 9 has no profile overrides: the upload mirrors the package
  in both profiles.

### 4.7 `README.md`

The Step 0 and Step 3 rows of the skills table are updated: Step 0 shapes a provisional
checker choice among its numbers; Step 3 decides stock versus custom and writes
`outputs/checker.cpp` only for custom. The `outputs/` tree annotates `checker.cpp` as
conditional.

## 5. Data flow

```
problem-context.md ──► shaping-problems ──► problem.json
                                              │  (checker: provisional)
                        ┌─────────────────────┼──────────────────┐
                        ▼                     ▼                  ▼
                polygon-statement      polygon-checker      solution suite
              name/io/limits/points          │ overwrites      limits.time_ms
                                             │ checker
                                             ▼
                                   checker.cpp  (custom only)
                                             │
                                             ▼
                                   uploading-to-polygon
                          limits, io, format, checker, points sum
                                             │
                                             ▼
                                     outputs/polygon.json
```

## 6. Testing

No executable code changes, so verification is documentary:

- `claude plugin validate . --strict` passes;
- a grep sweep finds no surviving claim that `outputs/checker.cpp` is unconditional, and
  no surviving reference to a `polygon` key inside `problem.json`;
- every stock token named anywhere in the pack appears in §3.2's allowlist;
- the two `checker` shapes in §3.1 are the only ones any skill instructs an agent to write.

## 7. Success criteria

- A unique-output problem uploads with `set_problem_checker(problem_id,
  "std::ncmp.cpp")` and no `outputs/checker.cpp` in the package.
- A non-unique-output problem still ships and binds its own `outputs/checker.cpp`.
- Step 3 changing the Step 0 proposal produces a reported disagreement, not a silent
  overwrite.
- Every field this design touches — `checker`, `limits`, `io`, `format`, `name`, `title`,
  `subtasks[].points` — has a named consumer in §5. Fields it does not touch (`schema`,
  `tags`, `constraints`, `examples`) are left as they are.
- `light-weight` has a defined answer for Step 0 and Step 9.
- No skill states that `uploading-to-polygon` writes `problem.json`.

## 8. Out of scope

- **Numeric bounds in `validator.cpp` / `gentest.cpp`.** Owned by the tools-layer spec's
  `constraints.h` codegen.
- **Polygon test groups.** Considered and declined: the ungrouped equal-per-test split
  stays, documented rather than changed.
- **Difficulty recognition.** Its own design.
