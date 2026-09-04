---
name: uploading-to-polygon
description: >
  Upload a finished outputs/ package to Codeforces Polygon through the external
  cf-polygon-mcp server — take the Polygon problem name from
  source/problem-context.md (ask when it states none), create the problem or,
  when that name already exists on Polygon, wipe its old content — script,
  every test, the statement fields — and re-sync this package onto it,
  push the Vietnamese statement with its Scoring section, upload validator.cpp,
  gentest.cpp, the checker named by problem.json (a std:: token or checker.cpp),
  and every solution tagged with its intended verdict,
  set time/memory limits, upload samples, load test-script.txt, wire subtasks
  and per-test points, commit without email, build the package, and grant
  `codeforces` READ access. Triggers on upload this problem to
  polygon, upload to codeforces, push to polygon, đẩy lên polygon, take it to
  polygon, sync to polygon. Runs after `creating-problems` Step 7 and its
  cross-artifact consistency gate; the opt-in Step 8 editorial is not required.
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

Source of truth, in this order: `source/problem-context.md` (constraints, subtask
scoring, **and the Polygon problem name**), `outputs/problem.json` (limits, I/O mode, `format`, the checker
decision, subtask points), `outputs/statement.txt`, `outputs/solution/manifest.md`. **Never invent a number
the package does not state** — no time limit, no memory limit, no subtask points —
and never invent the Polygon problem name.

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
Phase 1b also needs the wipe side: whatever the server calls `deleteTest`,
`clearScript` and `discardWorkingCopy`. If it exposes none of them, do that
phase through the raw API (Appendix) rather than skipping it — an un-wiped
re-upload is not what this skill promises.
Credentials are `POLYGON_API_KEY` / `POLYGON_API_SECRET` in the server's own env;
if the server reports them missing, stop and tell the user. This plugin ships no
MCP config of its own — the server is configured by the host (Claude Code
`.mcp.json` / settings, or Cursor `.cursor/mcp.json`). In Cursor the entry also
needs the dependency pin `"--with", "mcp<2"` when the ambient Python resolves
mcp 2.x, and must be toggled on in the MCP server list before its tools resolve.

If the server is not connected at all: offer the manual fallback at the end,
phase by phase, and stop. Never invent tool calls against a server you cannot see.

Every MCP write returns a structured envelope: `status` (`success`/`error`),
`action`, `message`, `result`, `error`; workflows add `stage`, `decision`,
`can_retry`, `recovery_actions`. **Read them.** A `status: error` after a
create/commit/build is a stop-and-look, not a shrug.

Polygon's own HTTP API (the Appendix) uses a *different* envelope: `status` is
`OK` or `FAILED`, a failure carries `comment` and normally no `result`, and a
success carries `result`. `FAILED` arrives with HTTP 400 for a bad parameter or
an access violation. Do not test a raw-API response for `status == "success"`.

---

## Pipeline

```
  0 preflight            [block until the problem name is known]
  1 create or resolve    [gate: id recorded, block]
  1b wipe the old content, when the problem already existed
  2 limits
  3 statement (type english, content in Vietnamese)
  4 checker + validator + generator + solutions
  5 tests (samples, script, per-test points)
  6 readiness -> commit (no email) -> build package
  7 grant codeforces READ
```

One gate blocks on the user, one on Polygon. Phase 0 needs the **Polygon
problem name**, and it comes from `source/problem-context.md`; when that file
states none, ask the user and do nothing else until they answer. Phase 1 then
needs the problem to **exist** on Polygon with its id recorded before any other
phase runs.

**A name that already exists on Polygon is a re-upload, not a question.** The
existing problem is wiped in Phase 1b and re-synced from this package: the
script is cleared, every test deleted, and each statement field, source file and
solution overwritten. This package is the truth; whatever the problem held
before is not. Create a *copy* under a new name only when the user explicitly
asks for one — and then ask them for that name rather than coining
`<name>-2` yourself.

---

### 0. Preflight (block)

1. The required files above exist; the package passed its consistency gate.
   `outputs/problem.json` loads, and its `checker` is one of the two shapes —
   `{ "kind": "stock", "name": "<token>" }` or `{ "kind": "custom", "file": "checker.cpp" }`.
2. The MCP server is connected and its tools resolve.
3. **Polygon problem name (blocking).** It comes from
   `source/problem-context.md` — the field that names the Polygon problem
   (`problem_id`, `Polygon name`, or the same idea under another label). Read it
   there and use it verbatim.

   - **It is stated** → that is the name. Do not rewrite it, do not "normalize"
     it, do not prefer a directory name over it.
   - **It is not stated** → **ask the user for it and stop until they answer.**
     No other preflight step, no `get_problems`, no file upload, nothing runs
     first. Never derive a slug from the working directory name, the statement
     title, or the editorial as a substitute for asking.

   Do not write the answer back into `source/problem-context.md`; that file is a
   source-of-truth the sibling skills own. Record it in `outputs/polygon.json`
   in Phase 1 instead, and report that `problem-context.md` is missing the field
   so the author can add it.

   The statement's Vietnamese title (`\textbf{Tên bài:}`) is a separate thing —
   it becomes the statement `name` in Phase 3, not the Polygon problem name.
4. **Limits.** Read the time and memory limit from `source/problem-context.md`;
   when it states none, read `limits.time_ms_published` and `limits.memory_mb`
   from `outputs/problem.json`. If neither has them, ask. A limit that Step 0
   proposed and Step 4 never measured is an assumption — confirm it with the
   user before publishing it.

   Polygon's own bounds are narrower than the package's: `timeLimit` is
   **250–15000 ms and must be divisible by 50**, `memoryLimit` is **4–1024 MB**.
   A package limit outside that, or a millisecond value not divisible by 50,
   cannot be uploaded — stop and have it fixed upstream rather than rounding it
   here.
5. **Already uploaded?** If `outputs/polygon.json` exists, this package already
   has a Polygon id; Phase 1 resolves it by name anyway and Phase 1b wipes it.
   Never silently create a *second* problem for a package that already has an
   id — a mismatch between the recorded id and the id the name resolves to is a
   stop-and-ask (Phase 1, step 4).
6. **Samples.** Read `outputs/example-test/` — workflow Step 5b produced the
   `test_<i>.inp`/`test_<i>.out` pairs, and those are the samples. Upload them
   as-is; do not re-derive answers or substitute inputs of your own. If the
   directory is missing or an `.out` is absent, the package is unfinished: stop
   and send it back to Step 5b rather than picking samples here.
7. State the bindings, the problem name **and where it came from**
   (`problem-context.md` or the user's answer), and the limits in the first
   status update.

---

### 1. Create or resolve the problem (gate, block)

The `problem_id` every later call takes is the **numeric Polygon id**, and only
Polygon issues it: `create_problem` returns it in `result.id`, `get_problems`
reports it for a problem that already exists. The name from Phase 0 is what you
look the problem up by — it is not the `problem_id`.

Prefer **MCP** `get_problems` / `create_problem`; the raw equivalents are
`problems.list` (note the **plural** `problems`) and `problem.create`. MCP can
work while raw HTTP 404s or times out — do not call the API "down" until MCP
also fails.

1. `get_problems(name=<name from Phase 0>)` — raw: `problems.list?name=…`.
   A `Problem` carries `id`, `owner`, `name`, `deleted`, `accessType`,
   `revision`, `workingCopyRevision`, `latestPackage`. Ignore `deleted: true`
   hits; `problems.list` hides them unless `showDeleted=true`.
2. **A non-deleted problem with that name exists → that is the problem.** Report
   its `id`, `owner` and `accessType`, state plainly that Phase 1b will wipe its
   script, tests and statement fields before re-syncing this package, and go on.
   Do not ask; do not invent a second name.
   - `accessType` below WRITE → you cannot upload at all. Stop and tell the
     user which account owns it.
3. No problem with that name → `create_problem(name=<name>)`.
   - `status: success` → `result.id` (and `result.owner`) is your id. A freshly
     created problem is empty, so **skip Phase 1b**.
   - `status: error` saying the name exists → the lookup missed it (a deleted or
     unlisted problem holds the name); re-run step 1 with `showDeleted=true`,
     report what you find, and stop.
   - Any other `status: error` → report and stop.
4. **Record the id** in `outputs/polygon.json` — the only file this skill writes.
   On a re-upload the file must already agree with the id you resolved; if it
   names a different id, **stop and ask** rather than rewriting it or wiping the
   problem you just resolved:

```json
{ "id": 123456, "owner": "nudetiger", "name": "sum-of-paths" }
```

The gate closes when that file exists and loads.

---

### 1b. Wipe the old content (re-upload only)

Only when Phase 1 resolved an **existing** problem. A fresh `create_problem`
skips this phase entirely.

Polygon deletes unevenly, and the skill must not pretend otherwise:

| Content | How it goes away |
|---|---|
| Uncommitted junk from a failed earlier attempt | `problem.discardWorkingCopy` |
| Test script | `problem.clearScript(testset="tests")` |
| Tests | `problem.deleteTest(testset="tests", testIndices="…")` |
| Statement fields | overwritten in Phase 3, blanked field by field |
| Source files, solutions | overwritten by name in Phase 4 — **no delete API** |

1. **Discard the working copy** — `problem.discardWorkingCopy` (MCP
   `discard_problem_changes` if it exposes one). This throws away *uncommitted*
   changes only, so a half-finished previous upload does not survive into this
   one. Committed content is untouched; that is what steps 2–4 handle. Skip it
   if the user wants uncommitted work-in-progress preserved — but say so.
2. **Clear the script first, then delete the tests.** Order matters:
   script-generated tests cannot be deleted individually — `problem.deleteTest`
   rejects them with reason `FREEMARKER_SCRIPT_TEST`. So
   `problem.clearScript(testset="tests")`, then read the survivors with
   `problem.tests(testset="tests", noInputs=true)` (`noInputs=true` keeps the
   response small) and delete them:

   ```text
   problem.deleteTest(testset="tests", testIndices="1,2,3,…")
   ```

   `deleteTest` is **all-or-nothing on the pre-check**: if one index fails it
   returns `FAILED` with comment `Some tests can not be deleted.` and a
   `DeleteTestsResult` whose `failures[]` give `index`, `reason`
   (`DUPLICATE` / `NOT_FOUND` / `FREEMARKER_SCRIPT_TEST` / `DELETE_FAILED`) and
   `message` — and deletes nothing. Read the reasons instead of retrying blind:
   a `FREEMARKER_SCRIPT_TEST` means the script did not actually clear, and
   `NOT_FOUND` means your index list is stale. `DELETE_FAILED` is the one
   partial case — comment `Some tests failed during deletion; problem state may
   be partially modified.` — so re-read `problem.tests` before retrying.
   Confirm with `problem.tests` that the testset is empty before Phase 5.
3. **Files and solutions cannot be deleted through the API.** There is no
   `deleteFile` / `deleteSolution` / `deleteStatement` method — only
   `problem.saveFile`, `problem.saveSolution` and `problem.saveStatement`, which
   *overwrite by name* when `checkExisting` is omitted or false. So:

   - Every file and solution this package ships is refreshed in Phase 4 simply
     by saving it under the same name. Never pass `checkExisting=true` on a
     re-upload — it turns the save into an add-only call and fails on an
     existing name.
   - Anything the *old* problem holds that this package no longer ships — a
     renamed solution, a `checker.cpp` left behind after the package moved to a
     `std::` token, a stale generator — is **leftover**. List it by diffing
     `problem.files` (`resourceFiles`, `sourceFiles`, `auxFiles`) and
     `problem.solutions` against the package, and report each leftover with the
     one place it can be removed: the Polygon web UI (**Files** / **Solutions**
     tab → the row's delete action). Do not claim a clean wipe when leftovers
     remain, and do not neutralise a leftover by overwriting it with an empty
     file — an empty source breaks the build instead.
   - A leftover solution is not harmless: `verify=true` in Phase 6 runs it, and
     a stale wrong-answer file tagged `OK` fails the build. Whenever a leftover
     solution exists, either the user deletes it in the UI or you re-tag it `NR`
     ("do not run") via `save_problem_solution(name=<leftover>, tag="NR")` —
     state which you did.
4. **Statements** cannot be deleted either, and a stale language slot survives
   forever. Read `problem.statements` (a map of language → `Statement`) and
   report any language other than `english`; only the owner can remove it in the
   web UI. The `english` slot itself is fully rewritten in Phase 3.
5. Report the wipe as three counts before moving on: tests deleted, files and
   solutions to be overwritten, leftovers that need the web UI.

---

### 2. Limits

`update_problem_info(problem_id, …)`:

| field | from |
|---|---|
| `input_file` / `output_file` | `io` in `outputs/problem.json` — `"stdin"`/`"stdout"`, or the `.inp`/`.out` pair it names, matching the guarded `freopen` block the solutions use |
| `time_limit` (ms) | `source/problem-context.md`, else `limits.time_ms_published` in `outputs/problem.json` |
| `memory_limit` (MB) | `source/problem-context.md`, else `limits.memory_mb` in `outputs/problem.json` |
| `interactive` | `false` — this pack does not model interactive problems |

Raw equivalent: `problem.updateInfo`, whose parameters are `inputFile`,
`outputFile`, `interactive`, `wellFormed`, `skipDuplicatedTestsValidation`,
`timeLimit`, `memoryLimit` — all optional, all left untouched when omitted.
Two failure modes worth knowing before the call:

- `timeLimit` must be 250–15000 and divisible by 50; `memoryLimit` 4–1024.
  Phase 0 already refused an out-of-range package limit.
- If the effective input and output file names are equal ignoring case, the
  method returns `FAILED`. `stdin`/`stdout` and a distinct `.inp`/`.out` pair
  are both fine; `data.txt`/`DATA.TXT` is not.

Do not touch `wellFormed` or `skipDuplicatedTestsValidation` — the package's
tests are generated from a validated script, and silencing the duplicate-test
check would hide a real generator bug.

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

`problem.saveStatement` takes `lang` (the only required parameter) plus
`encoding`, `name`, `legend`, `input`, `output`, `scoring`, `interaction`,
`notes`, `tutorial`, `showInReview`, `showCautionsAndGrammaticalFixes` — and
**an omitted field is left as it was**, not cleared.

That matters on a re-upload: Phase 1b could not delete the statement, so a
field this package does not fill would still hold the old problem's prose.
**Send every field the package does not use as an empty string** in the same
call — at minimum `notes` and `tutorial`, plus `scoring` when `format` is
`"icpc"` and `interaction` always (this pack has no interactive problems). The
editorial is never uploaded, so `tutorial` is always empty here.

`name` must be a **single line**: Polygon rejects a value containing CR, LF,
U+2028 or U+2029 rather than storing a multiline problem name. Take the
`\textbf{Tên bài:}` value on one line and strip any trailing newline.

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

The raw calls behind this phase, for when MCP is missing one:
`problem.saveFile(type="source", name=…, file=<content>)`,
`problem.saveSolution(name=…, file=<content>, tag=…)`,
`problem.setValidator(validator=…)`, `problem.setChecker(checker=…)`. Every one
of them **overwrites by name**, which is exactly what a re-upload wants — so
never pass `checkExisting=true` here: that flag makes the call add-only and it
fails outright on a name that already exists. The `resource`/`aux` file types
and the `forTypes`/`stages`/`assets` resource properties are not used by this
pack; upload everything as `type="source"`.

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
  `wa-*` candidate is. Do not narrow it to `WA` on a guess. Polygon's full tag
  set is `MA`, `OK`, `RJ`, `TL`, `TO` (time limit **or** accepted), `TM` (time
  **or** memory limit), `WA`, `PE`, `ML`, `NR` (do not run), `RE`; this pack
  uses only `MA`, `OK`, `RJ`, `TL`, and `NR` for a Phase 1b leftover.
- Per-testset or per-group overrides exist (`problem.editSolutionExtraTags`),
  but this pack has one testset and no groups — never call it.
- A `light-weight` package legitimately has a single AC and no WA/TLE zoo; that
  is not an error.

---

### 5. Tests

**Points, no groups.** `format` in `outputs/problem.json` decides: `"oi"` →
`enable_problem_points(problem_id, enable=true)`; `"icpc"` → leave points off, and
skip `scoring` too.

Raw: `problem.enablePoints(enable=true)` — problem-wide, it takes **no**
`testset`, unlike `problem.enableGroups(testset, enable)`. On a re-upload the
flag may already be set; setting it again is harmless.

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
`outputs/example-test/`; upload them as manual tests **before** the script. On a
re-upload Phase 1b already emptied the testset, so `1..S` are free — if
`problem.tests` still reports anything here, Phase 1b did not finish and this
phase does not start.

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
  test_output=<test_i.out content>,          # MCP convenience; see below
  test_use_in_statements=true,
  test_input_for_statements=<same input>,
  test_output_for_statements=<same answer>,
  verify_input_output_for_statements=true,
  test_points=0)
```

If you call Polygon's raw `problem.saveTest` HTTP API instead, its parameters
are `testset`, `testIndex`, `testInput`, `testGroup`, `testPoints`,
`testDescription`, `testUseInStatements`, **`testInputForStatements`**,
**`testOutputForStatements`** and `verifyInputOutputForStatements` — every
statement key carries the `test` prefix and the plural `Statements`. The
singular `inputForStatement` / `outputForStatement` are fields of the `Test`
object that `problem.tests` *returns*; passing those names to `saveTest` sets
nothing and is silently ignored, which is the classic empty-Examples bug. Read
back with the singular names, write with the plural ones.

Note what is *not* in that list: there is **no `testOutput`**. Polygon stores no
expected answer for a test — the answer is whatever the main correct solution
prints, which is why exactly one `MA` matters. The only answer text you upload
is `testOutputForStatements`, the sample answer *displayed* in the statement,
and `verifyInputOutputForStatements=true` is what makes Polygon check it against
the main solution at build time. So a sample whose `.out` is stale fails the
build rather than silently publishing a wrong example.

**Samples are never graded: `test_points=0`.** If you bulk-assign points later,
**re-upload the samples last** so their statement I/O is not cleared.

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
`save_problem_script(problem_id, testset="tests", source=…)` — raw
`problem.saveScript(testset="tests", source=…)`, which replaces the whole script
in one call (`problem.clearScript(testset)` is the Phase 1b eraser).

**Points.** After every test exists, give each **non-sample** test the same
value, `polygon.total_points / <number of non-sample tests>` from
`preference.yml` — 100 points over 100 generated tests is 1 point each. State
the arithmetic (total, test count, per-test value) before applying it, and make
the values **sum exactly** to `total_points`: with an integer base, put the
remainder on the first tests. `testPoints` is a non-negative double below `10^9`
with **at most two decimal places**, so a value like `100/3` must be rounded to
2 dp and the rounding error absorbed by the first tests — never uploaded as
`33.333`, which Polygon rejects. Samples stay at 0, so the graded total is carried
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
   - The raw equivalent is `problem.cautions`, which inspects the current
     working copy and always returns the four arrays `common`, `statement`,
     `structure`, `issues` (each possibly empty) plus `packageReadinessIssues`,
     `latestPackageWarnings` and cached `ai` tips. It starts no new AI request,
     and `NO_CHECKER_TESTS` / `NO_VALIDATOR_TESTS` are suppressed when the
     corresponding `NO_CHECKER` / `NO_VALIDATOR` caution is present — a missing
     tests caution disappearing is not the same as it being fixed.
2. `commit_problem_changes(problem_id, minor_changes=true,
   message="<slug>: upload from cp-problem-generation")` — raw
   `problem.commitChanges(minorChanges=true, message=…)`, which needs WRITE
   access. `minor_changes=true` is how Polygon commits **without sending
   email**.
   - It returns a `CommitResult`. `status: OK` with `committed: false`,
     `conflictOccurred: false`, `message: "No changes"` means there was nothing
     to commit — on a re-upload that means an earlier phase silently wrote
     nothing, so go back and look rather than reporting success.
   - `conflictOccurred: true` means someone else moved the working copy under
     you; stop and tell the user instead of re-committing.
3. `build_problem_package_and_wait(problem_id, full=true, verify=true,
   timeout_seconds=1800, poll_interval_seconds=5)`. Raw
   `problem.buildPackage(full=true, verify=true)` only **starts** the build and
   returns immediately — poll `problem.packages` for the new `Package` and its
   state yourself; there is no waiting variant in the HTTP API. `full=true`
   builds the standard, linux and windows packages (linux and windows carry the
   generated tests; standard does not), and `verify=true` runs every solution on
   every test so the tags from Phase 4 are actually checked.
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

**An OK verification is not a package — always create one after it.** A clean
readiness check, a `CommitResult` with `committed: true`, solutions verifying
against their tags, samples reading back right: none of those produce a package.
The build is a separate, mandatory action, and the upload is unfinished until
`problem.packages` shows a **full, verified** package in state `READY` for the
committed revision. So whenever a verification comes back OK — the first time,
and again after every later fix, points assignment or re-commit — the next call
is `build_problem_package_and_wait`, not a completion report. Never tell the
user the problem is on Polygon while its newest revision has no `READY`
package; say what is still building or what failed instead.

**Verify before declaring done:** samples show statement I/O; every non-sample
test carries its points and the points sum to `total_points`; package state is
`READY`. Re-upload samples **last** if a
later step cleared their statement fields.

---

### 7. Grant `codeforces` READ

Importing the problem into a Codeforces contest requires the special
`codeforces` user to have **READ** access.

**This is now an API call.** The older `problem.addUser` / `problem.saveUser`
never existed and 404'd; access management arrived as `problem.accesses` and
`problem.setAccess` (2026-08-10). Use them:

1. `problem.setAccess(problemId=<id>, login="codeforces", accessType="READ")`.
   A success is `{"status":"OK"}` with **no `result` field** — do not read the
   absence of `result` as a failure.
2. Verify with `problem.accesses`, which returns the direct entries sorted by
   login (users and `@groups` alike, ungrouped and not expanded). The
   `codeforces` row must read `READ`.

Facts that decide whether the call can work at all:

- It takes effect **immediately** — no `commitChanges` — and is stored at
  problem level, outside the working copy. Phase 6 need not re-run.
- It needs **direct** WRITE or OWNER access. Access inherited through a user
  group is *not* enough, and `problem.accesses` (which only needs *effective*
  access) succeeding does not prove `setAccess` will.
- Repeating a state already stored is a successful no-op: no email, no
  modification-time bump, no rate-limit consumption. Re-running the upload does
  not re-notify anyone.
- `accessType="NONE"` removes a direct entry; `OWNER` cannot be assigned and an
  owner cannot be downgraded. The caller can remove **their own** non-owner
  access — so never call `setAccess` on the uploading account's own login.
- Rate limit: 60 *real* changes per minute per calling user.
- `FAILED` with HTTP 400 for: an unknown login, an `@group` login, a bad
  `accessType`, insufficient direct access, an ownership change, a sample or
  example problem, an exceeded rate limit, or a wrong `pin`. Report the
  `comment` verbatim rather than guessing which one it was.

**Manual fallback**, when the call returns `FAILED` for insufficient direct
access or the MCP server is absent: hand the owner the working URL — the session
link the web UI shows, `https://polygon.codeforces.com/generalInfo?ccid=<ccid>&session=<session>`
— **not** a fabricated `https://polygon.codeforces.com/cf/<id>`: the numeric API
id in `outputs/polygon.json` is not a web URL. Take the link from a tool result
when the server returns one, otherwise ask the logged-in owner to copy it from
the address bar. From there: **Manage access** → add user `codeforces` with
access type **READ**. Do not mark this done until they confirm.

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

1. params = method args + `apiKey` + `time` (unix seconds, as str). `time` must
   be within **5 minutes** of Polygon's clock or the request is denied — a
   skewed local clock looks exactly like a bad signature.
2. `rand` = any 6 characters; random digits are fine, and a fresh `rand` per
   request is recommended.
3. sort params **by name, then by value** (a repeated name like `testIndex`
   sorts among its own values), exclude `apiSig` itself, join as `k=v&k=v`
4. `base = f"{rand}/{method}?{joined}#{secret}"`; `sig = sha512(base)` in lower
   hex
5. add `apiSig = rand + sig`; GET → query string, otherwise POST form body.
   Send a large body (a source file, a test input, a script) as POST form data.

Useful calls: `problem.info` (limits sanity check), `problem.tests(testset,
noInputs=true)` (every test's `index`, `points`, `manual`, `useInStatements`,
`scriptLine`, `group`, `inputForStatement`, `outputForStatement` — the cheap way
to confirm the points layout end to end; `noInputs=true` keeps the response
small), `problem.saveTest` with `testPoints` and no other test field (a pure
points update; `group` stays empty because groups are off),
`problem.files` / `problem.solutions` (the Phase 1b leftover diff),
`problem.statements` (stale language slots), `problem.deleteTest` and
`problem.clearScript` (the Phase 1b wipe), `problem.discardWorkingCopy`,
`problem.setAccess` / `problem.accesses` (Phase 7).

Read-only reads that never touch the working copy: `problem.viewFile(type,
name)`, `problem.viewSolution(name)`, `problem.script(testset)`,
`problem.testInput` / `problem.testAnswer` — each returns the **raw file**, not
JSON, so do not parse them as an envelope.

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

The Phase 1b wipe in the UI, in this order: **Files** and **Solutions** tabs →
delete every row this package does not ship (the UI *can* delete these; the API
cannot); **Tests** tab → clear the script, then select and delete the remaining
tests; **Statement** tab → empty the fields this package does not fill. Then
re-upload from Phase 2 as usual.

## Done

- [ ] The Polygon problem name came from `source/problem-context.md`, or from
      the user when that file states none — never from a directory name
- [ ] When that name already existed on Polygon, Phase 1b ran: working copy
      discarded, script cleared, testset verified empty via `problem.tests`,
      and every file/solution/statement leftover the API cannot delete reported
      to the user with the web-UI step (or a leftover solution re-tagged `NR`)
- [ ] `outputs/polygon.json` records `id`, `owner`, `name`
- [ ] Limits match `source/problem-context.md`, else `outputs/problem.json`
      (TL, ML, I/O names); no unconfirmed provisional limit published
- [ ] Statement uploaded under `lang="english"` with Vietnamese content; `name`
      single-line; display math is `$$...$$`; `scoring` present (OI) and
      matching the statement's subtasks; every unused field sent as `""` so no
      old prose survives
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
- [ ] Committed with `minor_changes=true` (no email) and the `CommitResult`
      actually reports `committed: true`
- [ ] A package was **created after** the verification came back OK — and again
      after every later fix or re-commit — with `problem.packages` showing a
      full, verified package in state `READY` for the committed revision
- [ ] `codeforces` READ granted via `problem.setAccess` and confirmed by
      `problem.accesses` — or, when the call failed for insufficient direct
      access, granted in the web UI and confirmed by the user
