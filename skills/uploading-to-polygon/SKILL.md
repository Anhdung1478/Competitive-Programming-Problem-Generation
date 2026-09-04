---
name: uploading-to-polygon
description: >
  Upload a finished outputs/ package to Codeforces Polygon through the external
  cf-polygon-mcp server — create the problem (or report that it already exists),
  push the Vietnamese statement with its Scoring section, upload validator.cpp,
  gentest.cpp, the checker named by problem.json (a std:: token or checker.cpp),
  and every solution tagged with its intended verdict,
  set time/memory limits, upload samples, load test-script.txt, wire subtasks
  and per-test points, commit without email, build the package, and hand over
  the manual "grant codeforces READ" step. Triggers on upload this problem to
  polygon, upload to codeforces, push to polygon, đẩy lên polygon, take it to
  polygon, sync to polygon. Runs after `creating-problems` Step 8 and its
  cross-artifact consistency gate.
---

# Uploading to Polygon

Ship a finished `outputs/` package to Codeforces Polygon via the **cf-polygon-mcp**
server. You do **not** regenerate tests, rewrite the statement, or fix solutions
here; you mirror what the package already says. Do not spawn subagents.

This skill owns the **upload only**. Shaping, solutions, tests, statement and
editorial belong to the sibling skills — a package that is not finished is sent
back to them, not patched up here.

## Am I the right skill?

| If it's really about | Use |
|---|---|
| The statement prose | `cp-problem-generation:polygon-statement` |
| A printable/PDF statement | `cp-problem-generation:tex-statement` |
| The checker | `cp-problem-generation:polygon-checker` |
| The validator | `cp-problem-generation:polygon-validator` |
| The test plan / script / generator | `cp-problem-generation:generator-config`, `cp-problem-generation:generating-tests` |
| Building the whole package | `cp-problem-generation:creating-problems` |
| Pushing an already-finished package to Polygon | this skill |

Use `yagni-principles` here too: upload what the package contains, nothing more.

## Bootstrap

```text
PROBLEM = working repository root (the one with source/ and outputs/)
```

Source of truth, in this order: `source/problem-context.md` (constraints and subtask
scoring included), `outputs/problem.json` (limits, I/O mode, `format`, the checker
decision, subtask points), `outputs/statement.txt`, `outputs/solution/manifest.md`. **Never invent a number
the package does not state** — no time limit, no memory limit, no subtask points.

**Precondition — the package is finished.** These must exist and be current:

| File | Required |
|---|---|
| `outputs/statement.txt` | yes |
| `outputs/validator.cpp` | yes |
| `outputs/gentest.cpp`, `outputs/test-script.txt` | yes |
| `outputs/solution/` + `manifest.md` | yes (at least one full-scope AC) |
| `outputs/problem.json` | yes — carries limits, `io`, `format` and the checker decision |
| `outputs/checker.cpp` | only when `outputs/problem.json` says `"kind": "custom"` |
| `outputs/example-test/test_<i>.inp` + `.out` | yes (1-2 pairs; these are the statement samples) |
| `outputs/editorial.html` | not uploaded; ignore here |

If any required file is missing or the `creating-problems` cross-artifact
consistency gate has not passed, **stop** and run `creating-problems` first.
Upload is not a substitute for review.

**Precondition — the MCP server is connected.** cf-polygon-mcp's tools
(`create_problem`, `get_problems`, `update_problem_info`,
`save_problem_statement`, `save_problem_file`, `save_problem_solution`,
`set_problem_validator`, `set_problem_checker`, `save_problem_script`,
`save_problem_test`, `get_problem_tests`, `enable_problem_points`,
`check_problem_readiness`, `commit_problem_changes`,
`build_problem_package_and_wait`, …) must be visible in your tool catalog.
**Read the actual schemas before calling** — never guess a parameter name.
Credentials are `POLYGON_API_KEY` / `POLYGON_API_SECRET` in the server's own env;
if the server reports them missing, stop and tell the user. This plugin ships no
MCP config of its own — the server is configured by the host (Claude Code
`.mcp.json` / settings, or Cursor `.cursor/mcp.json`). In Cursor the entry also
needs the dependency pin `"--with", "mcp<2"` when the ambient Python resolves
mcp 2.x, and must be toggled on in the MCP server list before its tools resolve.

If the server is not connected at all: offer the manual fallback at the end,
phase by phase, and stop. Never invent tool calls against a server you cannot see.

Every write returns a structured envelope: `status` (`success`/`error`), `action`,
`message`, `result`, `error`; workflows add `stage`, `decision`, `can_retry`,
`recovery_actions`. **Read them.** A `status: error` after a create/commit/build
is a stop-and-look, not a shrug.

---

## Pipeline

```
  0 preflight            [block until answered]
  1 create in Polygon    [gate: id recorded, block]
  2 limits
  3 statement (type english, content in Vietnamese)
  4 checker + validator + generator + solutions
  5 tests (samples, script, per-test points)
  6 readiness -> commit (no email) -> build package
  7 grant codeforces READ  [manual, owner only]
```

Phase 1 is the one blocking gate: the problem must **exist** on Polygon and its
id must be recorded before any other phase runs.

---

### 0. Preflight (block)

1. The required files above exist; the package passed its consistency gate.
   `outputs/problem.json` loads, and its `checker` is one of the two shapes —
   `{ "kind": "stock", "name": "<token>" }` or `{ "kind": "custom", "file": "checker.cpp" }`.
2. The MCP server is connected and its tools resolve.
3. **Polygon name.** The package has no slug field. Derive a candidate from the
   working directory name (lowercase ASCII, digits and hyphens only) and
   **confirm it with the user** before creating anything. The statement's
   Vietnamese title (`\textbf{Tên bài:}`) is a separate thing — it becomes the
   statement `name` in Phase 3, not the Polygon slug.
4. **Limits.** Read the time and memory limit from `source/problem-context.md`;
   when it states none, read `limits.time_ms_published` and `limits.memory_mb`
   from `outputs/problem.json`. If neither has them, ask. A limit that Step 0
   proposed and Step 4 never measured is an assumption — confirm it with the
   user before publishing it.
5. **Already uploaded?** If `outputs/polygon.json` exists, this package already
   has a Polygon id. Ask: re-sync that same problem (update info, statement,
   files, solutions, tests, commit again) or stop. Never silently create a
   second problem.
6. **Samples.** Read `outputs/example-test/` — workflow Step 5b produced the
   `test_<i>.inp`/`test_<i>.out` pairs, and those are the samples. Upload them
   as-is; do not re-derive answers or substitute inputs of your own. If the
   directory is missing or an `.out` is absent, the package is unfinished: stop
   and send it back to Step 5b rather than picking samples here.
7. State the bindings, the confirmed slug and the limits in the first status
   update.

---

### 1. Create in Polygon (gate, block)

Prefer **MCP** `get_problems` / `create_problem`. Raw HTTP
`problem.createProblem` / `problem.list` can 404 or time out while MCP still
works — do not call the API "down" until MCP also fails.

1. `get_problems(name=<slug>)`.
   - A non-deleted problem with that name exists → **stop** and tell the user:
     *the Polygon problem already exists with id `<id>`*. Never create a
     duplicate, never guess a new slug on your own.
2. Otherwise `create_problem(name=<slug>)`.
   - `status: success` → `result.id` (and `result.owner`) is your id.
   - `status: error` saying the name exists → same stop as above.
   - Any other `status: error` → report and stop.
3. **Record the id** in `outputs/polygon.json` — the only file this skill writes:

```json
{ "id": 123456, "owner": "nudetiger", "name": "sum-of-paths" }
```

The gate closes when that file exists and loads.

---

### 2. Limits

`update_problem_info(problem_id, …)`:

| field | from |
|---|---|
| `input_file` / `output_file` | `io` in `outputs/problem.json` — `"stdin"`/`"stdout"`, or the `.inp`/`.out` pair it names, matching the guarded `freopen` block the solutions use |
| `time_limit` (ms) | `source/problem-context.md`, else `limits.time_ms_published` in `outputs/problem.json` |
| `memory_limit` (MB) | `source/problem-context.md`, else `limits.memory_mb` in `outputs/problem.json` |
| `interactive` | `false` — this pack does not model interactive problems |

---

### 3. Statement — type `english`, content in Vietnamese

`save_problem_statement(problem_id, lang="english", …)`: the statement
**type/language slot is always `english`**, because Codeforces contest import
reads that slot. The **content is not translated** — paste the Vietnamese prose
from `outputs/statement.txt` exactly as authored. Only `lang` stays `english`.

`outputs/statement.txt` is already Polygon simpleTex, so it transfers verbatim.
Split it on its section headers:

| `outputs/statement.txt` | Polygon field |
|---|---|
| `\textbf{Tên bài:}` value | `name` |
| `\textbf{Đề bài}` body, including `\textbf{Yêu cầu:}` | `legend` |
| `\textbf{Input}` body | `input` |
| `\textbf{Output}` body | `output` |
| `\textbf{Subtask}` itemize | `scoring` |
| sample explanations, if the package has any | `notes` |

Rules that survive the transfer:

- Keep the `\begin{itemize}\item Subtask $1$ ($50\%$ số điểm): …` shape for
  `scoring` — Polygon renders it as the **Scoring** section. With points enabled
  (Phase 5) this field must exist, or readiness warns about tests-with-points
  and no scoring. A single full-score subtask still gets one `\item`.
- Never downgrade math to Unicode: `$...$`, `\le`, `\times`, `\ldots` stay.
- **Display math is `$$...$$`, never `\[...\]`** anywhere in a Codeforces
  statement.
- `---` renders as an em dash.
- Samples are **not** pasted into `legend`/`notes` — they arrive as tests with
  statement I/O in Phase 5.
- If the statement and `source/problem-context.md` disagree, stop and fix it in
  `polygon-statement`; do not resolve it in the Polygon field.

---

### 4. Checker, validator, generator, solutions

Upload source files first, then bind roles. `save_problem_file` takes
`file_type="source"` and either `local_path` or `file_content`.

1. **Validator.** `save_problem_file(file_type="source",
   file_name="validator.cpp", local_path="outputs/validator.cpp")`, then
   `set_problem_validator(problem_id, "validator.cpp")`. Polygon compiles it at
   save time from that single file, so it must be self-contained: `testlib.h` is
   the only include Polygon resolves. Any other local `#include "…"` fails with
   `No such file or directory` — inline the header into a temporary copy and
   upload that, leaving `outputs/validator.cpp` unchanged.
2. **Generator.** `save_problem_file` for `outputs/gentest.cpp` as
   `file_name="gentest.cpp"` — the name must match the `gentest` token in
   `outputs/test-script.txt`. No role binding needed.
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
4. **Solutions.** Upload **every** solution with
   `save_problem_solution(problem_id, name=<basename>, local_path=…,
   tag=<mapped>)`. Tags come from the filename convention plus the *measured*
   verdicts recorded in `outputs/solution/manifest.md`:

| package file | Polygon type | API tag |
|---|---|---|
| `source/solution.cpp` — the validated official solution | Main correct solution | `MA` |
| `outputs/solution/ac-*.cpp` — the other correct solutions | Accepted | `OK` |
| `outputs/solution/tle-*.cpp` | Time limit exceeded | `TL` |
| `outputs/solution/wa-*.cpp` | Incorrect | `RJ` |

Notes on the mapping:

- Exactly one `MA` — readiness warns when the main count ≠ 1. Upload
  `source/solution.cpp` as `sol-main.cpp` so it cannot collide with a suite
  filename. When the package has no `source/solution.cpp`, promote one
  full-scope `ac-full-*.cpp` to `MA` and leave the rest `OK`.
- `ac-subtask-*.cpp` is correct only inside its subtask, so it fails the full
  test set: upload it as **Incorrect** (`RJ`), not `OK`.
- `RJ` (Incorrect) means "any verdict except accepted", which is exactly what a
  `wa-*` candidate is. Do not narrow it to `WA` on a guess.
- A `light-weight` package legitimately has a single AC and no WA/TLE zoo; that
  is not an error.

---

### 5. Tests

**Points, no groups.** `format` in `outputs/problem.json` decides: `"oi"` →
`enable_problem_points(problem_id, enable=true)`; `"icpc"` → leave points off, and
skip `scoring` too.

**Never enable test groups.** Do not call `enable_problem_groups`,
`save_problem_test_group` or `set_problem_test_group`, and never pass a
`test_group`. Every test is scored on its own: a solution earns the points of
each test it passes, whatever it does on the others. The statement's subtask
list stays in the `scoring` field as the description of the ladder; it is not
mirrored into Polygon groups.

**This divergence is deliberate, and contestants can see it.** `subtasks[].points`
in `outputs/problem.json` sets the statement's `% số điểm` prose and the sum check;
it is never uploaded as group points. A contestant who clears only `g1` therefore
earns one point per g1 test passed, not the 40 the ladder advertises. Do not "fix"
that during an upload by enabling groups — per-test scoring is the pack's choice,
and changing it is a change to this skill, not to one problem.

**Samples first (indices `1..S`).** The samples are the files in
`outputs/example-test/`; upload them as manual tests **before** the script:

1. Take `test_<i>.inp` and `test_<i>.out` in index order — `S` is how many pairs
   the directory holds (normally 1 or 2). Send the file contents verbatim; the
   `.out` is already the validated solution's own output from Step 5b, so do not
   re-run, reformat, or hand-check it.
2. Run `outputs/validator.cpp` on each sample input as a last check; a rejected
   sample is a package bug, so stop and fix it in Step 5b.
3. Upload each with **all** of these set together — the "use in statements"
   flag alone leaves the Examples block empty:

```text
save_problem_test(
  problem_id, testset="tests", test_index=i,
  test_input=<test_i.inp content>,
  test_output=<test_i.out content>,          # raw API: testOutput
  test_use_in_statements=true,
  test_input_for_statements=<same input>,
  test_output_for_statements=<same answer>,
  verify_input_output_for_statements=true,
  test_points=0)
```

If you call Polygon's raw `problem.saveTest` HTTP API instead, the statement I/O
keys are **`inputForStatement` / `outputForStatement`** (singular) —
the plural forms are silently ignored, a common upload bug. **Samples are never
graded: `test_points=0`.** If you bulk-assign points later, **re-upload the
samples last** so their statement I/O is not cleared.

**Script.** `outputs/test-script.txt` transfers almost verbatim, with two edits:

- Every line ends `> $` in the package. Manual samples already occupy `1..S`, so
  **rewrite each `$` to an explicit index** `S+1, S+2, …` in the original order.
  Indices must not collide with the samples.
- Strip any `#` comment line — Polygon's parser rejects them
  (`source: Line should end with > testIndex`).

Nothing else changes: the seeds and `--subtask`/`--rate` options are what
produced the local tests, and the generator is a pure function of argv
(`registerGen(argc, argv, 1)`), so Polygon reproduces byte-identical input. If a
line's argv is not the one that generated the local file, regenerate the suite
with `generating-tests` — do not guess argv that makes different tests. Then
`save_problem_script(problem_id, testset="tests", source=…)`.

**Points.** After every test exists, give each **non-sample** test the same
value, `polygon.total_points / <number of non-sample tests>` from
`preference.yml` — 100 points over 100 generated tests is 1 point each. State
the arithmetic (total, test count, per-test value) before applying it, and make
the values **sum exactly** to `total_points`: with an integer base, put the
remainder on the first tests. Samples stay at 0, so the graded total is carried
entirely by the generated tests.

Assign with `save_problem_test(problem_id, testset="tests", test_index=i,
test_points=…)` — or raw `problem.saveTest` with `testPoints` when MCP times out
over ~100 tests (see the Appendix on throttling). Do **not** pass `test_group`;
there are no groups.

**Validator groups.** With groups off, Polygon never calls `validator.group()`,
which matches the pack's default validator — it validates the full legal input
domain and does not branch on a group. A validator that *requires* a group name
would fail the build here; that is a bug to fix in `polygon-validator`.

**Verify before committing:** `get_problem_tests` on `1..S` — each must show
`useInStatements: true` and non-null `inputForStatement` / `outputForStatement`.

---

### 6. Readiness → commit (no email) → build

1. `check_problem_readiness(problem_id, testset="tests")`.
   - `blocking_issues` non-empty → fix each real issue, but **judge first**. One
     known false positive: `scriptLine missing from script: …` — the tool's
     script normalizer can fail to match saved lines that are present and
     regenerate fine; cross-check with `view_problem_script` or a passing build.
     Polygon's own package build is the authority over this tool's verdict.
   - Only `warnings` → judge each. Acceptable: no validator tests, no checker
     tests, no package yet, empty tutorial.
2. `commit_problem_changes(problem_id, minor_changes=true,
   message="<slug>: upload from cp-problem-generation")`. `minor_changes=true`
   is how Polygon commits **without sending email**.
3. `build_problem_package_and_wait(problem_id, full=true, verify=true,
   timeout_seconds=1800, poll_interval_seconds=5)`.
   - `decision: package_ready` → done.
   - `package_failed` / `build_timeout` / `workflow_error` → read
     `recovery_actions`, fix the named cause, retry. Never re-commit blindly
     after a build failed on the script or a solution.
   - Do **not** rebuild if Polygon says a full verified package for this
     revision already exists; use it, or commit a new revision first.
   - After assigning points, commit and rebuild so scoring sticks;
     confirm state `READY`.
   - `PackageException: Got exception while generating tests: Can't generate
     input or answer for test <N> [… Validator 'validator.exe' returns exit code
     3 [FAIL <msg>]]` means the validator rejected a regenerated test (testlib
     exit 3 = FAIL). Causes, in order: a script line whose argv does not
     reproduce the intended subtask/bounds, or a generator violating a
     validator constraint — both upstream bugs.

**Verify before declaring done:** samples show statement I/O; every non-sample
test carries its points and the points sum to `total_points`; package state is
`READY`. Re-upload samples **last** if a
later step cleared their statement fields.

---

### 7. Grant `codeforces` READ

Importing the problem into a Codeforces contest requires the special
`codeforces` user to have **READ** access.

**Known gap (verified):** Polygon's API exposes no collaborator/access method —
`problem.addUser` / `problem.saveUser` return HTTP 404, and Codeforces has
confirmed access management is not in the API. No tool, in cf-polygon-mcp or
anywhere else, can grant it. Do not fake it:

1. State plainly that this is a manual, owner-only browser step.
2. Give the user the working URL. That is the session link the web UI shows —
   `https://polygon.codeforces.com/generalInfo?ccid=<ccid>&session=<session>` —
   **not** a fabricated `https://polygon.codeforces.com/cf/<id>`: the numeric API
   id in `outputs/polygon.json` is not a web URL. Take the link from a tool
   result when the server returns one, otherwise ask the logged-in owner to copy
   it from the address bar. From there: **Manage access** → add user
   `codeforces` with access type **READ**.
3. Do not mark this done until the user confirms they did it.

Never invent a tool name for a server that does not have it.

---

## Appendix — direct Polygon API calls

When the MCP server lacks a capability (bulk `testPoints` updates, a compact
readback of every test's points), call Polygon's HTTP API directly — it is
the same API the server wraps. Base URL `https://polygon.codeforces.com/api/`;
credentials are the server's `POLYGON_API_KEY` / `POLYGON_API_SECRET`.

**Signing — pure stdlib** (do not import the installed `cf-polygon-mcp` package;
outside its venv its compiled deps fail to import). With `hashlib`, `random`,
`time`, `urllib`:

1. params = method args + `apiKey` + `time` (unix seconds, as str)
2. `rand` = 6 random digits
3. sort params by key, join as `k=v&k=v`
4. `base = f"{rand}/{method}?{joined}#{secret}"`; `sig = sha512(base)`
5. add `apiSig = rand + sig`; GET → query string, otherwise POST form body

Useful calls: `problem.info` (limits sanity check), `problem.tests` (every
test's `index`, `points`, `manual`, `useInStatements` — the cheap way to confirm
the points layout end to end), `problem.saveTest` with `testPoints` and no other
test field (a pure points update; `group` stays empty because groups are off).

**Throttling.** The API rate-limits bursts — plain `429 Too Many Requests`
(HTML body), typically around the ~25th rapid call; also retry 502/503/504/521
with backoff. Sleep ~1–2 s between calls in a loop (points for ~100 tests takes
minutes), then verify the whole range and retry the stragglers.

Prefer MCP for create, sample statement I/O and `build_problem_package_and_wait`;
raw API for bulk points when MCP times out.

## Degraded mode — no MCP server

Per phase, offer the manual equivalent and stop; never fake a successful upload.
Create and limits via the Polygon web UI, statement via the Statement tab, files
via Sources, tests via the script and manual-test forms, commit via "Commit
changes", package via "Build package", access via "Manage access". Proceed only
as the user confirms each phase.

## Done

- [ ] `outputs/polygon.json` records `id`, `owner`, `name`
- [ ] Limits match `source/problem-context.md`, else `outputs/problem.json`
      (TL, ML, I/O names); no unconfirmed provisional limit published
- [ ] Statement uploaded under `lang="english"` with Vietnamese content; display
      math is `$$...$$`; `scoring` present (OI) and matching the statement's
      subtasks
- [ ] `validator.cpp` and `gentest.cpp` uploaded and bound; the checker bound per
      `outputs/problem.json` — `checker.cpp` uploaded on `"custom"`, the `std::`
      token bound with no upload on `"stock"`
- [ ] Every solution uploaded with its mapped tag; exactly one `MA`
- [ ] Points enabled (OI); groups never enabled; every non-sample test carries
      an equal share of `preference.yml` → `polygon.total_points`, summing exactly
- [ ] Samples uploaded as tests `1..S` with statement I/O verified via
      `get_problem_tests`; 0 points, ungrouped
- [ ] `test-script.txt` uploaded with explicit indices from `S+1`, comments
      stripped, argv unchanged
- [ ] Readiness clean apart from the known `scriptLine` false positive
- [ ] Committed with `minor_changes=true` (no email); package `READY`
- [ ] `codeforces` READ granted manually by the owner — user confirms
