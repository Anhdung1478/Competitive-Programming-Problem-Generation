---
name: uploading-to-polygon
description: >
  Upload a finished outputs/ package to Codeforces Polygon through this
  plugin's bundled `polygon` MCP server (tools `polygon_*`) — take the Polygon
  problem name from source/problem-context.md (ask when it states none),
  create the problem or, when that name already exists on Polygon, wipe its old
  content — script, every test, the statement fields — and re-sync this package
  onto it, push the Vietnamese statement with its Scoring section, upload
  validator.cpp, gentest.cpp, the checker named by problem.json (a std:: token
  or checker.cpp), and every solution tagged with its intended verdict, set
  time/memory limits, upload samples, load test-script.txt, enable per-test
  points, commit without email, build a full verified package, and grant
  `codeforces` READ access. Triggers on upload this problem to polygon, upload
  to codeforces, push to polygon, đẩy lên polygon, take it to polygon, sync to
  polygon. Runs after `creating-problems` Step 7 and its cross-artifact
  consistency gate; the opt-in Step 8 editorial is not required.
---

# Uploading to Polygon

Ship a finished `outputs/` package to Codeforces Polygon via this plugin's
bundled **`polygon`** MCP server. You mirror what the package already says: no
regenerating tests, no rewriting the statement, no fixing solutions. Do not
spawn subagents.

A package that is not finished goes back to the sibling skill that owns it —
statement to `polygon-statement`, checker to `polygon-checker`, validator to
`polygon-validator`, tests to `generator-config` / `generating-tests`, the whole
build to `creating-problems`. Upload is not a way to finish a package.

Use `yagni-principles` here too: upload what the package contains, nothing more.

## Bootstrap

```text
PROBLEM = working repository root (the one with source/ and outputs/)
```

Source of truth, in this order: `source/problem-context.md` (constraints,
subtask scoring, **and the Polygon problem name**), `outputs/problem.json`
(limits, I/O mode, `format`, the checker decision, subtask points),
`outputs/statement.txt`, `outputs/solution/manifest.md`. **Never invent a number
the package does not state** — no time limit, no memory limit, no subtask
points — and never invent the Polygon problem name.

**Precondition — the package is finished.** These must exist and be current:

| File | Required |
|---|---|
| `outputs/statement.txt` | yes |
| `outputs/validator.cpp` | yes |
| `outputs/gentest.cpp`, `outputs/test-script.txt` | yes |
| `outputs/solution/` + `manifest.md` | yes (at least one full-scope AC) |
| `outputs/problem.json` | yes — limits, `io`, `format`, the checker decision |
| `outputs/checker.cpp` | only when `problem.json` says `"kind": "custom"` |
| `outputs/example-test/test_<i>.inp` + `.out` | yes (1–2 pairs; the statement samples) |
| `outputs/editorial.html` | not uploaded; ignore here |

Any required file missing, or the `creating-problems` cross-artifact consistency
gate not passed → **stop** and run `creating-problems` first.

**Precondition — the server is connected.** The `polygon_*` tools must resolve.
They are not in your catalogue until you load them: call `ToolSearch` with a
`select:` list of the names the phase needs, and **read the loaded schema**,
which always wins over anything written down.
[`references/polygon-tools.md`](references/polygon-tools.md) lists all
thirty by phase — a map, not an authority.

Credentials are `POLYGON_API_KEY` / `POLYGON_API_SECRET` in the server's own
environment; if a tool reports them missing, stop and point the user at
[`../../mcp-server/README.md`](../../mcp-server/README.md). Never read, print or
ask for them. If the server is not connected at all, offer the manual fallback
at the end, phase by phase, and stop.

Every tool returns a dict carrying `ok`. A failure is
`{"ok": false, "error": "<Polygon's own comment>", "method": "…"}`, sometimes
with `details`. **Read them.** Correct the call; never retry the identical
request, and never invent a tool the server does not have.

## The API reference — look it up, never guess

[`references/polygon-api.md`](references/polygon-api.md) is the complete Polygon
HTTP API as this skill needs it. **Read it instead of searching.**

| You need | Go to |
|---|---|
| Which method performs this phase's action | §1 *Action → method index* |
| Exact parameter names, and which are required | §4 *Methods* |
| How to sign a raw request | §2 |
| What a response looks like, and what `FAILED` means | §3, §5 |
| The legal value of a `tag` / `accessType` / `state` / … | §6 |
| Whether a limit or a points value is in range | §7 |

Each phase names its methods and links into §4. The reference is the authority
on what **Polygon** accepts; the loaded tool schema is the authority on how to
spell a **tool** argument. The two differ — see Phase 5's plural/singular trap.
If a method is not in §1 or §4, check there before concluding the docs are
incomplete: §1 also lists the methods that must never be called and the ones
that do not exist.

---

## Pipeline

```
  0 preflight            [block until the problem name is known]
  1 create or resolve    [gate: id recorded, block]
  1b wipe the old content, when the problem already existed
  2 limits
  3 statement (lang english, content in Vietnamese)
  4 checker + validator + generator + solutions
  5 tests (samples, script, per-test points)
  6 cautions -> commit (no email) -> build package
  7 grant codeforces READ
```

One gate blocks on the user, one on Polygon. Phase 0 needs the **Polygon
problem name** from `source/problem-context.md`; when that file states none, ask
and do nothing else until answered. Phase 1 needs the problem to **exist** with
its id recorded before any other phase runs.

**A name that already exists on Polygon is a re-upload, not a question.** Phase
1b wipes it and re-syncs from this package: script cleared, every test deleted,
each statement field, source file and solution overwritten. This package is the
truth. Create a *copy* under a new name only when the user explicitly asks —
and then ask them for that name rather than coining `<name>-2` yourself.

---

### 0. Preflight (block)

1. The required files exist; the package passed its consistency gate.
   `outputs/problem.json` loads, and its `checker` is one of the two shapes —
   `{ "kind": "stock", "name": "<token>" }` or
   `{ "kind": "custom", "file": "checker.cpp" }`.
2. `polygon_whoami()` — the cheapest proof that the key, the secret and the
   clock are all good. Run it before anything else touches Polygon.
3. **Polygon problem name (blocking).** It comes from
   `source/problem-context.md` — the field naming the Polygon problem
   (`problem_id`, `Polygon name`, or the same idea under another label). Use it
   verbatim.

   - **Stated** → that is the name. Do not rewrite or "normalize" it, and do
     not prefer a directory name over it.
   - **Not stated** → **ask the user and stop until they answer.** Nothing else
     runs first. Never derive a slug from the working directory, the statement
     title, or the editorial as a substitute for asking.

   Do not write the answer back into `source/problem-context.md` — the sibling
   skills own that file. Record it in `outputs/polygon.json` in Phase 1, and
   report that `problem-context.md` is missing the field so the author can add
   it. The statement's Vietnamese title (`\textbf{Tên bài:}`) is a separate
   thing: it becomes the statement `name` in Phase 3.
4. **Limits.** Time and memory from `source/problem-context.md`; failing that,
   `limits.time_ms_published` and `limits.memory_mb` from `outputs/problem.json`.
   If neither has them, ask. A limit Step 0 proposed and Step 4 never measured
   is an assumption — confirm it before publishing it.

   Polygon's bounds are narrower than the package's: `timeLimit` **250–15000 ms,
   divisible by 50**; `memoryLimit` **4–1024 MB**. Anything outside that cannot
   be uploaded — have it fixed upstream rather than rounding it here.
5. **Already uploaded?** If `outputs/polygon.json` exists, this package has an
   id; Phase 1 resolves it by name anyway and Phase 1b wipes it. Never silently
   create a *second* problem for a package that already has an id — a mismatch
   between the recorded id and the resolved one is a stop-and-ask.
6. **Samples.** `outputs/example-test/` holds the `test_<i>.inp`/`.out` pairs
   Step 5b produced; those are the samples. Upload them as-is — do not re-derive
   answers or substitute inputs. A missing directory or absent `.out` means the
   package is unfinished: stop and send it back to Step 5b.
7. **The generator must not force line endings (cheap gate, run it).**

   ```text
   grep -nE '_setmode|_O_BINARY|freopen[^;]*stdout|[\]r' outputs/gentest.cpp
   ```

   (`[\]r` matches a literal `\r` in the source — a bare `\r` would match every
   `return`.)

   Any hit is a **stop**. Polygon generates and validates tests under a Windows
   toolchain whose strict testlib `eoln()` requires CRLF, so a generator forcing
   bare LF fails every package build in Phase 6 — after the upload, with an
   error naming the wrong test. Send it back to
   `cp-problem-generation:generating-tests`. That every test validates on the
   authoring machine is not evidence against this; it is the symptom.
8. State the bindings, the problem name **and where it came from**
   (`problem-context.md` or the user's answer), and the limits, in the first
   status update.

---

### 1. Create or resolve the problem (gate, block)

> **API:** [`problems.list`](references/polygon-api.md#problemslist),
> [`problem.create`](references/polygon-api.md#problemcreate)

The `problem_id` every later tool takes is the **numeric Polygon id**, and only
Polygon issues it. The Phase 0 name is what you look the problem up *by* — it
is not the id.

1. `polygon_problems_list(name=<name from Phase 0>)`. Each `Problem` carries
   `id`, `owner`, `name`, `deleted`, `accessType`, `revision`,
   `workingCopyRevision`, `latestPackage`. Ignore `deleted: true` hits.
2. **A non-deleted problem with that name exists → that is the problem.**
   Report its `id`, `owner` and `accessType`, state plainly that Phase 1b will
   wipe its script, tests and statement fields before re-syncing, and go on. Do
   not ask; do not invent a second name.
   - `accessType` below WRITE → you cannot upload. Stop and tell the user which
     account owns it.
3. No problem with that name → `polygon_problem_create(name=<name>)`.
   - `ok: true` → `problem.id` (and `problem.owner`) is your id — the tool
     returns `{"ok": true, "problem": {…}}`, with no `result` wrapper. A fresh
     problem is empty, so **skip Phase 1b**.
   - An error saying the name exists → the lookup missed it (a deleted or
     unlisted problem holds it); re-run step 1 with `show_deleted=true`, report
     what you find, and stop.
   - Any other error → report and stop.
4. **Record the id** in `outputs/polygon.json` — the only file this skill
   writes. On a re-upload it must already agree with the id you resolved; if it
   names a different one, **stop and ask** rather than rewriting it or wiping
   the problem you just resolved:

```json
{ "id": 123456, "owner": "nudetiger", "name": "sum-of-paths" }
```

`problem.create` returns **no address** — Polygon's web URLs are
session-scoped and cannot be built from the id. Do not fabricate one.

The gate closes when that file exists and loads.

---

### 1b. Wipe the old content (re-upload only)

> **API:** [`problem.discardWorkingCopy`](references/polygon-api.md#problemdiscardworkingcopy),
> [`problem.clearScript`](references/polygon-api.md#problemclearscript),
> [`problem.tests`](references/polygon-api.md#problemtests),
> [`problem.deleteTest`](references/polygon-api.md#problemdeletetest),
> [`problem.files`](references/polygon-api.md#problemfiles),
> [`problem.solutions`](references/polygon-api.md#problemsolutions),
> [`problem.statements`](references/polygon-api.md#problemstatements)

Only when Phase 1 resolved an **existing** problem. Polygon deletes unevenly,
and the skill must not pretend otherwise:

| Content | How it goes away |
|---|---|
| Uncommitted junk from a failed earlier attempt | `polygon_discard_working_copy` |
| Test script | `polygon_clear_script` |
| Tests | `polygon_delete_tests` |
| Statement fields | overwritten in Phase 3, blanked field by field |
| Source files, solutions | overwritten by name in Phase 4 — **no delete API** |

1. **`polygon_discard_working_copy(problem_id)`** throws away *uncommitted*
   changes only, so a half-finished previous upload does not survive into this
   one. Committed content is untouched. Skip it only if the user wants
   work-in-progress preserved — and say so.
2. **Clear the script first, then delete the tests.** Order matters:
   script-generated tests cannot be deleted individually — `deleteTest` rejects
   them with reason `FREEMARKER_SCRIPT_TEST`. So
   `polygon_clear_script(problem_id, "tests")`, then read the survivors with
   `polygon_tests(problem_id, "tests", no_inputs=true)` and
   `polygon_delete_tests(problem_id, "tests", test_indices=[…])` in one call.

   `deleteTest` is **all-or-nothing on the pre-check**: one bad index and it
   deletes nothing, returning `details.failures` with each `index`, `reason`
   (`DUPLICATE` / `NOT_FOUND` / `FREEMARKER_SCRIPT_TEST` / `DELETE_FAILED`) and
   `message`. Read the reasons instead of retrying blind — `FREEMARKER_SCRIPT_TEST`
   means the script did not actually clear, `NOT_FOUND` means your index list is
   stale. `DELETE_FAILED` is the one partial case, so re-read `polygon_tests`
   before retrying it. Confirm the testset is empty before Phase 5.
3. **Files and solutions cannot be deleted through the API.** There is no
   `deleteFile` / `deleteSolution` / `deleteStatement` — only the `save*`
   methods, which **overwrite by name**. So:

   - Everything this package ships is refreshed in Phase 4 by saving it under
     the same name.
   - Anything the *old* problem holds that this package no longer ships — a
     renamed solution, a `checker.cpp` left behind after a move to a `std::`
     token, a stale generator — is **leftover**. Diff `polygon_files`
     (`resourceFiles`, `sourceFiles`, `auxFiles`) and `polygon_solutions`
     against the package, and report each leftover with the one place it can be
     removed: the Polygon web UI (**Files** / **Solutions** tab → the row's
     delete action). Do not claim a clean wipe when leftovers remain, and do not
     neutralise one by overwriting it with an empty file — an empty source
     breaks the build instead.
   - A leftover *solution* is not harmless: `verify=true` in Phase 6 runs it,
     and a stale wrong-answer file tagged `OK` fails the build. Either the user
     deletes it in the UI, or you re-tag it with
     `polygon_save_solution(problem_id, name=<leftover>, tag="NR")` and no source — state
     which you did.
4. **Statements** cannot be deleted either, and a stale language slot survives
   forever. Read `polygon_statements` and report any language other than
   `english`; only the owner can remove it in the web UI. The `english` slot is
   fully rewritten in Phase 3.
5. Report the wipe as three counts: tests deleted, files and solutions to be
   overwritten, leftovers needing the web UI.

---

### 2. Limits

> **API:** [`problem.updateInfo`](references/polygon-api.md#problemupdateinfo)
> — every parameter optional, omitted ones untouched; bounds in §7

`polygon_problem_update_info(problem_id, …)`:

| argument | from |
|---|---|
| `input_file` / `output_file` | the `.inp`/`.out` pair `io` names in `outputs/problem.json` |
| `time_limit_ms` | `source/problem-context.md`, else `limits.time_ms_published` |
| `memory_limit_mb` | `source/problem-context.md`, else `limits.memory_mb` |
| `interactive` | leave unset — this pack does not model interactive problems |

**Pass the `.inp`/`.out` pair verbatim**, matching the guarded `freopen` block
the solutions use. This pack's problems are file I/O; a package whose `io` says
`stdin` is the exception, and there the two arguments are **left off** — Polygon
stores a literal filename, so `input_file="stdin"` would create a problem
reading a file *called* `stdin`. Never pass the word.

Two failure modes worth knowing before the call:

- `time_limit_ms` must be 250–15000 and divisible by 50; `memory_limit_mb`
  4–1024. Phase 0 already refused an out-of-range package limit.
- If the effective input and output names are equal ignoring case, the method
  returns FAILED. `""`/`""` and a distinct `.inp`/`.out` pair are both fine;
  `data.txt`/`DATA.TXT` is not.

Do not touch `well_formed` or `skipDuplicatedTestsValidation`. The package's
tests come from a validated script, and silencing the duplicate-test check
would hide a real generator bug. `wellFormed` in particular is the natural
first guess when a build fails on `Expected EOLN`, and it is **not** a remedy:
it does not normalise line endings for the validator. Both values were tested on
the same package across four builds with no effect. Fix the generator (Phase 0
step 7), not this flag.

---

### 3. Statement — lang `english`, content in Vietnamese

> **API:** [`problem.saveStatement`](references/polygon-api.md#problemsavestatement)
> — `lang` is the only required parameter

`polygon_save_statement(problem_id, lang="english", …)`. The **slot is always
`english`**, because Codeforces contest import reads that slot. The **content is
not translated** — paste the Vietnamese prose from `outputs/statement.txt`
exactly as authored. Only `lang` stays `english`.

`outputs/statement.txt` is already Polygon simpleTex, so it transfers verbatim.
Split it on its section headers:

| `outputs/statement.txt` | argument |
|---|---|
| `\textbf{Tên bài:}` value | `name` |
| `\textbf{Đề bài}` body, including `\textbf{Yêu cầu:}` | `legend` |
| `\textbf{Input}` body | `input` |
| `\textbf{Output}` body | `output` |
| `\textbf{Subtask}` itemize | `scoring` |
| sample explanations, if any | `notes` |

**Send every field the package does not use as `""` in the same call** — at
minimum `notes` and `tutorial`, plus `scoring` when `format` is `"icpc"`. An
argument left off keeps its old value, and Phase 1b could not delete the
statement, so a field this package does not fill would still hold the *previous*
problem's prose. The editorial is never uploaded, so `tutorial` is always `""`.

**`interaction` is the exception: never send it at all.** Polygon accepts that
field only for a problem already marked interactive, so passing even an empty
one at a non-interactive problem risks a FAILED save of the whole statement.
This pack has no interactive problems, so the slot was never filled and there is
nothing to clear.

Rules that survive the transfer:

- Keep the `\begin{itemize}\item Subtask $1$ ($50\%$ số điểm): …` shape for
  `scoring` — Polygon renders it as the **Scoring** section. With points enabled
  (Phase 5) this field must exist, or readiness warns about tests-with-points
  and no scoring. A single full-score subtask still gets one `\item`.
- `name` must be a **single line**: Polygon rejects a value containing CR, LF,
  U+2028 or U+2029. Strip any trailing newline.
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

> **API:** [`problem.saveFile`](references/polygon-api.md#problemsavefile),
> [`problem.saveSolution`](references/polygon-api.md#problemsavesolution),
> [`problem.setValidator`](references/polygon-api.md#problemsetvalidator),
> [`problem.setChecker`](references/polygon-api.md#problemsetchecker);
> the full `tag` set is §6

Upload source files first, then bind roles. `polygon_save_file` takes
`file_type="source"` and either `path=` (preferred — the bytes go up exactly as
they sit on disk, with nothing round-tripping through you) or `content=`.
Everything in this pack is `source`; the `resource`/`aux` types are unused.

**A `path=` is resolved against the server's root, not your shell's working
directory** — the two start out the same, but a `cd` in your shell does not move
the server. Pass a path relative to `PROBLEM`, or an absolute one; never one
relative to a directory you changed into.

Every save **overwrites by name**, which is what a re-upload wants.

1. **Validator.** `polygon_save_file(problem_id, file_type="source",
   name="validator.cpp", path="outputs/validator.cpp")`, then
   `polygon_set_validator(problem_id, "validator.cpp")`. Polygon compiles it at
   save time from that single file, so it must be self-contained: `testlib.h` is
   the only include Polygon resolves. Any other local `#include "…"` fails with
   `No such file or directory` — inline the header into a temporary copy and
   upload that, leaving `outputs/validator.cpp` unchanged.
2. **Generator.** `polygon_save_file` for `outputs/gentest.cpp` as
   `name="gentest.cpp"` — the name must match the `gentest` token in
   `outputs/test-script.txt`. No role binding needed.
3. **Checker.** Read `checker` from `outputs/problem.json` and take the matching
   branch. Never decide it here: a missing or malformed `checker` key means the
   package is unfinished — **stop** and run `polygon-checker`.

   - `{ "kind": "custom", "file": "checker.cpp" }` → `polygon_save_file(problem_id,
     file_type="source", name="checker.cpp", path="outputs/checker.cpp")`, then
     `polygon_set_checker(problem_id, "checker.cpp")`. Same self-containment
     rule as the validator. If the file is missing, stop.
   - `{ "kind": "stock", "name": "std::ncmp.cpp" }` →
     `polygon_set_checker(problem_id, "std::ncmp.cpp")`, token passed through
     **verbatim**. Upload no checker source, and expect no `outputs/checker.cpp`.
4. **Solutions.** Upload **every** solution with
   `polygon_save_solution(problem_id, name=<basename>, path=…, tag=<resolved>)`.

**The tag is the measured verdict, not the filename.** Resolve it per file, in
this order:

1. **`outputs/solution/manifest.md` states a `Polygon tag:` line for the file**
   → use that tag **verbatim**. It exists precisely because the measured verdict
   departs from the filename prefix, and overriding it re-creates the bug it
   records.
2. **Otherwise** the filename prefix supplies the default:

   | package file | Polygon type | tag |
   |---|---|---|
   | `source/solution.cpp` — the validated official solution | Main correct | `MA` |
   | `outputs/solution/ac-*.cpp` | Accepted | `OK` |
   | `outputs/solution/tle-*.cpp` | Time limit exceeded | `TL` |
   | `outputs/solution/wa-*.cpp` | Incorrect | `RJ` |

The prefix is a *default*, and it is the half that can be wrong: it records what
the author intended, while `verify=true` checks what the file actually does. A
mismatch fails the entire package:

```text
PackageException: tle-dfs-enumeration.cpp got RE on tests#3
which violates tag(s): solution tag TIME_LIMIT_EXCEEDED
```

A candidate that fails *different ways on different tests* — RE on the large
tests, TLE on the medium ones — is `RJ`, and the manifest should already say so.
If you hit that error and the manifest has no `Polygon tag:` line, the gap is
upstream: fix it in `creating-problems` Step 4 so the next re-upload does not
re-derive the same broken tag, then upload again. Do not patch the tag only here.

Notes on the mapping:

- Exactly one `MA` — readiness warns when the main count ≠ 1. Upload
  `source/solution.cpp` as `sol-main.cpp` so it cannot collide with a suite
  filename. With no `source/solution.cpp`, promote one full-scope `ac-full-*.cpp`
  to `MA` and leave the rest `OK`.
- `ac-subtask-*.cpp` is correct only inside its subtask, so it fails the full
  test set: upload it as `RJ`, not `OK`. This prefix default already disagrees
  with its own prefix — an explicit `Polygon tag:` line is better.
- `RJ` means "any verdict except accepted", which is exactly what a `wa-*`
  candidate is. Do not narrow it to `WA` on a guess. This pack uses `MA`, `OK`,
  `RJ`, `TL`, and `NR` for a Phase 1b leftover.
- Per-testset or per-group overrides (`problem.editSolutionExtraTags`) are never
  called: one testset, no groups.
- A `light-weight` package legitimately has a single AC and no WA/TLE zoo. Not
  an error.

---

### 5. Tests

> **API:** [`problem.enablePoints`](references/polygon-api.md#problemenablepoints),
> [`problem.saveTest`](references/polygon-api.md#problemsavetest),
> [`problem.saveScript`](references/polygon-api.md#problemsavescript),
> [`problem.tests`](references/polygon-api.md#problemtests)

**Points, no groups.** `format` in `outputs/problem.json` decides: `"oi"` →
`polygon_enable_points(problem_id, true)`; `"icpc"` → leave points off, and skip
`scoring` too. Enabling a flag that is already set is harmless on a re-upload.

**Groups are not available and that is deliberate.** The server ships no group
tool at all. Every test is scored on its own: a solution earns the points of
each test it passes, whatever it does on the others. The statement's subtask
list stays in the `scoring` field as the description of the ladder; it is not
mirrored into Polygon groups.

**Contestants can see this divergence.** `subtasks[].points` in
`outputs/problem.json` sets the statement's `% số điểm` prose and the sum check;
it is never uploaded as group points. A contestant who clears only `g1` earns
one point per g1 test passed, not the 40 the ladder advertises. Do not try to
"fix" that during an upload — per-test scoring is the pack's choice, and
changing it is a change to this skill and to the server, not to one problem.

**Samples first (indices `1..S`).** The samples are the files in
`outputs/example-test/`; upload them as manual tests **before** the script. On a
re-upload Phase 1b already emptied the testset, so `1..S` are free — if
`polygon_tests` still reports anything here, Phase 1b did not finish and this
phase does not start.

1. Take `test_<i>.inp` and `test_<i>.out` in index order — `S` is how many pairs
   the directory holds (normally 1 or 2). The `.out` is already the validated
   solution's own output from Step 5b: do not re-run, reformat or hand-check it.
2. Run `outputs/validator.cpp` on each sample input as a last check; a rejected
   sample is a package bug — stop and fix it in Step 5b.
3. Upload each with **all** of these set together — the "use in statements" flag
   alone leaves the Examples block empty:

```text
polygon_save_test(
  problem_id, testset="tests", test_index=i,
  test_input=<test_i.inp content>,
  use_in_statements=true,
  input_for_statements=<same input>,
  output_for_statements=<test_i.out content>,
  verify_for_statements=true,
  test_points=0)
```

If you drop to the raw
[`problem.saveTest`](references/polygon-api.md#problemsavetest): every statement
key carries the `test` prefix and the plural `Statements`
(`testInputForStatements`, `testOutputForStatements`). The singular
`inputForStatement` / `outputForStatement` are fields of the `Test` object that
`problem.tests` *returns*; passing those names to `saveTest` sets nothing and is
silently ignored — the classic empty-Examples bug. **Write plural, read
singular.**

Note what is *not* in the parameter list: there is **no expected answer**.
Polygon stores none — the answer is whatever the `MA` solution prints, which is
why exactly one `MA` matters. The only answer text you upload is
`output_for_statements`, the sample answer *displayed* in the statement, and
`verify_for_statements=true` is what makes Polygon check it against the main
solution at build time. So a stale sample `.out` fails the build rather than
silently publishing a wrong example.

**Samples are never graded: `test_points=0`.** If points are assigned later,
**re-upload the samples last** so their statement I/O is not cleared.

**Script.** `outputs/test-script.txt` transfers almost verbatim, with two edits:

- Every line ends `> $` in the package. Manual samples occupy `1..S`, so
  **rewrite each `$` to an explicit index** `S+1, S+2, …` in the original order.
  Indices must not collide with the samples.
- Strip any `#` comment line — Polygon's parser rejects them
  (`source: Line should end with > testIndex`).

Nothing else changes. The seeds and `--subtask`/`--rate` options are what
produced the local tests, and the generator is a pure function of argv
(`registerGen(argc, argv, 1)`), so Polygon reproduces byte-identical input. If a
line's argv is not the one that generated the local file, regenerate the suite
with `generating-tests` — do not guess argv that makes different tests. Then
`polygon_save_script(problem_id, "tests", source=…)`, which replaces the whole
script in one call.

**Points.** After every test exists, give each **non-sample** test the same
value, `polygon.total_points / <number of non-sample tests>` from
`preference.yml` — 100 points over 100 generated tests is 1 point each. State
the arithmetic (total, test count, per-test value) before applying it, and make
the values **sum exactly** to `total_points`: with an integer base, put the
remainder on the first tests. `testPoints` is a non-negative double below `10^9`
with **at most two decimal places**, so `100/3` must be rounded to 2 dp and the
rounding error absorbed by the first tests — never uploaded as `33.333`, which
Polygon rejects. Samples stay at 0, so the graded total is carried entirely by
the generated tests.

Assign them in one call:

```text
polygon_save_test_points(problem_id, testset="tests",
                         points={"3": 1.0, "4": 1.0, …})
```

It sends `testPoints` and nothing else per test, so a generated test's input and
script line are untouched. It stops at the first refusal and reports
`applied_count` and `failed_index` so the run resumes from there, and returns
`total_points` — check it against `preference.yml` before moving on.

**Validator groups.** With groups off, Polygon never calls `validator.group()`,
which matches the pack's default validator: it validates the full legal input
domain and does not branch on a group. A validator that *requires* a group name
would fail the build here; that is a bug to fix in `polygon-validator`.

**Verify before committing:** `polygon_tests(problem_id, "tests",
no_inputs=true)` — tests `1..S` each show `useInStatements: true` and non-null
`inputForStatement` / `outputForStatement`; every generated index still shows
`manual: false` with its `scriptLine`; the points are what you assigned.

---

### 6. Cautions → commit (no email) → build

> **API:** [`problem.cautions`](references/polygon-api.md#problemcautions),
> [`problem.commitChanges`](references/polygon-api.md#problemcommitchanges),
> [`problem.buildPackage`](references/polygon-api.md#problembuildpackage),
> [`problem.packages`](references/polygon-api.md#problempackages);
> package states are §6

1. `polygon_cautions(problem_id)`. Judge each entry rather than obeying it.
   Acceptable: no validator tests, no checker tests, no package yet, empty
   tutorial. A `NO_CHECKER_TESTS` / `NO_VALIDATOR_TESTS` caution is *suppressed*
   while the corresponding `NO_CHECKER` / `NO_VALIDATOR` is present — one
   disappearing is not the same as it being fixed. Polygon's own package build
   is the authority over any readiness verdict.
2. `polygon_commit(problem_id, minor_changes=true,
   message="<slug>: upload from cp-problem-generation")`. `minor_changes=true`
   is how Polygon commits **without sending email**.
   - **Read `committed`, not `ok`.** `ok: true` with `committed: false` and
     message "No changes" means nothing was saved — on a re-upload that means an
     earlier phase silently wrote nothing, so go back and look rather than
     reporting success.
   - `conflict_occurred: true` means someone else moved the working copy under
     you. `polygon_update_working_copy` and commit again, or stop and tell the
     user.
3. `polygon_build_package_and_wait(problem_id, full=true, verify=true)`. It
   reads the committed revision, starts the build and polls until every package
   for *that* revision settles, then reports `states` (type → state) and
   `all_ready`. `verify=true` runs every solution on every test, so the Phase 4
   tags are actually checked.

   **`full=false` proves nothing about the tests.** It produces only a
   `standard` package, which never materialises the generated tests — so the
   generator and the validator never run, and the build reaches `READY` on a
   package that cannot generate. Always `full=true`, and check that **`linux`
   and `windows` reached `READY`**, not just `standard`.

   - `all_ready: true` with all three types **and `built_new: true`** → done.
   - `built_new: false` means Polygon handed back packages that already
     existed rather than building one, so they predate this call — they are
     not evidence that the fix you just committed was built. Check the
     revision, and commit before rebuilding if it did not move.
   - A `FAILED` package's `comment` says why. Read it; never re-commit blindly
     after a build failed on the script or a solution.
   - `PackageException: <file>.cpp got <VERDICT> on tests#<N> which violates
     tag(s): solution tag <TAG>` means `verify=true` observed a verdict the
     Phase 4 tag does not allow. The tag is wrong, **not** the test — do not
     delete the test or loosen the limit to make the tag true. Fix it in
     `creating-problems` Step 4 with a `Polygon tag:` line (`RJ` when the
     candidate fails different ways on different tests), then re-upload that
     solution.
   - Do **not** rebuild if a full verified package for this revision already
     exists; use it, or commit a new revision first.
   - After assigning points, commit and rebuild so scoring sticks.
   - `PackageException: Got exception while generating tests: Can't generate
     input or answer for test <N> [… Validator 'validator.exe' returns exit code
     3 [FAIL <msg>]]` means the validator rejected a regenerated test (testlib
     exit 3 = FAIL). Causes, in this order:

     1. **A generator that forces bare LF** — the most likely and the least
        visible. Re-run the Phase 0 step 7 grep for `_setmode`/`_O_BINARY`
        **before** suspecting anything else. `FAIL Expected EOLN (stdin, line 1)`
        is this one almost every time.
     2. A script line whose argv does not reproduce the intended subtask/bounds.
     3. A generator violating a validator constraint.

     All three are upstream bugs — none is fixed here.

     Two traps in reading it:

     - `Can't generate input or answer for test <N> [Request skipped because of
       previous errors in the requests batch [Reason: …]]` means test `N` was
       **skipped**. The `Reason` belongs to a *different* test in the same batch,
       and `N` changes between rebuilds of identical data. Do not bisect toward
       test `N`, and do not read `(stdin, line 1)` as a claim about that test.
     - To see what Polygon actually feeds the validator, temporarily bind a
       validator that dumps the first ~36 characters with `{CR}`/`{LF}`/`{SP}`
       markers via `quitf(_fail, …)`. The package error quotes the bytes
       verbatim. One build settles what six builds of inference will not.

**An OK verification is not a package — always create one after it.** Clean
cautions, `committed: true`, solutions verifying against their tags, samples
reading back right: none of those produce a package. The build is a separate,
mandatory action, and the upload is unfinished until a **full, verified** package
is `READY` for the committed revision. So whenever a verification comes back OK —
the first time, and again after every later fix, points assignment or re-commit —
the next call is `polygon_build_package_and_wait`, not a completion report.
Never tell the user the problem is on Polygon while its newest revision has no
`READY` package; say what is still building or what failed instead.

---

### 7. Grant `codeforces` READ

> **API:** [`problem.setAccess`](references/polygon-api.md#problemsetaccess),
> [`problem.accesses`](references/polygon-api.md#problemaccesses)

Importing the problem into a Codeforces contest requires the special
`codeforces` user to have **READ** access.

1. `polygon_set_access(problem_id, "codeforces", "READ")`.
2. Verify with `polygon_accesses(problem_id)` — the direct entries sorted by
   login (users and `@groups` alike, ungrouped and not expanded). The
   `codeforces` row must read `READ`.

Facts that decide whether the call can work at all:

- It takes effect **immediately** — no commit — and is stored at problem level,
  outside the working copy. Phase 6 need not re-run.
- It needs **direct** WRITE or OWNER access. Access inherited through a user
  group is *not* enough, and `polygon_accesses` succeeding (it only needs
  *effective* access) does not prove `set_access` will.
- Repeating a state already stored is a successful no-op: no email, no
  modification-time bump, no rate-limit consumption. Re-running the upload does
  not re-notify anyone.
- `"NONE"` removes a direct entry; OWNER cannot be assigned and an owner cannot
  be downgraded. The caller can remove **their own** non-owner access — so never
  call it on the uploading account's own login.
- Rate limit: 60 *real* changes per minute per calling user.
- It fails for: an unknown login, an `@group` login, a bad access type,
  insufficient direct access, an ownership change, a sample or example problem,
  an exceeded rate limit, or a wrong pin. Report the `error` verbatim rather
  than guessing which one it was.

**Manual fallback**, when the call fails for insufficient direct access or the
server is absent: hand the owner the working URL — the session link the web UI
shows, `https://polygon.codeforces.com/generalInfo?ccid=<ccid>&session=<session>`
— **not** a fabricated `https://polygon.codeforces.com/cf/<id>`: the numeric id
in `outputs/polygon.json` is not a web URL. Take the link from a tool result
when one is returned, otherwise ask the logged-in owner to copy it from the
address bar. From there: **Manage access** → add user `codeforces` with access
type **READ**. Do not mark this done until they confirm.

---

## Appendix — direct Polygon API calls

The bundled server covers every phase, so this is for the case where a tool is
genuinely missing rather than a routine escape hatch. It is the same API the
server wraps; credentials are its `POLYGON_API_KEY` / `POLYGON_API_SECRET`.

**Everything about the raw API lives in
[`references/polygon-api.md`](references/polygon-api.md)**: base URL and signing
in §2 (pure `hashlib`/`random`/`time`/`urllib`), envelope and throttling in §3,
methods in §4. Look the call up there; do not reconstruct it from memory. Note
the envelope differs from the server's: raw `status` is `"OK"` or `"FAILED"`,
a failure carries `comment` and normally no `result`, and `FAILED` arrives with
HTTP 400. Never test a raw response for `ok`.

Sign the request yourself — do not import the server's own package to do it;
outside its venv its compiled deps fail to import.

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
- [ ] When that name already existed, Phase 1b ran: working copy discarded,
      script cleared, testset verified empty via `polygon_tests`, and every
      file/solution/statement leftover the API cannot delete reported with the
      web-UI step (or a leftover solution re-tagged `NR`)
- [ ] `outputs/polygon.json` records `id`, `owner`, `name`
- [ ] Limits match `source/problem-context.md`, else `outputs/problem.json`,
      and `input_file`/`output_file` are the `.inp`/`.out` pair `io` names; no
      unconfirmed provisional limit published
- [ ] Statement uploaded under `lang="english"` with Vietnamese content; `name`
      single-line; display math `$$...$$`; `scoring` present (OI) and matching
      the statement's subtasks; every unused field sent as `""` except
      `interaction`, which was not sent at all
- [ ] `validator.cpp` and `gentest.cpp` uploaded and bound; the checker bound
      per `outputs/problem.json` — `checker.cpp` uploaded on `"custom"`, the
      `std::` token bound with no upload on `"stock"`
- [ ] Every solution uploaded with its resolved tag — a `Polygon tag:` line in
      `outputs/solution/manifest.md` used verbatim where one exists, the
      filename-prefix default only where none does; exactly one `MA`
- [ ] Points enabled (OI); every non-sample test carries an equal share of
      `preference.yml` → `polygon.total_points`, summing exactly
- [ ] Samples uploaded as tests `1..S` with statement I/O verified via
      `polygon_tests`; 0 points
- [ ] `test-script.txt` uploaded with explicit indices from `S+1`, comments
      stripped, argv unchanged; the readback still shows `manual: false` and a
      `scriptLine` for every generated index
- [ ] `outputs/gentest.cpp` forces no line-ending mode — the Phase 0 step 7 grep
      came back empty
- [ ] `polygon_commit` reported `committed: true` with `minor_changes=true`
- [ ] A package was **created after** the verification came back OK — and again
      after every later fix or re-commit — with `all_ready: true`,
      `built_new: true`, and `linux` and `windows` among the `READY` types, not
      `standard` alone
- [ ] `codeforces` READ granted and confirmed by `polygon_accesses` — or, when
      the call failed for insufficient direct access, granted in the web UI and
      confirmed by the user
