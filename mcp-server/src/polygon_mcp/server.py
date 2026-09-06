"""MCP server exposing Polygon problem preparation: statements, files, tests, packages.

Adapted from the `competitive-programming` plugin by LamTer
(<https://github.com/LLaammTTeerr/competitive-programming>). Sized for the
`cp-problem-generation:uploading-to-polygon` skill, which means it differs from
that original in three ways that are all deliberate:

* **The group tools are gone.** `problem.enableGroups`, `problem.setTestGroup`,
  `problem.saveTestGroup` and `problem.viewTestGroup` have no wrapper here,
  because the skill scores every test on its own and never enables groups. A
  rule that cannot be violated beats a rule that is written down. So are
  `problem.setInteractor` (no interactive problems), `problem.saveTags`,
  `problem.saveGeneralDescription` and `problem.saveStatementResource`.
* **An empty string is a value, not an omission.** `polygon_save_statement` and
  `polygon_problem_update_info` take `str | None`, where `None` means "leave the
  field alone" and `""` means "store an empty field". The original collapsed
  both to "leave alone" with `value or None`, which silently defeats a
  re-upload that means to blank the previous problem's prose.
* **Five tools are new**: `polygon_clear_script`, `polygon_delete_tests`,
  `polygon_cautions`, `polygon_save_test_points` and
  `polygon_build_package_and_wait`. Each backs a phase of the skill that the
  original had no call for.

Every tool wraps one Polygon API method — except the two that say otherwise in
their own docstring — names it, and returns a dict carrying `ok`. Failures come
back as `{"ok": false, "error": ..., "method": ...}` rather than raising, so the
model reads the comment Polygon sent and corrects itself. No tool returns, logs
or writes the API secret.
"""

from __future__ import annotations

import asyncio
import time
from typing import Any

from mcp.server.fastmcp import FastMCP

from .api import PolygonApi
from .config import (
    Config,
    PolygonError,
    read_text_verbatim,
    resolve_local_path,
)

mcp = FastMCP("polygon")

config = Config.from_env()
api = PolygonApi(config)

# The tags Polygon accepts on a solution. Kept here so a typo is caught before
# a request goes out rather than as a FAILED comment afterwards. The full set,
# not the subset the skill normally uses: a solution's tag comes from the
# package's own manifest, and narrowing this would reject a legitimate one.
SOLUTION_TAGS = ("MA", "OK", "RJ", "TL", "TO", "TM", "WA", "PE", "ML", "NR", "RE")
FILE_TYPES = ("resource", "source", "aux")
# The three states `problem.setAccess` accepts. OWNER is deliberately absent:
# the API documents that ownership cannot be assigned through this method.
ACCESS_TYPES = ("READ", "WRITE", "NONE")
# A package is still being built while it is in one of these.
PENDING_STATES = ("PENDING", "RUNNING")


def _fail(error: Exception, method: str) -> dict[str, Any]:
    """The single failure shape every tool returns.

    Every tool catches `Exception`, not just `PolygonError`: a traceback out of
    a tool reaches the model as a protocol-level error rather than as something
    it can read and correct. `str(error)` is safe here only because nothing in
    this package ever puts an httpx exception into the message — httpx spells
    the request URL into everything it raises, and a signed GET's URL carries
    `apiKey`.
    """
    failure = {
        "ok": False,
        "error": str(error),
        "method": getattr(error, "method", "") or method,
    }
    # Only when Polygon sent structured failure details, so every other tool's
    # failure shape is exactly what it was.
    details = getattr(error, "details", None)
    if details is not None:
        failure["details"] = details
    return failure


def _read_source(
    content: str, path: str, *, what: str = "content", required: bool = True
) -> str | None:
    """Resolve a `content`-or-`path` pair into the text to upload.

    A path is honoured only inside the configured root; see
    `config.resolve_local_path` for what that is and why. Files are read as
    bytes and decoded, never through text mode, so the line endings that reach
    Polygon are the ones on disk — see `config.read_text_verbatim`.

    `required=False` returns None when neither was given, for the methods whose
    edit mode means "change the metadata, leave the body alone".
    """
    if content and path:
        raise PolygonError(f"Pass either {what} or path, not both.")
    if path:
        return read_text_verbatim(resolve_local_path(path, config.root))
    if content:
        return content
    if required:
        raise PolygonError(f"Pass the {what} inline, or a path to a file holding it.")
    return None


def _one_of(name: str, value: str, allowed: tuple[str, ...]) -> str:
    if value not in allowed:
        raise PolygonError(f"{name} must be one of {', '.join(allowed)}; got {value!r}.")
    return value


# --------------------------------------------------------------------- tools


@mcp.tool()
async def polygon_whoami() -> dict[str, Any]:
    """Check that this server's Polygon credentials work. → `problems.list`

    Lists the problems the key can see and summarises them, which is the
    cheapest call that proves the key, the secret and the clock are all good.
    Call it first when any other tool fails, before re-reading its error.
    """
    status: dict[str, Any] = {
        "ok": True,
        "base_url": config.base_url,
        "credentials_configured": config.has_credentials,
        "path_reads_allowed_under": str(config.root),
    }
    if not config.has_credentials:
        return {
            **status,
            "ok": False,
            "error": "No Polygon credentials. Generate a key at Polygon → "
            "Settings → API keys and set POLYGON_API_KEY and "
            "POLYGON_API_SECRET for this server.",
            "method": "problems.list",
        }
    try:
        problems = await api.call("problems.list") or []
    except Exception as error:
        return {**status, **_fail(error, "problems.list")}
    return {
        **status,
        "problem_count": len(problems),
        "first_problems": [
            {"id": p.get("id"), "name": p.get("name"), "owner": p.get("owner")}
            for p in problems[:5]
        ],
    }


@mcp.tool()
async def polygon_problems_list(
    show_deleted: bool = False,
    problem_id: int | None = None,
    name: str = "",
    owner: str = "",
) -> dict[str, Any]:
    """List the problems this account can open. → `problems.list`

    Every filter is optional: `problem_id`, `name` and `owner` narrow the list,
    `show_deleted` brings back deleted problems. Each entry carries the `id`
    that every other tool here takes as `problem_id`, plus `accessType` — WRITE
    or OWNER is required before anything can be saved — and `revision`, the
    committed revision a package has to match.

    A name that finds nothing while `problem.create` then refuses it as taken
    means a deleted or unlisted problem holds the name: re-run with
    `show_deleted=true`.
    """
    try:
        result = await api.call(
            "problems.list",
            {
                "showDeleted": show_deleted,
                "id": problem_id,
                "name": name or None,
                "owner": owner or None,
            },
        )
        return {"ok": True, "count": len(result or []), "problems": result or []}
    except Exception as error:
        return _fail(error, "problems.list")


@mcp.tool()
async def polygon_problem_create(name: str) -> dict[str, Any]:
    """Create a new empty problem and return it. → `problem.create`

    The returned `id` is the `problem_id` every other tool takes. The problem
    starts empty: no statement, no checker, no tests. There is no address in
    the result — Polygon's web URLs are session-scoped and cannot be built from
    the id.
    """
    try:
        return {"ok": True, "problem": await api.call("problem.create", {"name": name})}
    except Exception as error:
        return _fail(error, "problem.create")


@mcp.tool()
async def polygon_problem_info(problem_id: int) -> dict[str, Any]:
    """Read a problem's input/output files and limits. → `problem.info`

    Returns `inputFile`, `outputFile`, `interactive`, `wellFormed`,
    `timeLimit` (milliseconds) and `memoryLimit` (MB). An empty `inputFile` is
    stdin.
    """
    try:
        return {
            "ok": True,
            "info": await api.call("problem.info", {"problemId": problem_id}),
        }
    except Exception as error:
        return _fail(error, "problem.info")


@mcp.tool()
async def polygon_problem_update_info(
    problem_id: int,
    input_file: str | None = None,
    output_file: str | None = None,
    interactive: bool | None = None,
    well_formed: bool | None = None,
    time_limit_ms: int | None = None,
    memory_limit_mb: int | None = None,
) -> dict[str, Any]:
    """Set a problem's input/output files and limits. → `problem.updateInfo`

    Every field is optional and an omitted one is left alone, so this is safe
    to call to change a single limit. `time_limit_ms` is milliseconds and
    `memory_limit_mb` is megabytes, matching Polygon's own units — a 2-second,
    256 MB problem is `time_limit_ms=2000, memory_limit_mb=256`.

    **Never pass the word `"stdin"`.** Polygon stores the literal name of a
    file, so that creates a problem reading a file *called* `stdin`. Pass a real
    name for file I/O (`input_file="TASK.INP"`); a stdin/stdout problem wants
    both arguments left off, which a new problem already is.

    `None` — the default — leaves the stored value alone, while `""` is sent as
    an empty parameter. Polygon documents `inputFile` as 1–64 characters, so an
    empty one may be refused.

    Polygon's other bounds: `time_limit_ms` is 250–15000 and must be divisible
    by 50; `memory_limit_mb` is 4–1024. The effective input and output names
    must differ ignoring case, or the call comes back FAILED.
    """
    try:
        await api.call(
            "problem.updateInfo",
            {
                "problemId": problem_id,
                "inputFile": input_file,
                "outputFile": output_file,
                "interactive": interactive,
                "wellFormed": well_formed,
                "timeLimit": time_limit_ms,
                "memoryLimit": memory_limit_mb,
            },
        )
        return {"ok": True, "updated": True}
    except Exception as error:
        return _fail(error, "problem.updateInfo")


# -------------------------------------------------------------------- access


@mcp.tool()
async def polygon_accesses(problem_id: int) -> dict[str, Any]:
    """List who has direct access to the problem. → `problem.accesses`

    Each entry is a `login` and an `accessType` of READ, WRITE or OWNER. These
    are the *stored direct* entries, not anyone's effective access: a login
    beginning with `@` is a user group, and its members are not expanded. The
    access list lives at problem level, so it is not part of the working copy
    and needs no commit.

    Reading it needs WRITE or OWNER access on the problem, which access
    inherited through a group satisfies — so this succeeding does not prove
    `polygon_set_access` will.
    """
    try:
        result = await api.call("problem.accesses", {"problemId": problem_id})
        return {"ok": True, "count": len(result or []), "accesses": result or []}
    except Exception as error:
        return _fail(error, "problem.accesses")


@mcp.tool()
async def polygon_set_access(
    problem_id: int, login: str, access: str
) -> dict[str, Any]:
    """Grant or remove one user's direct access. → `problem.setAccess`

    This is how a finished problem is handed over — `polygon_set_access(id,
    "codeforces", "READ")` is what makes it importable into a Codeforces
    contest. `access` is READ, WRITE or NONE; NONE removes the direct entry and
    leaves any access the user has through a group intact. OWNER is not on the
    list because Polygon does not let this method assign ownership, and a
    direct owner can be neither downgraded nor removed.

    Takes effect immediately — no commit — and `login` must be a real user, not
    a `@group`. Setting the access a user already has is a successful no-op: no
    email, no rate-limit consumption. Calling it needs *direct* WRITE or OWNER
    access; access held only through a group is not enough, and that is the one
    failure worth predicting here. Rate limit: 60 real changes per minute.
    """
    try:
        await api.call(
            "problem.setAccess",
            {
                "problemId": problem_id,
                "login": login,
                "accessType": _one_of("access", access, ACCESS_TYPES),
            },
        )
        return {"ok": True, "login": login, "access": access}
    except Exception as error:
        return _fail(error, "problem.setAccess")


# ---------------------------------------------------------------- statements


@mcp.tool()
async def polygon_statements(problem_id: int) -> dict[str, Any]:
    """Read every language's statement for a problem. → `problem.statements`

    Returns a map from language code to the statement's `name`, `legend`,
    `input`, `output`, `scoring`, `interaction`, `notes` and `tutorial`. A
    language slot cannot be deleted through the API, so this is how a stale one
    left by an earlier upload is found; only the owner can remove it, in the
    web UI.
    """
    try:
        result = await api.call("problem.statements", {"problemId": problem_id})
        return {
            "ok": True,
            "languages": sorted((result or {}).keys()),
            "statements": result or {},
        }
    except Exception as error:
        return _fail(error, "problem.statements")


@mcp.tool()
async def polygon_save_statement(
    problem_id: int,
    lang: str,
    encoding: str | None = None,
    name: str | None = None,
    legend: str | None = None,
    input: str | None = None,
    output: str | None = None,
    scoring: str | None = None,
    interaction: str | None = None,
    notes: str | None = None,
    tutorial: str | None = None,
) -> dict[str, Any]:
    """Create or update one language's statement. → `problem.saveStatement`

    Only `lang` is required (`english`, `vietnamese`, …).

    **An omitted field keeps its old value; an empty string clears it.** The
    two are different, and on a re-upload the difference is the whole point:
    Polygon has no delete-statement method, so a field this package does not
    fill would otherwise still hold the *previous* problem's prose. Pass `""`
    for every section the package does not use, and `None` — the default — only
    when you mean to leave what is there.

    `interaction` is the one field to leave alone rather than clear: Polygon
    accepts it only for a problem already marked interactive, so sending even an
    empty one at a normal problem risks failing the whole save.

    The text is Polygon's own markup. `name` must be a single line: a value
    containing CR, LF, U+2028 or U+2029 is rejected rather than stored.
    """
    try:
        await api.call(
            "problem.saveStatement",
            {
                "problemId": problem_id,
                "lang": lang,
                "encoding": encoding,
                "name": name,
                "legend": legend,
                "input": input,
                "output": output,
                "scoring": scoring,
                "interaction": interaction,
                "notes": notes,
                "tutorial": tutorial,
            },
        )
        return {"ok": True, "saved": True, "lang": lang}
    except Exception as error:
        return _fail(error, "problem.saveStatement")


# --------------------------------------------------------- files and binding


@mcp.tool()
async def polygon_files(problem_id: int) -> dict[str, Any]:
    """List a problem's resource, source and aux files. → `problem.files`

    Returns `resourceFiles`, `sourceFiles` and `auxFiles`. Sources are what
    `polygon_set_checker` and `polygon_set_validator` choose from. There is no
    delete-file method, so this is also how a leftover from a previous upload
    is found — removing one needs the web UI.
    """
    try:
        return {
            "ok": True,
            "files": await api.call("problem.files", {"problemId": problem_id}),
        }
    except Exception as error:
        return _fail(error, "problem.files")


@mcp.tool()
async def polygon_save_file(
    problem_id: int,
    file_type: str,
    name: str,
    content: str = "",
    path: str = "",
    source_type: str = "",
) -> dict[str, Any]:
    """Add or replace one resource, source or aux file. → `problem.saveFile`

    `file_type` is the API's `type`: `source` for a checker, validator or
    generator; `resource` for something a compile needs beside a source, such
    as a shared header; `aux` for anything else. Pass the text in `content`, or
    `path` to a UTF-8 file inside the configured root — absolute, or relative
    to that root rather than to your own shell's working directory. `source_type` is
    Polygon's compiler id (`cpp.g++17`, `cpp.gcc11-64`, …) and can be left
    empty to let Polygon guess from the extension.

    This **overwrites by name**, which is what a re-upload wants. Polygon
    compiles a source at save time from that single file, so it must be
    self-contained: `testlib.h` is the only include Polygon resolves.
    """
    try:
        await api.call(
            "problem.saveFile",
            {
                "problemId": problem_id,
                "type": _one_of("file_type", file_type, FILE_TYPES),
                "name": name,
                "file": _read_source(content, path),
                "sourceType": source_type or None,
            },
        )
        return {"ok": True, "saved": True, "name": name, "type": file_type}
    except Exception as error:
        return _fail(error, "problem.saveFile")


@mcp.tool()
async def polygon_set_validator(problem_id: int, name: str) -> dict[str, Any]:
    """Point the problem at one of its source files as the validator.
    → `problem.setValidator`

    `name` must already be a source file — upload it with
    `polygon_save_file(file_type="source")` first.
    """
    try:
        await api.call(
            "problem.setValidator", {"problemId": problem_id, "validator": name}
        )
        return {"ok": True, "validator": name}
    except Exception as error:
        return _fail(error, "problem.setValidator")


@mcp.tool()
async def polygon_set_checker(problem_id: int, name: str) -> dict[str, Any]:
    """Point the problem at a checker. → `problem.setChecker`

    `name` is either one of the problem's own source files or one of Polygon's
    standard checkers, spelled the way Polygon spells them: `std::wcmp.cpp`
    (sequences of tokens), `std::ncmp.cpp` (sequences of int64),
    `std::rcmp6.cpp` (doubles to 1e-6), `std::fcmp.cpp` (files as lines),
    `std::lcmp.cpp` (lines as token sequences). The name is passed through
    untouched, so a checker Polygon adds later works without a change here, and
    a `std::` token needs no file uploaded.
    """
    try:
        await api.call("problem.setChecker", {"problemId": problem_id, "checker": name})
        return {"ok": True, "checker": name}
    except Exception as error:
        return _fail(error, "problem.setChecker")


# ----------------------------------------------------------------- solutions


@mcp.tool()
async def polygon_solutions(problem_id: int) -> dict[str, Any]:
    """List the problem's solutions and their tags. → `problem.solutions`

    Each entry has `name`, `sourceType`, `length` and `tag`. Exactly one
    solution should carry `MA`. There is no delete-solution method, so this is
    how a leftover from a previous upload is found — a verified build *runs*
    it, so a stale one still tagged `OK` fails the package.
    """
    try:
        result = await api.call("problem.solutions", {"problemId": problem_id})
        return {"ok": True, "count": len(result or []), "solutions": result or []}
    except Exception as error:
        return _fail(error, "problem.solutions")


@mcp.tool()
async def polygon_save_solution(
    problem_id: int,
    name: str,
    tag: str,
    content: str = "",
    path: str = "",
    source_type: str = "",
) -> dict[str, Any]:
    """Add or replace a solution and set its expected verdict. → `problem.saveSolution`

    `tag` is what the solution is *supposed* to do, and Polygon checks it when
    a package is built with verification: `MA` the main solution, `OK` another
    correct one, `RJ` rejected on some test (any non-accepted verdict), `WA`,
    `PE`, `TL`, `ML`, `RE`, `TO` (time limit or accepted), `TM` (time limit or
    memory limit), `NR` do not run. Pass the source in `content`, or `path` to
    a file inside the configured root — absolute, or relative to that root
    rather than to your own shell's working directory.

    Called with a `name` and a `tag` and no source, this re-tags a solution
    that is already uploaded — which is the one way to neutralise a leftover
    the API cannot delete: `tag="NR"` stops a verified build from running it.
    """
    try:
        await api.call(
            "problem.saveSolution",
            {
                "problemId": problem_id,
                "name": name,
                # Optional: re-tagging a solution that is already uploaded is
                # a save with a name and a tag and nothing else.
                "file": _read_source(content, path, what="source", required=False),
                "tag": _one_of("tag", tag, SOLUTION_TAGS),
                "sourceType": source_type or None,
            },
        )
        return {"ok": True, "saved": True, "name": name, "tag": tag}
    except Exception as error:
        return _fail(error, "problem.saveSolution")


# --------------------------------------------------------------------- tests


@mcp.tool()
async def polygon_script(problem_id: int, testset: str = "tests") -> dict[str, Any]:
    """Read the generator script for a testset. → `problem.script`

    Returns the script as plain text: one generator invocation per line, each
    ending in `> $` or in an explicit test index.
    """
    try:
        return {
            "ok": True,
            "testset": testset,
            "script": await api.call(
                "problem.script", {"problemId": problem_id, "testset": testset}
            ),
        }
    except Exception as error:
        return _fail(error, "problem.script")


@mcp.tool()
async def polygon_save_script(
    problem_id: int, testset: str, source: str
) -> dict[str, Any]:
    """Replace a testset's generator script. → `problem.saveScript`

    The whole script at once, not a line appended: whatever was there is gone.
    Each line is a generator call ending in `> $` or in an explicit index, e.g.
    `gentest --seed 7 > 3`. Polygon's parser rejects `#` comment lines with
    `source: Line should end with > testIndex`.
    """
    try:
        await api.call(
            "problem.saveScript",
            {"problemId": problem_id, "testset": testset, "source": source},
        )
        return {"ok": True, "saved": True, "testset": testset}
    except Exception as error:
        return _fail(error, "problem.saveScript")


@mcp.tool()
async def polygon_clear_script(
    problem_id: int, testset: str = "tests"
) -> dict[str, Any]:
    """Empty a testset's generator script. → `problem.clearScript`

    The eraser `polygon_save_script` is not: it removes the script *and* the
    tests the script owned. This has to run **before** `polygon_delete_tests`
    on a re-upload, because a script-generated test cannot be deleted
    individually — `problem.deleteTest` refuses it with reason
    `FREEMARKER_SCRIPT_TEST`.
    """
    try:
        await api.call(
            "problem.clearScript", {"problemId": problem_id, "testset": testset}
        )
        return {"ok": True, "cleared": True, "testset": testset}
    except Exception as error:
        return _fail(error, "problem.clearScript")


@mcp.tool()
async def polygon_tests(
    problem_id: int, testset: str = "tests", no_inputs: bool = False
) -> dict[str, Any]:
    """List a testset's tests. → `problem.tests`

    Each test has `index`, `manual`, `group`, `points`, `useInStatements` and
    either `input` (manual tests) or `scriptLine` (generated ones), plus
    `inputForStatement` / `outputForStatement` — note the **singular**, which
    is what this method returns; `problem.saveTest` takes the plural
    `testInputForStatements` / `testOutputForStatements`. Write plural, read
    singular.

    Set `no_inputs` to leave the inputs out, which matters for a testset whose
    manual tests are large and is how a points layout is confirmed cheaply.
    """
    try:
        result = await api.call(
            "problem.tests",
            {
                "problemId": problem_id,
                "testset": testset,
                "noInputs": no_inputs or None,
            },
        )
        return {
            "ok": True,
            "testset": testset,
            "count": len(result or []),
            "tests": result or [],
        }
    except Exception as error:
        return _fail(error, "problem.tests")


@mcp.tool()
async def polygon_save_test(
    problem_id: int,
    testset: str,
    test_index: int,
    test_input: str = "",
    path: str = "",
    test_points: float | None = None,
    test_description: str = "",
    use_in_statements: bool | None = None,
    input_for_statements: str = "",
    output_for_statements: str = "",
    verify_for_statements: bool | None = None,
) -> dict[str, Any]:
    """Add or replace one manual test. → `problem.saveTest`

    This is for hand-written tests — samples; generated ones come from the
    script (`polygon_save_script`). Pass the input in `test_input`, or `path`
    to a file inside the configured root. When editing an existing test,
    everything but `testset` and `test_index` is optional and an omitted field
    is left alone.

    **There is no expected-answer parameter, and that is not an omission.**
    Polygon stores no answer for a test: the answer is whatever the `MA`
    solution prints, which is why exactly one solution carries that tag. The
    only answer text that goes up is `output_for_statements`, the sample answer
    *displayed* in the statement, and `verify_for_statements=true` makes
    Polygon check it against the main solution at build time — so a stale
    sample answer fails the build instead of quietly publishing a wrong
    example.

    A sample needs `use_in_statements`, `input_for_statements`,
    `output_for_statements` and `verify_for_statements` set **together**: the
    flag on its own leaves the statement's Examples block empty.

    `test_points` needs points enabled for the problem
    (`polygon_enable_points`) and is at least 0, below 1e9, with at most two
    decimal places. For points on many tests at once, use
    `polygon_save_test_points`.

    There is no `test_group` parameter: this server does not do groups.
    """
    try:
        await api.call(
            "problem.saveTest",
            {
                "problemId": problem_id,
                "testset": testset,
                "testIndex": test_index,
                "testInput": _read_source(
                    test_input, path, what="test_input", required=False
                ),
                "testPoints": test_points,
                "testDescription": test_description or None,
                "testUseInStatements": use_in_statements,
                "testInputForStatements": input_for_statements or None,
                "testOutputForStatements": output_for_statements or None,
                "verifyInputOutputForStatements": verify_for_statements,
            },
        )
        return {"ok": True, "saved": True, "testset": testset, "test_index": test_index}
    except Exception as error:
        return _fail(error, "problem.saveTest")


@mcp.tool()
async def polygon_delete_tests(
    problem_id: int, testset: str, test_indices: list[int]
) -> dict[str, Any]:
    """Delete one or more tests from a testset. → `problem.deleteTest`

    One call for the whole list: the indices go over the wire as the API's
    comma-separated `testIndices`.

    **All-or-nothing on the pre-check.** Polygon checks every test before
    deleting any, so a refusal leaves the testset exactly as it was and comes
    back with `details.failures`, each naming an `index`, a `reason` of
    `DUPLICATE`, `NOT_FOUND`, `FREEMARKER_SCRIPT_TEST` or `DELETE_FAILED`, and
    a `message`. Read the reasons rather than retrying blind:
    `FREEMARKER_SCRIPT_TEST` means the script has not been cleared, `NOT_FOUND`
    means the index list is stale. `DELETE_FAILED` is the one partial case —
    its comment says the problem state may be partially modified — so re-read
    `polygon_tests` before retrying that one.
    """
    if not test_indices:
        return {"ok": True, "deleted": 0, "testset": testset, "test_indices": []}
    try:
        await api.call(
            "problem.deleteTest",
            {
                "problemId": problem_id,
                "testset": testset,
                "testIndices": ",".join(str(index) for index in test_indices),
            },
        )
        return {
            "ok": True,
            "deleted": len(test_indices),
            "testset": testset,
            "test_indices": list(test_indices),
        }
    except Exception as error:
        return _fail(error, "problem.deleteTest")


@mcp.tool()
async def polygon_enable_points(problem_id: int, enable: bool) -> dict[str, Any]:
    """Turn per-test points on or off for the problem. → `problem.enablePoints`

    Needed before a test can carry points — that is, for any scored problem.
    Problem-wide: it takes no `testset`. Setting a flag that is already set is
    harmless, which matters on a re-upload.
    """
    try:
        await api.call(
            "problem.enablePoints", {"problemId": problem_id, "enable": enable}
        )
        return {"ok": True, "points_enabled": enable}
    except Exception as error:
        return _fail(error, "problem.enablePoints")


@mcp.tool()
async def polygon_save_test_points(
    problem_id: int, testset: str, points: dict[str, float]
) -> dict[str, Any]:
    """Set the points of many tests in one call. → N × `problem.saveTest`

    `points` maps a test index, as a string, to its points:
    `{"3": 1.0, "4": 1.0, "5": 1.5}`. Each entry becomes a `problem.saveTest`
    carrying `testPoints` and nothing else, which is a pure points update — the
    input and the script line of a generated test are left alone, because every
    unset parameter is omitted from the request.

    This exists because the alternative is a hundred separate tool calls for a
    hundred-test problem. The requests are paced by the same floor as every
    other call here, so budget roughly half a second each.

    It stops at the first refusal rather than pressing on, and reports how many
    had already been applied so the run can resume from there. `total_points`
    comes back so the caller can check the ladder sums to what it meant to.
    """
    applied: list[int] = []
    try:
        ordered = sorted(((int(k), v) for k, v in points.items()), key=lambda kv: kv[0])
    except (TypeError, ValueError):
        return _fail(
            PolygonError("Every key of `points` must be a test index."),
            "problem.saveTest",
        )
    for index, value in ordered:
        try:
            await api.call(
                "problem.saveTest",
                {
                    "problemId": problem_id,
                    "testset": testset,
                    "testIndex": index,
                    "testPoints": value,
                },
            )
        except Exception as error:
            return {
                **_fail(error, "problem.saveTest"),
                "applied": applied,
                "applied_count": len(applied),
                "failed_index": index,
            }
        applied.append(index)
    return {
        "ok": True,
        "testset": testset,
        "applied_count": len(applied),
        "applied": applied,
        "total_points": round(sum(value for _, value in ordered), 2),
    }


# ------------------------------------------------ readiness, commit, package


@mcp.tool()
async def polygon_cautions(problem_id: int) -> dict[str, Any]:
    """Read the working copy's cautions and package-readiness issues.
    → `problem.cautions`

    Returns the four caution arrays `common`, `statement`, `structure` and
    `issues` — each a `type`, `severity`, `category`, `message` and
    `parameters` — plus `packageReadinessIssues` (`type`, `reason`, `message`),
    `latestPackageWarnings` and a cached `ai` block.

    One trap in reading it: `NO_CHECKER_TESTS` and `NO_VALIDATOR_TESTS` are
    *suppressed* while the corresponding `NO_CHECKER` / `NO_VALIDATOR` caution
    is present, so one of them disappearing is not the same as it being fixed.
    """
    try:
        result = await api.call("problem.cautions", {"problemId": problem_id})
        cautions = result if isinstance(result, dict) else {}
        return {
            "ok": True,
            "cautions": result,
            "readiness_issue_count": len(cautions.get("packageReadinessIssues") or []),
        }
    except Exception as error:
        return _fail(error, "problem.cautions")


@mcp.tool()
async def polygon_update_working_copy(problem_id: int) -> dict[str, Any]:
    """Pull the latest revision into the working copy. → `problem.updateWorkingCopy`

    Needed when someone else committed while this one was open; a working copy
    behind the repository is what makes `polygon_commit` come back with
    `conflict_occurred`.
    """
    try:
        await api.call("problem.updateWorkingCopy", {"problemId": problem_id})
        return {"ok": True, "problem_id": problem_id}
    except Exception as error:
        return _fail(error, "problem.updateWorkingCopy")


@mcp.tool()
async def polygon_discard_working_copy(problem_id: int) -> dict[str, Any]:
    """Throw away every uncommitted change. → `problem.discardWorkingCopy`

    Destructive and not undoable: everything saved since the last commit is
    gone, and committed content is untouched. This is the way out of a working
    copy that will not commit, and the first step of a re-upload — it stops a
    half-finished earlier attempt from surviving into this one.
    """
    try:
        await api.call("problem.discardWorkingCopy", {"problemId": problem_id})
        return {"ok": True, "problem_id": problem_id}
    except Exception as error:
        return _fail(error, "problem.discardWorkingCopy")


@mcp.tool()
async def polygon_commit(
    problem_id: int, minor_changes: bool = False, message: str = ""
) -> dict[str, Any]:
    """Commit the working copy. → `problem.commitChanges`

    Nothing saved by the tools above is visible to anyone else until this runs.
    `minor_changes=true` suppresses the email notification to the problem's
    other authors.

    Read `committed` rather than `ok`. Polygon answers a no-op with a
    *successful* envelope carrying `committed=false` and the message "No
    changes", and it reports a working copy that fell behind the repository as
    `conflict_occurred=true` — so `ok: true` means the call went through, not
    that a revision was created. On a re-upload, "No changes" means an earlier
    phase silently wrote nothing and is a reason to go back and look. On a
    conflict, `polygon_update_working_copy` and then commit again. A missing
    field is reported false: this tool does not claim a commit Polygon did not
    confirm.
    """
    try:
        result = await api.call(
            "problem.commitChanges",
            {
                "problemId": problem_id,
                "minorChanges": minor_changes,
                "message": message or None,
            },
        )
        commit_result = result if isinstance(result, dict) else {}
        return {
            "ok": True,
            "committed": bool(commit_result.get("committed")),
            "conflict_occurred": bool(commit_result.get("conflictOccurred")),
            "message": str(commit_result.get("message") or ""),
            "commit_result": result,
        }
    except Exception as error:
        return _fail(error, "problem.commitChanges")


@mcp.tool()
async def polygon_build_package(
    problem_id: int, full: bool = True, verify: bool = True
) -> dict[str, Any]:
    """Start building a package. → `problem.buildPackage`

    Returns as soon as the build is queued, not when it finishes — poll
    `polygon_packages`, or use `polygon_build_package_and_wait` instead.

    `verify=true` runs every solution on every test and checks the tags
    actually hold, which is the point of building one at all. `full=true`
    additionally produces the linux and windows packages; those two carry the
    generated tests and `standard` does not, so a `standard`-only build never
    runs the generator or the validator and a `READY` on it is no evidence the
    tests generate.
    """
    try:
        await api.call(
            "problem.buildPackage",
            {"problemId": problem_id, "full": full, "verify": verify},
        )
        return {"ok": True, "build_started": True, "full": full, "verify": verify}
    except Exception as error:
        return _fail(error, "problem.buildPackage")


@mcp.tool()
async def polygon_packages(problem_id: int) -> dict[str, Any]:
    """List the problem's packages. → `problem.packages`

    Each has `id`, `revision`, `creationTimeSeconds`, `state`
    (PENDING/RUNNING/READY/FAILED), `comment` and `type` (standard/linux/
    windows). A finished upload needs a full, verified package in state `READY`
    for the *committed* revision, so match on `revision` rather than taking the
    newest row.
    """
    try:
        result = await api.call("problem.packages", {"problemId": problem_id})
        return {"ok": True, "count": len(result or []), "packages": result or []}
    except Exception as error:
        return _fail(error, "problem.packages")


async def _current_revision(problem_id: int) -> int | None:
    """The problem's committed revision, or None if it cannot be read."""
    result = await api.call("problems.list", {"id": problem_id}) or []
    for problem in result:
        if problem.get("id") == problem_id:
            revision = problem.get("revision")
            return int(revision) if revision is not None else None
    return None


@mcp.tool()
async def polygon_build_package_and_wait(
    problem_id: int,
    full: bool = True,
    verify: bool = True,
    timeout_seconds: int = 1800,
    poll_interval_seconds: int = 10,
) -> dict[str, Any]:
    """Build a package and wait for it to finish.
    → `problem.buildPackage`, then `problem.packages` until it settles

    The HTTP API has no waiting variant, so this is the two calls the caller
    would otherwise make in a loop. It reads the problem's committed revision
    first and follows only the packages built for *that* revision, which is
    what "a READY package for the committed revision" actually means — taking
    the newest row instead would happily accept a package from a previous
    upload.

    It also waits for the build it started to actually *show itself* before
    settling. Polling the instant `buildPackage` returns can catch a revision
    whose previous build already left a full READY set — every row reads READY,
    nothing is pending, and a naive settle declares success for a build that has
    not begun. That matters most where the skill says "after assigning points,
    commit and rebuild so scoring sticks": a stale pass there reports a package
    that predates the points.

    So the answer carries `built_new` and `new_packages` — the packages this
    call produced, as opposed to the ones already there. When Polygon reuses an
    existing package instead of building one, `built_new` is false and the call
    returns anyway rather than hanging until the timeout; read it before
    treating `all_ready` as evidence of your latest fix.

    Comes back with `states`, a map of package type to its state, and
    `all_ready`, true only when every package of this revision reached `READY`.
    With `full=true` expect `standard`, `linux` and `windows`; check that
    `linux` and `windows` are among them, because those are the two that
    materialise the generated tests. A `FAILED` package's `comment` says why —
    read it rather than rebuilding over the top.
    """
    try:
        revision = await _current_revision(problem_id)
        before = {
            package.get("id")
            for package in (await api.call("problem.packages", {"problemId": problem_id}) or [])
        }
        await api.call(
            "problem.buildPackage",
            {"problemId": problem_id, "full": full, "verify": verify},
        )
    except Exception as error:
        return _fail(error, "problem.buildPackage")

    deadline = time.monotonic() + timeout_seconds
    # How many settled-looking polls to sit through before accepting that the
    # build produced nothing new. Bounded, because a reuse is a state that will
    # never change and waiting out the full timeout on it helps nobody.
    grace_polls = 3
    saw_activity = False
    ours: list[dict[str, Any]] = []
    new: list[dict[str, Any]] = []
    while True:
        # Before the first poll too: Polygon needs a moment to register the
        # build, and a poll that beats it sees only what was already there.
        await asyncio.sleep(poll_interval_seconds)
        try:
            packages = await api.call("problem.packages", {"problemId": problem_id}) or []
        except Exception as error:
            return _fail(error, "problem.packages")

        if revision is None:
            ours = [p for p in packages if p.get("id") not in before]
        else:
            ours = [p for p in packages if p.get("revision") == revision]
        new = [p for p in ours if p.get("id") not in before]

        busy = any(p.get("state") in PENDING_STATES for p in ours)
        if busy or new:
            saw_activity = True

        if ours and not busy:
            if saw_activity or grace_polls <= 0:
                break
            grace_polls -= 1
        if time.monotonic() >= deadline:
            return {
                "ok": False,
                "error": f"The build did not settle within {timeout_seconds}s.",
                "method": "problem.packages",
                "timed_out": True,
                "revision": revision,
                "built_new": bool(new),
                "new_packages": new,
                "packages": ours,
            }

    states = {str(p.get("type")): str(p.get("state")) for p in ours}
    return {
        "ok": True,
        "revision": revision,
        "full": full,
        "verify": verify,
        "states": states,
        "all_ready": all(state == "READY" for state in states.values()),
        "built_new": bool(new),
        "new_packages": new,
        "packages": ours,
    }


def run() -> None:
    mcp.run()
