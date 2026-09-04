# Checker Contract and `problem.json` Interface — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make `outputs/problem.json` a read interface instead of a write-only artifact, and replace the "every package ships `checker.cpp`" rule with a stock-versus-custom decision that `uploading-to-polygon` can act on directly.

**Architecture:** This pack is prose — the "code" is the `SKILL.md` instructions an agent follows, so every task edits Markdown or YAML and every test is a `grep` whose output proves the instruction is present or gone. Tasks run in dependency order: the schema's home (`shaping-problems`) first, then its authority (`polygon-checker`), then the umbrella that sequences them, then the three consumers, then the docs.

**Tech Stack:** Markdown skill files, YAML config, `git`, `grep`, `claude plugin validate`.

**Spec:** `docs/superpowers/specs/2026-09-04-problem-json-interface-design.md`

## Global Constraints

- The stock allowlist is exactly these tokens, verbatim with `std::` prefix and `.cpp` suffix, and no others appear anywhere in the pack: `std::ncmp.cpp`, `std::wcmp.cpp`, `std::lcmp.cpp`, `std::fcmp.cpp`, `std::rcmp4.cpp`, `std::rcmp6.cpp`, `std::rcmp9.cpp`, `std::yesno.cpp`, `std::nyesno.cpp`, `std::hcmp.cpp`.
- `problem.json.checker` has exactly two shapes: `{ "kind": "stock", "name": "<token>" }` and `{ "kind": "custom", "file": "checker.cpp" }`. No third shape, no extra keys.
- Stock is permitted **only** when the output is unique for every valid input **and** one listed token compares it exactly. A doubtful case is custom.
- `uploading-to-polygon` writes `outputs/polygon.json` and never writes `problem.json`.
- Numeric bounds in `validator.cpp` and `gentest.cpp` are **out of scope** — they belong to the `constraints.h` codegen in `2026-09-04-tools-layer-and-audit-design.md`. Do not add bounds-reading instructions to `polygon-validator` or `generating-tests` in this plan.
- Polygon test groups stay disabled. `subtasks[].points` drives statement prose and the sum check only.
- Every `grep` in this plan runs from the repository root, `E:/teaching-document/Competitive-Programming-Problem-Generation`.
- Commit after each task. Do not squash tasks into one commit.

---

### Task 1: `shaping-problems` — the schema's home

**Files:**
- Modify: `skills/shaping-problems/SKILL.md` (JSON example ~line 129; field table ~lines 155-170; self-check list)

**Interfaces:**
- Consumes: nothing.
- Produces: the `checker` two-shape schema and the stock token allowlist that Tasks 2, 3 and 5 all cite; the "never writes `problem.json`" sentence Task 5 mirrors.

- [ ] **Step 1: Write the failing check**

Run:
```bash
grep -c "std::ncmp.cpp" skills/shaping-problems/SKILL.md
grep -c 'Never written here' skills/shaping-problems/SKILL.md
```
Expected now: `0` for the first (the token form is absent) and `1` for the second (the false `polygon` row is present). Both are wrong; the task inverts them.

- [ ] **Step 2: Update the JSON example**

Replace:
```
  "checker": { "kind": "stock", "name": "ncmp" },
```
with:
```
  "checker": { "kind": "stock", "name": "std::ncmp.cpp" },   // provisional; Step 3 confirms
```

- [ ] **Step 3: Replace the two checker rows in the field table**

Replace these two rows:
```
| `checker.kind` | `"stock"` unless several outputs are valid, then `"custom"` (written later by `polygon-checker`) |
| `checker.name` | `ncmp` integer sequences, `wcmp` tokens, `rcmp6` reals to 1e-6. Pick by what the output **is**, and check the magnitude — an answer reaching 2·10¹⁰ is not a 32-bit token |
```
with:
```
| `checker` | Exactly two shapes: `{ "kind": "stock", "name": "<token>" }` or `{ "kind": "custom", "file": "checker.cpp" }`. **Provisional here** — `polygon-checker` re-decides it at Step 3 against the finished statement and overwrites |
| `checker.name` | The verbatim Polygon token, `std::` prefix and `.cpp` suffix included: `std::ncmp.cpp` int64 sequences, `std::wcmp.cpp` tokens, `std::lcmp.cpp` lines of tokens, `std::fcmp.cpp` exact lines, `std::rcmp4.cpp`/`std::rcmp6.cpp`/`std::rcmp9.cpp` reals to 1e-4/1e-6/1e-9, `std::yesno.cpp`/`std::nyesno.cpp` YES-NO, `std::hcmp.cpp` one bignum. Pick by what the output **is**, and check the magnitude — an answer reaching 2·10¹⁰ is not a 32-bit token. An output no token compares exactly is `"custom"` |
```

- [ ] **Step 4: Extend the limits row**

Replace:
```
| `limits.time_ms_published` | The proposal from §2. The measured limit comes later |
```
with:
```
| `limits.time_ms_published` | The proposal from §2, and the single home for the limit. Workflow Step 4 may raise or lower it once it has measured one, and announces the change |
```

- [ ] **Step 5: Delete the false `polygon` row and state the truth**

Delete this row entirely:
```
| `polygon` | Never written here. `uploading-to-polygon` adds it after the problem exists |
```

Then, immediately before the `### Bounds the source does not state` heading, add this paragraph:
```
`uploading-to-polygon` records the Polygon id in `outputs/polygon.json` and never
writes `problem.json`. No key here tracks the upload.
```

- [ ] **Step 6: Add a self-check line**

In the `## Self-check before reporting` list, after `- [ ] Every \`min\`/\`max\` is an integer`, add:
```
- [ ] `checker` uses one of the two shapes, and any `name` is a token from the table
```

- [ ] **Step 7: Run the checks to verify they pass**

Run:
```bash
grep -c "std::ncmp.cpp" skills/shaping-problems/SKILL.md
grep -c 'Never written here' skills/shaping-problems/SKILL.md
grep -n "never" skills/shaping-problems/SKILL.md | grep -i polygon.json
```
Expected: first `>= 1`; second `0`; third prints the new "never writes `problem.json`" sentence.

- [ ] **Step 8: Commit**

```bash
git add skills/shaping-problems/SKILL.md
git commit -m "shaping-problems: two-shape checker schema, drop the false polygon row"
```

---

### Task 2: `polygon-checker` — the authority

**Files:**
- Modify: `skills/polygon-checker/SKILL.md` (title area and the `## Always write this file` section; `## Final review` list)

**Interfaces:**
- Consumes: the two-shape schema and token allowlist from Task 1.
- Produces: the decision rule Task 3's Step 3 section points at, and the "`checker.cpp` exists iff custom" invariant Tasks 3 and 5 enforce.

- [ ] **Step 1: Write the failing check**

Run:
```bash
grep -c "Always write this file" skills/polygon-checker/SKILL.md
grep -c "Decide first" skills/polygon-checker/SKILL.md
```
Expected now: `1` and `0`. The task inverts both.

- [ ] **Step 2: Replace the opening and the "Always write this file" section**

Replace everything from the line `# Write \`outputs/checker.cpp\`` through the paragraph ending `...named token readers, \`_wa\`/\`_fail\`/\`_ok\`, rejecting extra output.` with:

````markdown
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
````

- [ ] **Step 3: Add the invariant to the final review**

In `## Final review`, add as the first bullet:
```
- `outputs/problem.json` carries the decision, and `outputs/checker.cpp` exists if and only if `kind` is `"custom"`.
```

- [ ] **Step 4: Run the checks to verify they pass**

Run:
```bash
grep -c "Always write this file" skills/polygon-checker/SKILL.md
grep -c "Decide first — stock or custom" skills/polygon-checker/SKILL.md
grep -c 'stock checker' skills/polygon-checker/SKILL.md
```
Expected: `0`, `1`, `0`.

- [ ] **Step 5: Commit**

```bash
git add skills/polygon-checker/SKILL.md
git commit -m "polygon-checker: decide stock vs custom before writing a file"
```

---

### Task 3: `creating-problems` — step order and the gate

**Files:**
- Modify: `skills/creating-problems/SKILL.md` (Purpose list; the "Every other step is mandatory" paragraph; `## Step 3`; Step 4's `### Time-limit policy`; `## Cross-artifact consistency gate`)

**Interfaces:**
- Consumes: Task 2's decision rule and Task 1's schema.
- Produces: the gate bullets that make a wrong `checker` a reported defect; the `limits.time_ms_published` write that Task 5's preflight reads.

- [ ] **Step 1: Write the failing check**

Run:
```bash
grep -c "Step 3 always produces" skills/creating-problems/SKILL.md
grep -c "record it prominently in .outputs/solution/manifest.md" skills/creating-problems/SKILL.md
```
Expected now: `1` and `1`. Both become `0`.

- [ ] **Step 2: Update the Purpose list**

Replace:
```
3. create `outputs/checker.cpp`;
```
with:
```
3. decide the checker, and create `outputs/checker.cpp` when that decision is custom;
```

- [ ] **Step 3: Replace the mandatory-checker paragraph**

Replace:
```
Every other step is mandatory. In particular **Step 3 always produces
`outputs/checker.cpp`**, including when the output is unique and a Polygon standard
checker such as `wcmp`/`ncmp` would compare it correctly: the package ships its own
checker so the uploaded problem never depends on a `std::` checker. The uniqueness
analysis still matters — it decides what the checker does, not whether it exists.
```
with:
```
Every other step is mandatory. In particular **Step 3 always produces a checker
decision** in `outputs/problem.json`: `{ "kind": "stock", "name": "<token>" }` or
`{ "kind": "custom", "file": "checker.cpp" }`. The file itself follows only on the
custom branch — an output that is unique for every valid input and that a Polygon
standard checker compares exactly binds that token instead, and `outputs/checker.cpp`
is not written. The uniqueness analysis is what decides the branch.
```

- [ ] **Step 4: Rewrite the Step 3 section's opening**

Replace:
```
Use the `polygon-checker` skill. This step is mandatory: every package ships
`outputs/checker.cpp`, which is what the Polygon problem binds as its checker.

First classify the output from the finished `outputs/statement.txt`, because it decides
what the checker does:

- unique/deterministic output (single correct answer, exact formatting, no accepted
  alternative, no floating-point tolerance beyond exact match) → a minimal comparison
  checker with the tokenization the statement implies — token-by-token for
  numbers/words, line-by-line when whole lines matter, a documented tolerance for
  floating point. Keep it as small as the comparison actually needs; do not add
  semantic validation the statement does not ask for.
- non-unique witnesses, optimization/construction objectives, or any special comparison
  protocol → a semantic checker, per the requirements below.

Requirements:
```
with:
```
Use the `polygon-checker` skill. This step is mandatory, but its deliverable is the
**decision**, not necessarily a file.

Classify the output from the finished `outputs/statement.txt`, then record the result in
`outputs/problem.json`:

- unique for every valid input, and one Polygon standard checker compares it exactly →
  `{ "kind": "stock", "name": "<token>" }`. No file is written. The allowlist of tokens
  lives in `polygon-checker`; nothing outside it may be named here.
- non-unique witnesses, optimization/construction objectives, any special comparison
  protocol, any semantic validation, or a unique output that no token fits →
  `{ "kind": "custom", "file": "checker.cpp" }`, and `outputs/checker.cpp` is written
  per the requirements below. A doubtful case is custom.

Step 0 wrote a provisional `checker` before the statement existed; Step 3 overwrites it
and reports any disagreement.

Requirements for a custom checker:
```

- [ ] **Step 5: Replace the time-limit policy**

Replace:
```
Treat a time limit in `source/problem-context.md` as authoritative. If it is absent, choose a provisional solution-testing time limit based on the constraints, intended complexity, and local benchmark evidence. Announce that choice to the user and record it prominently in `outputs/solution/manifest.md`. A provisional limit is only a testing assumption: do not add it to the statement or present it as authoritative metadata.
```
with:
```
Treat a time limit in `source/problem-context.md` as authoritative. Otherwise the limit lives in `outputs/problem.json` as `limits.time_ms_published`, proposed by Step 0. When measurement here shows that proposal is wrong — the intended solution does not fit it, or it is loose enough that a TLE candidate survives — update `limits.time_ms_published` and announce the change. `outputs/problem.json` is the single home for the limit: do not record a second one in `outputs/solution/manifest.md`, and do not put a limit in the statement that the source files do not state.
```

- [ ] **Step 6: Replace the checker bullet in the cross-artifact gate**

Replace:
```
- `outputs/checker.cpp` exists and its semantics == statement output semantics, including tokenization and any floating-point tolerance;
```
with:
```
- `outputs/problem.json` carries a `checker` in one of its two shapes, and it matches reality: on `"custom"`, `outputs/checker.cpp` exists and its semantics == statement output semantics, including tokenization and any floating-point tolerance; on `"stock"`, the named token's comparison == the statement's output format and no `outputs/checker.cpp` is present;
- the decision is `"custom"` whenever the statement admits more than one correct output;
- `outputs/statement.txt`'s limits, I/O mode, and problem name agree with `outputs/problem.json`;
```

- [ ] **Step 7: Run the checks to verify they pass**

Run:
```bash
grep -c "Step 3 always produces a checker" skills/creating-problems/SKILL.md
grep -c "record it prominently in .outputs/solution/manifest.md" skills/creating-problems/SKILL.md
grep -c 'is present' skills/creating-problems/SKILL.md
```
Expected: `1`, `0`, `1`.

- [ ] **Step 8: Commit**

```bash
git add skills/creating-problems/SKILL.md
git commit -m "creating-problems: checker decision replaces mandatory checker.cpp; limits live in problem.json"
```

---

### Task 4: `polygon-statement` — first consumer

**Files:**
- Modify: `skills/polygon-statement/SKILL.md` (after the "This skill owns problem prose" line; the `## Subtasks` section)

**Interfaces:**
- Consumes: `problem.json`'s `name`, `title.vi`, `io`, `limits`, `subtasks[].id`, `subtasks[].points` from Task 1.
- Produces: nothing other tasks read.

- [ ] **Step 1: Write the failing check**

Run:
```bash
grep -c "problem.json" skills/polygon-statement/SKILL.md
```
Expected now: `0`.

- [ ] **Step 2: Add a "Read first" section**

Immediately after the line `This skill owns problem prose and notation only. It does not invent algorithmic constraints or test data.`, insert:

````markdown

## Read first

1. `source/problem-context.md` — source of truth for the problem, its constraints, and its subtask ladder.
2. `outputs/problem.json` — Step 0's shaped numbers. Take from it: `name` and `title.vi` for the heading, `io` for whether the problem reads `stdin`/`stdout` or a named `.inp`/`.out` pair, `limits` when the statement quotes them, and each rung's `id` and `points` for the Subtask section.
3. `source/solution.cpp` when present — for notation only.

`problem-context.md` wins on conflict. Where it and `problem.json` disagree, write what the markdown says and report the conflict; do not edit either file to make them agree.
````

- [ ] **Step 3: Point the Subtasks section at the shaped points**

Replace:
```
Copy constraints and scoring from `problem-context.md`; do not recalculate or “improve” them.
```
with:
```
Copy constraints from `problem-context.md`, and take each rung's percentage from `outputs/problem.json`: `subtasks[].points` against `polygon.total_points` in `preference.yml`, so a 40-point rung out of 100 prints as $40\%$. Do not recalculate or “improve” either.
```

- [ ] **Step 4: Run the check to verify it passes**

Run:
```bash
grep -c "outputs/problem.json" skills/polygon-statement/SKILL.md
```
Expected: `>= 3`.

- [ ] **Step 5: Commit**

```bash
git add skills/polygon-statement/SKILL.md
git commit -m "polygon-statement: read problem.json for name, io, limits and rung points"
```

---

### Task 5: `uploading-to-polygon` — the consumer that acts

**Files:**
- Modify: `skills/uploading-to-polygon/SKILL.md` (source-of-truth line ~46; precondition table ~52-60; preflight items 1 and 4; Phase 2 limits table; Phase 4 item 3; Phase 5 "Points, no groups"; the Done checklist)

**Interfaces:**
- Consumes: the two-shape `checker` (Task 1/2), `limits.time_ms_published` and `limits.memory_mb` (Task 3), `format`, `io`, `subtasks[].points`.
- Produces: `outputs/polygon.json`, unchanged.

- [ ] **Step 1: Write the failing check**

Run:
```bash
grep -c "there is no stock-checker path here" skills/uploading-to-polygon/SKILL.md
grep -c "problem.json" skills/uploading-to-polygon/SKILL.md
```
Expected now: `1` and `0`.

- [ ] **Step 2: Add `problem.json` to the source-of-truth order**

Replace:
```
Source of truth, in this order: `source/problem-context.md` (constraints and subtask
scoring included), `outputs/statement.txt`, `outputs/solution/manifest.md`.
```
with:
```
Source of truth, in this order: `source/problem-context.md` (constraints and subtask
scoring included), `outputs/problem.json` (limits, I/O mode, `format`, the checker
decision, subtask points), `outputs/statement.txt`, `outputs/solution/manifest.md`.
```

- [ ] **Step 3: Update the precondition table**

Replace:
```
| `outputs/checker.cpp` | yes |
```
with:
```
| `outputs/problem.json` | yes — carries limits, `io`, `format` and the checker decision |
| `outputs/checker.cpp` | only when `outputs/problem.json` says `"kind": "custom"` |
```

- [ ] **Step 4: Extend preflight items 1 and 4**

Replace:
```
1. The required files above exist; the package passed its consistency gate.
```
with:
```
1. The required files above exist; the package passed its consistency gate.
   `outputs/problem.json` loads, and its `checker` is one of the two shapes —
   `{ "kind": "stock", "name": "<token>" }` or `{ "kind": "custom", "file": "checker.cpp" }`.
```

Replace:
```
4. **Limits.** Read the time and memory limit from `source/problem-context.md`.
   If they are absent, ask. A *provisional* limit recorded in
   `outputs/solution/manifest.md` is a testing assumption only — never upload it
   as the published limit without the user confirming it.
```
with:
```
4. **Limits.** Read the time and memory limit from `source/problem-context.md`;
   when it states none, read `limits.time_ms_published` and `limits.memory_mb`
   from `outputs/problem.json`. If neither has them, ask. A limit that Step 0
   proposed and Step 4 never measured is an assumption — confirm it with the
   user before publishing it.
```

- [ ] **Step 5: Extend the Phase 2 limits table**

Replace:
```
| `input_file` / `output_file` | the I/O the statement promises — `"stdin"`/`"stdout"` unless `source/problem-context.md` names files (a `.inp`/`.out` pair, matching the guarded `freopen` block the solutions use) |
| `time_limit` (ms) | `source/problem-context.md` |
| `memory_limit` (MB) | `source/problem-context.md` |
```
with:
```
| `input_file` / `output_file` | `io` in `outputs/problem.json` — `"stdin"`/`"stdout"`, or the `.inp`/`.out` pair it names, matching the guarded `freopen` block the solutions use |
| `time_limit` (ms) | `source/problem-context.md`, else `limits.time_ms_published` in `outputs/problem.json` |
| `memory_limit` (MB) | `source/problem-context.md`, else `limits.memory_mb` in `outputs/problem.json` |
```

- [ ] **Step 6: Replace Phase 4 item 3 with the two branches**

Replace:
```
3. **Checker.** `outputs/checker.cpp` is required — every package has one, so
   there is no stock-checker path here. `save_problem_file(file_type="source",
   file_name="checker.cpp", local_path="outputs/checker.cpp")`, then
   `set_problem_checker(problem_id, "checker.cpp")`. Same self-containment rule
   as the validator: `testlib.h` is the only include Polygon resolves. If
   `outputs/checker.cpp` is missing, **stop** — run
   `cp-problem-generation:polygon-checker` first; do not substitute a
   `std::` checker on your own.
```
with:
```
3. **Checker.** Read `checker` from `outputs/problem.json` and take the matching
   branch. Never decide this here: a package whose `checker` key is missing or
   malformed is unfinished — **stop** and run
   `cp-problem-generation:polygon-checker`.

   - `{ "kind": "custom", "file": "checker.cpp" }` →
     `save_problem_file(file_type="source", file_name="checker.cpp",
     local_path="outputs/checker.cpp")`, then
     `set_problem_checker(problem_id, "checker.cpp")`. Same self-containment rule
     as the validator: `testlib.h` is the only include Polygon resolves. If the
     file is missing, stop.
   - `{ "kind": "stock", "name": "std::ncmp.cpp" }` →
     `set_problem_checker(problem_id, "std::ncmp.cpp")` with the token passed
     through **verbatim**. Upload no checker source, and expect no
     `outputs/checker.cpp` in the package. **Read the MCP tool's schema before
     the call** — if it rejects a `std::` name, fall back to the raw Polygon HTTP
     API `problem.setChecker` with `checker=std::ncmp.cpp`, which accepts standard
     checker names.
```

- [ ] **Step 7: State the scoring divergence in Phase 5**

Replace:
```
**Points, no groups.** The pack's statements carry `% số điểm` subtasks, so this
is an OI problem: `enable_problem_points(problem_id, enable=true)`. Only when the
user says the problem is ICPC-style do you leave points off — and then skip
`scoring` too.
```
with:
```
**Points, no groups.** `format` in `outputs/problem.json` decides: `"oi"` →
`enable_problem_points(problem_id, enable=true)`; `"icpc"` → leave points off, and
skip `scoring` too.
```

Then, immediately after the `**Never enable test groups.**` paragraph, add:
```
**This divergence is deliberate, and contestants can see it.** `subtasks[].points`
in `outputs/problem.json` sets the statement's `% số điểm` prose and the sum check;
it is never uploaded as group points. A contestant who clears only `g1` therefore
earns one point per g1 test passed, not the 40 the ladder advertises. Do not "fix"
that during an upload by enabling groups — per-test scoring is the pack's choice,
and changing it is a change to this skill, not to one problem.
```

- [ ] **Step 8: Update the Done checklist**

Replace:
```
- [ ] Limits match `source/problem-context.md` (TL, ML, I/O names); no
      provisional limit published without confirmation
```
with:
```
- [ ] Limits match `source/problem-context.md`, else `outputs/problem.json`
      (TL, ML, I/O names); no unconfirmed provisional limit published
```

Replace:
```
- [ ] `validator.cpp`, `gentest.cpp` and `checker.cpp` uploaded and bound
```
with:
```
- [ ] `validator.cpp` and `gentest.cpp` uploaded and bound; the checker bound per
      `outputs/problem.json` — `checker.cpp` uploaded on `"custom"`, the `std::`
      token bound with no upload on `"stock"`
```

- [ ] **Step 9: Run the checks to verify they pass**

Run:
```bash
grep -c "there is no stock-checker path here" skills/uploading-to-polygon/SKILL.md
grep -c "outputs/problem.json" skills/uploading-to-polygon/SKILL.md
grep -c "problem.setChecker" skills/uploading-to-polygon/SKILL.md
```
Expected: `0`, `>= 8`, `1`.

- [ ] **Step 10: Commit**

```bash
git add skills/uploading-to-polygon/SKILL.md
git commit -m "uploading-to-polygon: bind stock or custom checker from problem.json"
```

---

### Task 6: `preference.yml` — Step 0 and Step 9

**Files:**
- Modify: `preference.yml` (`never_relaxed` list; `overrides` map; the `polygon` block's comment)

**Interfaces:**
- Consumes: Task 2's decision rule (the `never_relaxed` line must match it exactly).
- Produces: `step0_shaping` overrides that `shaping-problems` readers honor.

- [ ] **Step 1: Write the failing check**

Run:
```bash
grep -c "step0_shaping" preference.yml
grep -c 'is written in both profiles' preference.yml
```
Expected now: `0` and `1`.

- [ ] **Step 2: Rewrite the `never_relaxed` checker entry and add Step 0's**

Replace:
```
      - "Step 3's checker: `outputs/checker.cpp` is written in both profiles. Only its content scales with the output's complexity, never its existence."
```
with:
```
      - "Step 3's checker decision is recorded in `outputs/problem.json` in both profiles. Non-unique, optimization, or special-protocol output always gets a written `outputs/checker.cpp`; only an output that is unique for every valid input and that a listed `std::` token compares exactly may go stock."
      - "Step 0's separation table, and its two blocking findings — the problem is already known, and the ladder collapses or the intended solution is not needed. Both stop for the user in both profiles."
```

- [ ] **Step 3: Add the `step0_shaping` override**

In `overrides`, immediately before `step1_validate_solution`, add:
```
      step0_shaping:
        - "The originality verdict may be one sentence from recall rather than a systematic archive pass. It is still stated either way."
        - "Where `problem-context.md` states no point weights, propose a split and proceed under a stated assumption instead of blocking on approval."
```

- [ ] **Step 4: Rewrite the `step3_checker` override**

Replace:
```
      step3_checker:
        - "Lean toward the minimal comparison checker whenever the statement plausibly implies unique output; do not spend effort on a careful deterministic/non-unique/optimization classification pass. Ask only if it is genuinely unclear whether output is unique."
```
with:
```
      step3_checker:
        - "Lean toward a stock token whenever the statement plausibly implies unique output; do not spend effort on a careful deterministic/non-unique/optimization classification pass. Ask only if it is genuinely unclear whether output is unique."
```

- [ ] **Step 5: Record that Step 9 has no overrides**

At the end of the `light-weight` profile's `overrides` map, after the `cross_artifact_gate` entry, add:
```
      step9_upload:
        - "Nothing is relaxed. The upload mirrors the finished package in both profiles."
```

- [ ] **Step 6: Extend the `polygon` block's comment**

Replace:
```
  # Total score of the problem, split equally over the non-sample tests
  # (100 points / 100 generated tests = 1 point each). Sample tests are never
  # graded: they stay at 0 points. Tests are ungrouped.
```
with:
```
  # Total score of the problem, split equally over the non-sample tests
  # (100 points / 100 generated tests = 1 point each). Sample tests are never
  # graded: they stay at 0 points. Tests are ungrouped.
  #
  # `subtasks[].points` in outputs/problem.json must sum to this value, but those
  # weights are statement prose only — they are never uploaded as Polygon group
  # points. Polygon awards by tests passed.
```

- [ ] **Step 7: Verify the YAML still parses and the checks pass**

Run:
```bash
python -c "import yaml,sys; yaml.safe_load(open('preference.yml',encoding='utf-8')); print('yaml ok')"
grep -c "step0_shaping" preference.yml
grep -c "step9_upload" preference.yml
grep -c 'is written in both profiles' preference.yml
```
Expected: `yaml ok`, `1`, `1`, `0`. If `python` is absent, skip the parse and inspect indentation by eye — every `stepN_*` key sits at the same column.

- [ ] **Step 8: Commit**

```bash
git add preference.yml
git commit -m "preference.yml: Step 0 and Step 9 profiles, checker rule follows the new policy"
```

---

### Task 7: `README.md` and the whole-pack sweep

**Files:**
- Modify: `README.md` (Step 0 and Step 3 rows of the skills table; the `outputs/` tree's `checker.cpp` line)

**Interfaces:**
- Consumes: every preceding task.
- Produces: the final consistency evidence.

- [ ] **Step 1: Write the failing check**

Run:
```bash
grep -c "Always written: the package ships its own checker" README.md
```
Expected now: `1`.

- [ ] **Step 2: Update the Step 3 row**

Replace:
```
| 3 | `polygon-checker` | Writes `outputs/checker.cpp` with testlib — deterministic, constructive, optimization, and special-output checkers. Always written: the package ships its own checker rather than binding a Polygon `std::` checker |
```
with:
```
| 3 | `polygon-checker` | Decides the checker and records it in `outputs/problem.json`: a `std::` token when the output is unique and a standard checker compares it exactly, otherwise `outputs/checker.cpp` written with testlib — non-unique witnesses, optimization, construction, special output. The file exists only on the custom branch |
```

- [ ] **Step 3: Update the Step 0 row**

Replace the trailing text of the Step 0 row:
```
into `outputs/problem.json`. Its separation table can find that two rungs collapse into one, or that a simpler algorithm clears the full constraints; both are blocking findings for the user to resolve |
```
with:
```
into `outputs/problem.json` — which later steps read for the problem name, I/O mode, limits, rung points, and the provisional checker choice. Its separation table can find that two rungs collapse into one, or that a simpler algorithm clears the full constraints; both are blocking findings for the user to resolve |
```

- [ ] **Step 4: Mark `checker.cpp` conditional in the tree**

Replace:
```
   ├─ checker.cpp                    # Step 3
```
with:
```
   ├─ checker.cpp                    # Step 3 — custom checkers only
```

- [ ] **Step 5: Run the whole-pack sweep**

Run each and confirm the expected result:
```bash
# 1. no surviving claim that checker.cpp is unconditional
grep -rn "always produces .outputs/checker.cpp\|Always write this file\|every package ships\|Always written: the package ships" skills README.md
# expected: no output

# 2. no surviving claim that the upload writes problem.json
grep -rn "adds it after the problem exists" skills
# expected: no output

# 3. every std:: token in the pack is on the allowlist
grep -rho "std::[a-z0-9]*\.cpp" skills README.md preference.yml | sort -u
# expected: only ncmp, wcmp, lcmp, fcmp, rcmp4, rcmp6, rcmp9, yesno, nyesno, hcmp

# 4. the plugin still validates
claude plugin validate . --strict
# expected: passes
```

If sweep 4's command is unavailable in this environment, record that and move on; sweeps 1-3 are the load-bearing ones.

- [ ] **Step 6: Commit**

```bash
git add README.md
git commit -m "README: checker is a decision, problem.json feeds later steps"
```

---

## Done criteria

- `grep -rn "std::" skills` names only allowlist tokens.
- No file in `skills/` or `README.md` says `outputs/checker.cpp` is unconditional.
- `outputs/problem.json` is read by `polygon-statement`, `polygon-checker`, `creating-problems` Step 4, and `uploading-to-polygon`, and written by `shaping-problems` and `polygon-checker` only.
- `preference.yml` has a defined answer for Step 0 and Step 9.
- Seven commits, one per task.
