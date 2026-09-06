# Polygon HTTP API — reference for this skill

Source: <https://codeforces.github.io/polygon-misc/API>. Base URL
`https://polygon.codeforces.com/api/<methodName>`.

This file is the **lookup table**: find the phase or the action you are about to
perform in §1, read the exact method and parameter spelling in §4, then call it.
Do not grep the web docs and do not guess a parameter name — every parameter
this skill can need is spelled out below. A method that is not in §4 is a method
this skill does not use.

---

## 1. Action → method index

The one table to look at before any call. The `Tool` column names the bundled
`polygon` server's wrapper — read its loaded schema for the exact argument
spelling, and see [`polygon-tools.md`](polygon-tools.md) for the registry — but
the **raw** column is authoritative about what Polygon actually accepts.

| Phase | Action | Raw method | Tool |
|---|---|---|---|
| 0 | Prove the key, secret and clock | [`problems.list`](#problemslist) | `polygon_whoami` |
| 1 | Find a problem by name | [`problems.list`](#problemslist) (plural!) | `polygon_problems_list` |
| 1 | Create the problem | [`problem.create`](#problemcreate) | `polygon_problem_create` |
| 1 | Read id / revision / accessType | [`problems.list`](#problemslist), [`problem.info`](#probleminfo) | `polygon_problems_list`, `polygon_problem_info` |
| 1b | Throw away uncommitted work | [`problem.discardWorkingCopy`](#problemdiscardworkingcopy) | `polygon_discard_working_copy` |
| 1b | Clear the test script | [`problem.clearScript`](#problemclearscript) | `polygon_clear_script` |
| 1b | List surviving tests | [`problem.tests`](#problemtests) with `noInputs=true` | `polygon_tests` |
| 1b | Delete tests | [`problem.deleteTest`](#problemdeletetest) | `polygon_delete_tests` |
| 1b | Diff files for leftovers | [`problem.files`](#problemfiles), [`problem.solutions`](#problemsolutions) | `polygon_files`, `polygon_solutions` |
| 1b | Find stale statement languages | [`problem.statements`](#problemstatements) | `polygon_statements` |
| 1b | Neutralise a leftover solution | [`problem.saveSolution`](#problemsavesolution) with `tag=NR` | `polygon_save_solution` |
| 2 | Time / memory / I/O files | [`problem.updateInfo`](#problemupdateinfo) | `polygon_problem_update_info` |
| 3 | Statement fields | [`problem.saveStatement`](#problemsavestatement) | `polygon_save_statement` |
| 4 | Upload validator / generator / checker source | [`problem.saveFile`](#problemsavefile) `type=source` | `polygon_save_file` |
| 4 | Bind the validator | [`problem.setValidator`](#problemsetvalidator) | `polygon_set_validator` |
| 4 | Bind the checker (incl. `std::` token) | [`problem.setChecker`](#problemsetchecker) | `polygon_set_checker` |
| 4 | Upload a solution with its tag | [`problem.saveSolution`](#problemsavesolution) | `polygon_save_solution` |
| 5 | Turn per-test points on | [`problem.enablePoints`](#problemenablepoints) (no `testset`) | `polygon_enable_points` |
| 5 | Upload a sample | [`problem.saveTest`](#problemsavetest) | `polygon_save_test` |
| 5 | Set points on many tests | [`problem.saveTest`](#problemsavetest) ×N | `polygon_save_test_points` |
| 5 | Upload the generator script | [`problem.saveScript`](#problemsavescript) | `polygon_save_script` |
| 5 | Read tests back | [`problem.tests`](#problemtests) | `polygon_tests` |
| 6 | Readiness / cautions | [`problem.cautions`](#problemcautions) | `polygon_cautions` |
| 6 | Commit without email | [`problem.commitChanges`](#problemcommitchanges) `minorChanges=true` | `polygon_commit` |
| 6 | Build and wait for a package | [`problem.buildPackage`](#problembuildpackage) (does **not** wait) + [`problem.packages`](#problempackages) | `polygon_build_package_and_wait` |
| 6 | Poll for a READY package | [`problem.packages`](#problempackages) | `polygon_packages` |
| 7 | Grant `codeforces` READ | [`problem.setAccess`](#problemsetaccess) | `polygon_set_access` |
| 7 | Verify the grant | [`problem.accesses`](#problemaccesses) | `polygon_accesses` |

**Methods this skill must never call:** `problem.enableGroups`,
`problem.saveTestGroup`, `problem.setTestGroup`, `problem.viewTestGroup`
(groups are deliberately off — see SKILL.md Phase 5),
`problem.editSolutionExtraTags` (one testset, no groups),
`problem.setInteractor` / `problem.interactor` (no interactive problems),
`problem.saveValidatorTest` / `problem.saveCheckerTest` (not part of the pack),
`problem.setMaterial` / `problem.materials`, `problem.saveTags`,
`problem.saveGeneralDescription`, `problem.saveGeneralTutorial`,
`problem.saveNote`.

**Methods that do not exist** — do not look for them, do not invent them:
`problem.deleteFile`, `problem.deleteSolution`, `problem.deleteStatement`,
`problem.addUser`, `problem.saveUser`. Files, solutions and statements are
replaced by saving over the same name; only the web UI deletes them.

---

## 2. Authentication and signing

Every request carries three extra parameters:

| Parameter | Value |
|---|---|
| `apiKey` | the API key's *key* half (`POLYGON_API_KEY`) |
| `time` | current UNIX time in seconds, as a string |
| `apiSig` | `rand + sha512hex(base)` (see below) |

`time` must be within **5 minutes** of Polygon's clock; a skewed local clock
returns the same failure as a bad signature.

Signature:

1. `rand` = any 6 characters (random digits are fine; use a fresh one per request).
2. Take all parameters **including** `apiKey` and `time`, **excluding** `apiSig`.
3. Sort them lexicographically **by name, then by value** — a repeated name such
   as `testIndex` sorts among its own values.
4. Join as `name=value&name=value`, using the **raw** values — *not*
   URL-encoded ones. Encoding both the signature base and the request body
   passes until the first value that actually needs escaping (e.g.
   `checker=std::wcmp.cpp`), then fails with `apiKey: Incorrect signature`.
   Sign raw; encode only the transmitted body.
5. `base = "<rand>/<methodName>?<joined>#<secret>"`.
6. `apiSig = rand + sha512(base)` in lower-case hex.

Send GET parameters in the query string; anything with a large value (a source
file, a test input, a script, `items`) goes as a POST form body. The signature is
computed over the same parameter set either way.

**Pin.** If the problem has a pin code, add the `pin` parameter to the request
(and include it in the signature like any other parameter).

**Do not import the bundled server's own package to sign** — outside its venv
its compiled deps fail to import. `hashlib`, `random`, `time`, `urllib` are
enough.

## 3. Envelope, errors, throttling

Raw API responses are JSON:

| Field | When |
|---|---|
| `status` | always — `"OK"` or `"FAILED"` |
| `comment` | on `FAILED` — the reason, in plain text |
| `result` | on `OK` — method-dependent; **absent for methods that return nothing** |

`FAILED` normally arrives with HTTP 400 (bad parameter, access violation).
A successful call with no `result` field (`problem.setAccess`,
`problem.saveFile`, …) is **not** a failure.

This envelope is *not* the tool envelope. Every `polygon_*` tool returns a dict
carrying `ok`, and a failure is `{"ok": false, "error": "<this comment>",
"method": "<this method>"}`, sometimes with `details`. Never test a raw response
for `ok`, and never test a tool result for `status`.

**Non-JSON methods.** These return the raw file with its own MIME type, not an
envelope — do not parse them as JSON: `problem.viewFile`,
`problem.viewSolution`, `problem.script`, `problem.testInput`,
`problem.testAnswer`, `problem.viewStatementResource`, `problem.package`.

**Access.** Every `problem.*` method needs READ access; `problem.commitChanges`,
`problem.saveNote` and `problem.buildPackage` need WRITE. `problem.setAccess`
needs **direct** WRITE or OWNER — access inherited via a user group does not
count, and `problem.accesses` succeeding (effective access is enough for it)
does not prove `setAccess` will.

**Throttling.** Bursts get a plain `429 Too Many Requests` with an HTML body,
typically around the ~25th rapid call; 502/503/504/521 also happen. **Sleep ~2 s
between calls plus backoff** — a measured ~1.2 s gap still drew frequent `429`s
while assigning points to 100 tests, so budget several minutes for a points
pass. Then verify the whole range and retry the stragglers.
`problem.setMaterial` additionally caps at 60 attempts/minute;
`problem.setAccess` at 60 *real* changes/minute (a no-op repeat costs nothing).

---

## 4. Methods

Parameters are listed as `name` — *required* or *optional* — meaning.
An omitted optional parameter **leaves the stored value unchanged**; it does not
clear it.

### problems.list

Problems the caller can access. Note the plural `problems`.

- `showDeleted` — optional bool — include deleted problems (default: hidden)
- `id` — optional — filter by problem id
- `name` — optional — filter by name
- `owner` — optional — filter by owner login

Returns: list of [`Problem`](#problem).

### problem.create

- `name` — **required** — the problem name

Returns: [`Problem`](#problem) — `result.id` is the numeric id every later call
takes as `problemId`.

### problem.info

Returns: [`ProblemInfo`](#probleminfo-object).

### problem.updateInfo

All optional; omitted fields are untouched.

- `inputFile` — 1–64 chars
- `outputFile` — 1–64 chars
- `interactive` — bool
- `wellFormed` — bool
- `skipDuplicatedTestsValidation` — bool
- `timeLimit` — ms, **250–15000, divisible by 50**
- `memoryLimit` — MB, **4–1024**

`FAILED` if the effective input and output file names are equal ignoring case
(`data.txt`/`DATA.TXT`). `stdin`/`stdout` and a distinct `.inp`/`.out` pair are
both fine.

### problem.accesses

Direct access-control entries only — users and `@groups` alike, ungrouped, not
expanded. Sorted by login.

Returns: list of [`ProblemAccess`](#problemaccess).

### problem.setAccess

Sets one login's **direct** access. Takes effect immediately, outside the
working copy — no commit needed.

- `login` — **required**
- `accessType` — **required** — `READ` / `WRITE` / `NONE`

Returns: nothing (`{"status":"OK"}` with no `result`).

`NONE` removes the direct entry. `OWNER` cannot be assigned and an owner cannot
be downgraded. The caller may remove their **own** non-owner access — so never
call this on the uploading account's login. `FAILED`/400 for: unknown login, an
`@group` login, bad `accessType`, insufficient direct access, an ownership
change, a sample/example problem, rate limit exceeded, wrong `pin`.

### problem.discardWorkingCopy

Discards **uncommitted** changes. Committed content survives. No parameters.

### problem.updateWorkingCopy

Updates the working copy. No parameters. (Not used by this skill.)

### problem.commitChanges

Requires WRITE access.

- `minorChanges` — optional bool — **`true` suppresses the notification email**
- `message` — optional — commit message

Returns: [`CommitResult`](#commitresult). `committed: false` with
`message: "No changes"` means nothing was written — investigate, do not report
success. `conflictOccurred: true` means someone moved the working copy under you.

### problem.statements

Returns: map of language → [`Statement`](#statement).

### problem.saveStatement

Creates or updates one language slot. **An omitted field keeps its old value** —
send `""` explicitly for every field this package does not fill.

- `lang` — **required** — the language slot (this skill always uses `english`)
- `encoding` — optional
- `name` — optional — **single line**; a value containing CR, LF, U+2028 or
  U+2029 is rejected
- `legend` — optional — the main statement body
- `input` — optional
- `output` — optional
- `scoring` — optional
- `interaction` — optional — interactive problems only
- `notes` — optional
- `tutorial` — optional
- `showInReview` — optional bool
- `showCautionsAndGrammaticalFixes` — optional bool

### problem.renderStatements

Renders statements and tutorials from the working copy. Capped at 1 minute per
render, 4 minutes total.

- `includeContent` — optional bool — include Base64 content

Returns: [`RenderStatements`](#renderstatements).

### problem.files

Returns: an object with `resourceFiles`, `sourceFiles`, `auxFiles`, each a list
of [`File`](#file).

### problem.solutions

Returns: list of [`Solution`](#solution).

### problem.saveFile

Adds or **overwrites by name**.

- `checkExisting` — optional bool — `true` makes the call **add-only** and it
  fails on an existing name. Never pass it on a re-upload.
- `type` — **required** — `resource` / `source` / `aux`
- `name` — **required**
- `file` — optional on edit — the content
- `sourceType` — optional
- `forTypes` — optional, `resource` only — semicolon-separated file types
- `stages` — optional, `resource` only — `COMPILE` or `RUN`
- `assets` — optional, `resource` only — `VALIDATOR` / `INTERACTOR` / `CHECKER` / `SOLUTION`

Polygon compiles a saved source at save time from that single file: `testlib.h`
is the only include it resolves. Any other local `#include "…"` fails with
`No such file or directory`.

### problem.saveSolution

Adds or **overwrites by name**.

- `checkExisting` — optional bool — same add-only trap as `saveFile`
- `name` — **required**
- `file` — optional on edit
- `sourceType` — optional
- `tag` — optional — see [solution tags](#solution-tags)

### problem.viewFile

Non-JSON.

- `type` — **required** — `resource` / `source` / `aux`
- `name` — **required**

### problem.viewSolution

Non-JSON.

- `name` — **required**

### problem.setValidator

- `validator` — **required** — the source filename

### problem.setChecker

- `checker` — **required** — a source filename, or a standard checker token
  passed verbatim (`std::ncmp.cpp`, `std::wcmp.cpp`, …)

### problem.validator / problem.checker / problem.interactor

No parameters. Each returns the currently bound name as a string.

### problem.extraValidators

No parameters. Returns an array of validator names.

### problem.setInteractor

- `interactor` — **required** — the source filename

### problem.script

Non-JSON — the raw script text.

- `testset` — **required**

### problem.saveScript

Replaces the whole script for a testset in one call.

- `testset` — **required**
- `source` — **required** — the script text

Polygon's parser rejects `#` comment lines
(`source: Line should end with > testIndex`), and every line must end with an
explicit `> <index>`.

### problem.clearScript

- `testset` — **required**

### problem.tests

- `testset` — **required**
- `noInputs` — optional bool — omit the input bodies; keeps the response small

Returns: list of [`Test`](#test).

### problem.saveTest

Adds or edits one test.

- `checkExisting` — optional bool
- `testset` — **required**
- `testIndex` — **required**
- `testInput` — optional on edit
- `testGroup` — optional
- `testPoints` — optional — non-negative double `< 10^9`, **at most 2 decimal
  places**
- `testDescription` — optional
- `testUseInStatements` — optional bool
- `testInputForStatements` — optional
- `testOutputForStatements` — optional
- `verifyInputOutputForStatements` — optional bool

**There is no `testOutput`.** Polygon stores no expected answer for a test — the
answer is whatever the main correct (`MA`) solution prints. The only answer text
you can upload is `testOutputForStatements`, the sample answer *displayed* in
the statement; `verifyInputOutputForStatements=true` makes Polygon check it
against the main solution at build time.

**Polygon stores a manual `testInput` as CRLF** however it was submitted —
form-urlencoded and multipart both end up CRLF — while
`testInputForStatements` in the same request keeps LF. CRLF is Polygon's
canonical test-file form, which is also why its `validator.exe` is compiled to
require it (see `polygon-validator`).

**Write plural, read singular.** The write parameters are
`testInputForStatements` / `testOutputForStatements`; the fields `problem.tests`
returns are `inputForStatement` / `outputForStatement`. Passing the singular
names to `saveTest` sets nothing and is silently ignored — the classic
empty-Examples bug.

### problem.deleteTest

Deletes one or more tests. All pre-checks run **before** any deletion.

- `testset` — **required**
- `testIndex` — optional, repeatable — or
- `testIndices` — optional — comma-separated

All-or-nothing on the pre-check: one bad index returns `FAILED` with comment
`Some tests can not be deleted.` and a
[`DeleteTestsResult`](#deletetestsresult), and **deletes nothing**. Read the
reasons: `FREEMARKER_SCRIPT_TEST` means the script did not clear (clear the
script first — script-generated tests cannot be deleted individually);
`NOT_FOUND` means the index list is stale; `DELETE_FAILED` is the one partial
case (comment `Some tests failed during deletion; problem state may be
partially modified.`) — re-read `problem.tests` before retrying.

### problem.previewTests

- `testset` — **required**

Returns: list of [`TestPreview`](#testpreview).

### problem.testInput / problem.testAnswer

Non-JSON — the raw generated file.

- `testset` — **required**
- `testIndex` — **required**

`problem.testInput` and `problem.tests` can disagree on line endings for the
same test. The JSON `input` field from [`problem.tests`](#problemtests) is the
reliable view of the bytes Polygon actually stored — use it when line endings
are what you are investigating.

### problem.enablePoints

Problem-wide — takes **no** `testset`.

- `enable` — **required** bool

### problem.enableGroups

- `testset` — **required**
- `enable` — **required** bool

### problem.enableTreatPointsFromCheckerAsPercent

- `enable` — **required** bool

### problem.viewTestGroup

- `testset` — **required**
- `group` — optional

Returns: list of [`TestGroup`](#testgroup).

### problem.saveTestGroup

- `testset` — **required**
- `group` — **required**
- `pointsPolicy` — optional — `COMPLETE_GROUP` / `EACH_TEST`
- `feedbackPolicy` — optional — `NONE` / `POINTS` / `ICPC` / `COMPLETE`
- `dependencies` — optional — comma-separated group names

### problem.setTestGroup

- `testset` — **required**
- `testGroup` — **required**
- `testIndex` — optional, repeatable — or `testIndices` comma-separated

### problem.editSolutionExtraTags

- `remove` — **required** bool
- `name` — **required**
- `testset` — optional
- `testGroup` — optional — one of `testset`/`testGroup` required
- `tag` — optional on add

### problem.cautions

Structured cautions and package-readiness issues for the **current working
copy**. No parameters. Starts no new AI request.

Returns: [`ProblemCautions`](#problemcautions-object) — always the four arrays
`common`, `statement`, `structure`, `issues` (each possibly empty) plus
`packageReadinessIssues`, `latestPackageWarnings` and cached `ai`.

`NO_CHECKER_TESTS` / `NO_VALIDATOR_TESTS` are **suppressed** while the
corresponding `NO_CHECKER` / `NO_VALIDATOR` caution is present — a missing-tests
caution disappearing is not the same as it being fixed.

### problem.buildPackage

Requires WRITE. **Starts** the build and returns immediately — there is no
waiting variant in the HTTP API; poll `problem.packages`.

- `full` — **required** bool — build the full package (standard + linux +
  windows; linux and windows carry the generated tests, standard does not)
- `verify` — **required** bool — run every solution on every test, checking the
  Phase 4 tags

### problem.packages

No parameters. Returns: list of [`Package`](#package). A finished upload needs a
**full, verified** package in state `READY` for the committed revision.

### problem.package

Non-JSON — the package ZIP.

- `packageId` — **required**
- `type` — optional — `standard` / `linux` / `windows` (default `standard`)

### problem.note / problem.saveNote

`note` returns the user-level note as a string. `saveNote` takes
`note` — **required**, ≤50 chars, empty clears — saves immediately (no commit),
needs WRITE.

### problem.statementResources / problem.viewStatementResource / problem.saveStatementResource

- `viewStatementResource`: `name` — **required**. Non-JSON.
- `saveStatementResource`: `checkExisting` optional bool, `name` **required**,
  `file` **required**.

### problem.validatorTests / problem.saveValidatorTest

- `saveValidatorTest`: `checkExisting` optional bool, `testVerdict`
  **required** (`VALID` / `INVALID`), `testIndex` **required**, `testInput`
  required on add, `testGroup` optional, `testset` optional. Normalises line
  breaks to CRLF; clears the previous run result.

### problem.checkerTests / problem.saveCheckerTest

- `saveCheckerTest`: `checkExisting` optional bool, `testVerdict` **required**
  (`OK` / `WRONG_ANSWER` / `PRESENTATION_ERROR` / `CRASHED`), `testIndex`
  **required**, `testInput` / `testOutput` / `testAnswer` required on add.

### problem.viewTags / problem.saveTags

`saveTags`: `tags` — **required** — comma-separated; **replaces** the whole set.

### problem.viewGeneralDescription / problem.saveGeneralDescription

`saveGeneralDescription`: `description` — **required**, may be empty.

### problem.viewGeneralTutorial / problem.saveGeneralTutorial

`saveGeneralTutorial`: `tutorial` — **required**, may be empty.

### problem.materials / problem.setMaterial

`setMaterial` — POST with parameters in the **body**; do not put `items` in a
query string. Max 60 attempts/minute/user.

- `name` — **required** — 1–40 chars
- `originalName` — optional — previous name, for an atomic rename
- `publishStrategy` — required on save — `NONE` / `WITH_TUTORIAL` / `WITH_STATEMENT`
- `items` — required on save — JSON array of [`MaterialItem`](#material), ≤262144 chars
- `remove` — optional bool

### contest.problems

- `contestId` — **required**

Returns: list of [`Problem`](#problem).

---

## 5. Returned objects

### Problem
`id`, `owner`, `name`, `note`, `deleted`, `favourite`, `accessType`, `revision`,
`workingCopyRevision`, `latestPackage`, `modified`

### ProblemAccess
`login`, `accessType`

### ProblemInfo (object)
`inputFile`, `outputFile`, `interactive`, `wellFormed`,
`skipDuplicatedTestsValidation`, `timeLimit`, `memoryLimit`

### CommitResult
`committed` (bool), `conflictOccurred` (bool), `message`

### Statement
`encoding`, `name`, `legend`, `input`, `output`, `scoring`, `interaction`,
`notes`, `tutorial`, `showInReview`, `showCautionsAndGrammaticalFixes`

### RenderStatements
`revision`, `renderingTimeSeconds`, `statements`, `tutorials` —
each a `RenderedStatement` (`language`, `html`, `pdf`), each render a
`RenderResult` (`status`, `sha256`, `sizeBytes`, `contentBase64`, `message`)

### File
`name`, `modificationTimeSeconds`, `length`, `sourceType`,
`resourceAdvancedProperties` (`forTypes`, `main`, `stages`, `assets`)

### Solution
`name`, `modificationTimeSeconds`, `length`, `sourceType`, `tag`

### Test
`index`, `manual`, `input`, `inputBase64`, `description`, `useInStatements`,
`scriptLine`, `group`, `points`, **`inputForStatement`**,
**`outputForStatement`**, `verifyInputOutputForStatements`

### DeleteTestsResult
`failures` — list of `TestDeletionFailure` (`index`, `reason`, `message`);
`reason` is one of `DUPLICATE` / `NOT_FOUND` / `FREEMARKER_SCRIPT_TEST` /
`DELETE_FAILED`

### TestPreview
`id`, `session`, `creationTime`, `updateTime`, `problemId`, `testset`,
`testIndex`, `description`, `useInStatements`, `input`, `inputSha1`, `answer`,
`changeAfterLastGenerate`, `testOverviewLog`

### TestGroup
`name`, `pointsPolicy`, `feedbackPolicy`, `dependencies`

### Material
`name`, `publishStrategy`, `items` — each `MaterialItem` (`type`, `tests`,
`names`), each `MaterialTest` (`testset`, `index`, `all`, `asInput`)

### Package
`id`, `revision`, `creationTimeSeconds`, `state`, `comment`, `type`

### ValidatorTest
`index`, `input`, `expectedVerdict`, `testset`, `group`, `runVerdict`,
`runComment`

### CheckerTest
`index`, `input`, `output`, `answer`, `expectedVerdict`, `runVerdict`,
`runComment`

### ProblemCautions (object)
`common`, `statement`, `structure`, `issues`, `packageReadinessIssues`,
`latestPackageWarnings`, `ai` — a `Caution` is (`type`, `severity`, `category`,
`message`, `parameters`); a `PackageReadinessIssue` is (`type`, `reason`,
`message`); `ai` is `AiTips` (`disabled`, `statements`, `validator`, `checker`)

---

## 6. Enumerations

| Enum | Values |
|---|---|
| `accessType` | `READ`, `WRITE`, `OWNER`, `NONE` — `OWNER` cannot be assigned |
| <a id="solution-tags"></a>solution `tag` | `MA` main correct, `OK` accepted, `RJ` incorrect (any non-AC), `TL`, `TO` (TL or accepted), `TM` (TL or ML), `WA`, `PE`, `ML`, `NR` do-not-run, `RE` |
| `pointsPolicy` | `COMPLETE_GROUP`, `EACH_TEST` |
| `feedbackPolicy` | `NONE`, `POINTS`, `ICPC`, `COMPLETE` |
| validator `testVerdict` | `VALID`, `INVALID` |
| checker `testVerdict` | `OK`, `WRONG_ANSWER`, `PRESENTATION_ERROR`, `CRASHED` |
| `publishStrategy` | `NONE`, `WITH_TUTORIAL`, `WITH_STATEMENT` |
| package `type` | `standard`, `linux`, `windows` |
| package `state` | `PENDING`, `RUNNING`, `READY`, `FAILED` |
| file `type` | `resource`, `source`, `aux` |
| resource `stages` | `COMPILE`, `RUN` |
| resource `assets` | `VALIDATOR`, `INTERACTOR`, `CHECKER`, `SOLUTION` |
| test deletion `reason` | `DUPLICATE`, `NOT_FOUND`, `FREEMARKER_SCRIPT_TEST`, `DELETE_FAILED` |

## 7. Numeric bounds

| Field | Bound |
|---|---|
| `timeLimit` | 250–15000 ms, divisible by 50 |
| `memoryLimit` | 4–1024 MB |
| `inputFile` / `outputFile` | 1–64 chars, and must differ ignoring case |
| statement `name` | single line — no CR, LF, U+2028, U+2029 |
| `testPoints` | at least 0, below 10^9, at most 2 decimal places |
| problem note | at most 50 chars |
| material `name` | 1–40 chars |
| material `items` | at most 262144 chars |
| `time` skew | at most 5 minutes |
