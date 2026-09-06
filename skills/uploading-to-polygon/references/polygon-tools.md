# Polygon tool registry

Every tool the bundled `polygon` MCP server exposes, paired with the Polygon
API method it wraps, for [uploading-to-polygon](../SKILL.md). Grouped by the
upload phase that reaches for it, so the question this file answers is "what do
I call in Phase 5" rather than "what does the server have".

**The loaded tool schema always wins over this file.** Load the real definitions
with `ToolSearch` (`select:polygon_save_test,polygon_save_script,…`) and read
their parameter names before calling anything — a registry file can go stale, a
loaded schema cannot. For what *Polygon* accepts, the authority is
[`polygon-api.md`](polygon-api.md); this file only maps tools to methods.

Parameter names below carry `=` when they have a default.

A tool never raises. Every call returns a dict carrying `ok`; a failure is
`{"ok": false, "error": "<Polygon's own comment>", "method": "<the API
method>"}`, sometimes with `details`. Read the comment and correct the call — do
not retry the identical request.

The credentials live in the server's own environment and never reach this skill.
Setup is in [`mcp-server/README.md`](../../../mcp-server/README.md).

## Phase 0 — preflight

| Tool | Polygon method |
| --- | --- |
| `polygon_whoami()` | `problems.list` |

The cheapest proof that the key, the secret and the clock are all good. Polygon
refuses a request whose timestamp is more than five minutes off its own clock,
so a skewed system clock fails exactly like a bad signature. Call it first when
any other tool fails. It also reports `path_reads_allowed_under` — the one
directory a `path=` argument may read from.

## Phase 1 — create or resolve

| Tool | Polygon method |
| --- | --- |
| `polygon_problems_list(show_deleted=, problem_id=, name=, owner=)` | `problems.list` |
| `polygon_problem_create(name)` | `problem.create` |
| `polygon_problem_info(problem_id)` | `problem.info` |

`problem.create` returns `id`, `owner`, `name`, `deleted`, `favourite` and
`accessType` — and **no address**. Polygon's web URLs are session-scoped and
cannot be built from the id, so never compose one.

A name that `problems_list` does not find but `problem_create` refuses as taken
means a deleted or unlisted problem holds it: re-run with `show_deleted=true`.

## Phase 1b — wipe, on a re-upload

| Tool | Polygon method |
| --- | --- |
| `polygon_discard_working_copy(problem_id)` | `problem.discardWorkingCopy` |
| `polygon_clear_script(problem_id, testset=)` | `problem.clearScript` |
| `polygon_tests(problem_id, testset=, no_inputs=)` | `problem.tests` |
| `polygon_delete_tests(problem_id, testset, test_indices)` | `problem.deleteTest` |
| `polygon_files(problem_id)` | `problem.files` |
| `polygon_solutions(problem_id)` | `problem.solutions` |
| `polygon_statements(problem_id)` | `problem.statements` |

`polygon_discard_working_copy` is destructive and not undoable — it throws away
everything saved since the last revision, and leaves committed content alone.

**Clear the script before deleting tests.** A script-generated test cannot be
deleted individually; `deleteTest` refuses it with reason
`FREEMARKER_SCRIPT_TEST`.

`polygon_delete_tests` takes the whole list in one call and is **all-or-nothing
on the pre-check**: one bad index and nothing is deleted, with
`details.failures` naming each `index`, `reason` (`DUPLICATE` / `NOT_FOUND` /
`FREEMARKER_SCRIPT_TEST` / `DELETE_FAILED`) and `message`. `DELETE_FAILED` is
the one partial case — re-read `polygon_tests` before retrying that one.

**There is no delete for files, solutions or statements.** The three list tools
above exist so leftovers can be *found* and reported; removing one needs the web
UI. A leftover solution can at least be neutralised with
`polygon_save_solution(name=…, tag="NR")` and no source.

## Phase 2 — limits

| Tool | Polygon method |
| --- | --- |
| `polygon_problem_update_info(problem_id, input_file=, output_file=, interactive=, well_formed=, time_limit_ms=, memory_limit_mb=)` | `problem.updateInfo` |

`input_file`/`output_file` are the problem's literal file names. Never pass the
word `"stdin"` — that creates a problem reading a file *called* `stdin`; a
stdin/stdout problem wants both arguments left off instead.

They are `str | None`: `None` leaves the stored value alone, `""` is transmitted
as an empty parameter. Polygon documents `inputFile` as 1–64 chars, so an empty
one may be refused.

Other bounds: `time_limit_ms` 250–15000 and divisible by 50; `memory_limit_mb`
4–1024; the effective input and output names must differ ignoring case.

## Phase 3 — statement

| Tool | Polygon method |
| --- | --- |
| `polygon_save_statement(problem_id, lang, encoding=, name=, legend=, input=, output=, scoring=, interaction=, notes=, tutorial=)` | `problem.saveStatement` |

Only `lang` is required. Every section is `str | None`: an omitted one keeps its
old value, and **`""` clears it** — which is the only way a re-upload blanks the
previous problem's prose, since Polygon has no delete-statement method.

`interaction` is the exception to the clear-it-with-`""` rule: Polygon accepts
that field only for a problem already marked interactive, so never send it at
all from this pack.

`name` must be a single line; a value containing CR, LF, U+2028 or U+2029 is
rejected rather than stored.

There is no `polygon_save_statement_resource`: this pack's statements carry no
figures.

## Phase 4 — checker, validator, generator, solutions

| Tool | Polygon method |
| --- | --- |
| `polygon_save_file(problem_id, file_type, name, content=, path=, source_type=)` | `problem.saveFile` |
| `polygon_set_validator(problem_id, name)` | `problem.setValidator` |
| `polygon_set_checker(problem_id, name)` | `problem.setChecker` |
| `polygon_save_solution(problem_id, name, tag, content=, path=, source_type=)` | `problem.saveSolution` |

`file_type` is `source` (checker, validator, generator), `resource` (something a
compile needs beside a source) or `aux`. This pack uploads everything as
`source`. `source_type` is Polygon's compiler id and may be left empty for
Polygon to guess from the extension.

`path=` is honoured only under the configured root, and the file is read as
bytes and decoded — **line endings are not translated**, so what is on disk is
what goes up. Prefer it over `content=`: nothing has to round-trip through the
model. A relative path is resolved **against the root**, not against the calling
shell's working directory, which can move independently of the server.

Every save **overwrites by name**, which is what a re-upload wants.

`polygon_set_checker` passes the name through untouched, so a `std::` token
(`std::ncmp.cpp`, `std::wcmp.cpp`, `std::rcmp6.cpp`, …) works with no file
uploaded.

`tag` is what the solution is *supposed* to do, and a verified build checks the
claim: `MA`, `OK`, `RJ`, `TL`, `TO`, `TM`, `WA`, `PE`, `ML`, `NR`, `RE`. Exactly
one solution carries `MA`, and Polygon computes every test's answer by running
it. Called with a name and a tag and no source, `polygon_save_solution` re-tags
a solution already uploaded.

There is no `polygon_set_interactor`: this pack has no interactive problems.

## Phase 5 — tests and points

| Tool | Polygon method |
| --- | --- |
| `polygon_enable_points(problem_id, enable)` | `problem.enablePoints` |
| `polygon_save_test(problem_id, testset, test_index, test_input=, path=, test_points=, test_description=, use_in_statements=, input_for_statements=, output_for_statements=, verify_for_statements=)` | `problem.saveTest` |
| `polygon_save_script(problem_id, testset, source)` | `problem.saveScript` |
| `polygon_script(problem_id, testset=)` | `problem.script` |
| `polygon_save_test_points(problem_id, testset, points)` | N × `problem.saveTest` |
| `polygon_tests(problem_id, testset=, no_inputs=)` | `problem.tests` |

`polygon_enable_points` is problem-wide and takes no `testset`.

**There are no group tools.** `problem.enableGroups`, `problem.setTestGroup`,
`problem.saveTestGroup` and `problem.viewTestGroup` have no wrapper, and
`polygon_save_test` has no `test_group` parameter. The skill scores every test
on its own; a rule that cannot be violated beats a rule that is written down.

`polygon_save_script` replaces the whole script — it is not a line appended.
Polygon's parser rejects `#` comment lines.

`polygon_save_test` has **no expected-answer parameter**, and that is not an
omission: Polygon stores no answer for a test, because the answer is whatever
the `MA` solution prints. `output_for_statements` is only the sample answer
*displayed* in the statement, and `verify_for_statements=true` makes Polygon
check it against the main solution at build time. A sample needs
`use_in_statements`, `input_for_statements`, `output_for_statements` and
`verify_for_statements` set **together** — the flag alone leaves the Examples
block empty.

`polygon_tests` returns the **singular** `inputForStatement` /
`outputForStatement`, while `polygon_save_test` takes the plural
`input_for_statements` / `output_for_statements`. Write plural, read singular.

`polygon_save_test_points` takes `points` as a map of test index (a string) to
points — `{"3": 1.0, "4": 1.0}` — and sends `testPoints` and nothing else per
test, so a generated test's input and script line are untouched. It exists so a
hundred-test ladder is one tool call rather than a hundred; budget about half a
second each. It stops at the first refusal and reports `applied_count` and
`failed_index`, and returns `total_points` so the ladder's sum can be checked.

## Phase 6 — cautions, commit, package

| Tool | Polygon method |
| --- | --- |
| `polygon_cautions(problem_id)` | `problem.cautions` |
| `polygon_update_working_copy(problem_id)` | `problem.updateWorkingCopy` |
| `polygon_commit(problem_id, minor_changes=, message=)` | `problem.commitChanges` |
| `polygon_build_package(problem_id, full=, verify=)` | `problem.buildPackage` |
| `polygon_build_package_and_wait(problem_id, full=, verify=, timeout_seconds=, poll_interval_seconds=)` | `problem.buildPackage`, then `problem.packages` |
| `polygon_packages(problem_id)` | `problem.packages` |

`polygon_cautions` returns `common`, `statement`, `structure` and `issues`, plus
`packageReadinessIssues`, `latestPackageWarnings` and a cached `ai` block. One
trap: `NO_CHECKER_TESTS` / `NO_VALIDATOR_TESTS` are *suppressed* while the
corresponding `NO_CHECKER` / `NO_VALIDATOR` is present, so one of them
disappearing is not the same as it being fixed.

**Read `committed`, not `ok`.** Polygon answers a no-op commit with a
*successful* envelope carrying `committed=false` and the message "No changes",
and reports a working copy that fell behind as `conflict_occurred=true`. On a
conflict, `polygon_update_working_copy` and commit again.
`minor_changes=true` suppresses the notification mail.

`polygon_build_package` returns as soon as the build is *queued*.
`polygon_build_package_and_wait` is the one to use: it reads the committed
revision first and follows only the packages built for **that** revision — which
is what "a READY package for the committed revision" means, and what taking the
newest row would get wrong. It returns `states` (package type → state) and
`all_ready`.

`full=true` is not optional. A `standard` package never materialises the
generated tests, so the generator and validator never run and a `standard`-only
`READY` is no evidence the tests generate. Check that `linux` and `windows` are
among the READY types.

It also reports `built_new` and `new_packages`: the packages **this call**
produced, as opposed to ones that were already there. Polling the instant
`buildPackage` returns can otherwise catch a revision whose previous build left
a full READY set and declare success for a build that never started. When
`built_new` is false the packages predate the call — do not read them as
evidence that your latest commit was built.

## Phase 7 — access

| Tool | Polygon method |
| --- | --- |
| `polygon_set_access(problem_id, login, access)` | `problem.setAccess` |
| `polygon_accesses(problem_id)` | `problem.accesses` |

`access` is `READ`, `WRITE` or `NONE`; `OWNER` is deliberately absent because
the API does not let this method assign ownership. Access lives at problem
level, so it takes effect immediately and needs no commit.

`problem.setAccess` needs **direct** WRITE or OWNER — access held only through a
user group is not enough, and that is the one failure worth predicting here.
`polygon_accesses` only needs *effective* access, so it succeeding does not
prove `polygon_set_access` will.
