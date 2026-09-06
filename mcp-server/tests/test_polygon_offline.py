"""Exercise the Polygon server against a fake Polygon, without network access.

Everything here runs through `httpx.MockTransport`, so the whole client — the
signature, the read/write verb split, the pacing, the one retry — is the real
code, and only the socket is fake.

The harness is adapted from the `competitive-programming` plugin by LamTer.
The cases are this package's own, and they concentrate on the five places it
diverges from that original, because those are the ones no upstream test
covers: an empty string reaching the wire instead of being dropped, CRLF
surviving a `path=` read, the root defaulting to the working directory, the
batch test delete, and the build-and-wait matching on revision.
"""

from __future__ import annotations

import hashlib
from dataclasses import dataclass
from pathlib import Path
from typing import Any
from urllib.parse import parse_qsl

import httpx
import pytest

from polygon_mcp import server
from polygon_mcp.api import PolygonApi, WRITE_METHODS, sign
from polygon_mcp.config import (
    Config,
    PolygonError,
    read_text_verbatim,
    resolve_local_path,
)

API_KEY = "polygon-key-11112222"
API_SECRET = "polygon-secret-33334444"


# ------------------------------------------------------------------ the fake


@dataclass
class Call:
    method: str
    params: dict[str, str]
    verb: str


def ok(result: Any) -> tuple[int, Any]:
    return 200, {"status": "OK", "result": result}


def failed(comment: str, result: Any = None) -> tuple[int, Any]:
    payload: dict[str, Any] = {"status": "FAILED", "comment": comment}
    if result is not None:
        payload["result"] = result
    return 400, payload


class FakePolygon:
    """Replays canned replies and records what was sent.

    Replies are consumed in order and the last one repeats, so a poll test
    reads `FakePolygon(a, b, c)` as "a, then b, then c for ever".
    """

    def __init__(self, *replies: tuple[int, Any]):
        self.replies = list(replies) or [ok(None)]
        self.calls: list[Call] = []

    def __call__(self, request: httpx.Request) -> httpx.Response:
        if request.method == "GET":
            params = dict(request.url.params)
        else:
            params = dict(parse_qsl(request.content.decode(), keep_blank_values=True))
        self.calls.append(
            Call(
                method=request.url.path.rsplit("/", 1)[-1],
                params=params,
                verb=request.method,
            )
        )
        status, payload = (
            self.replies.pop(0) if len(self.replies) > 1 else self.replies[0]
        )
        if isinstance(payload, (dict, list)):
            return httpx.Response(status, json=payload)
        return httpx.Response(status, text=str(payload))

    @property
    def last(self) -> Call:
        return self.calls[-1]

    def of(self, method: str) -> list[Call]:
        return [call for call in self.calls if call.method == method]


@pytest.fixture
def polygon(monkeypatch):
    """Wire one fake Polygon into the server module for a single test."""

    def install(*replies, **overrides) -> FakePolygon:
        fake = FakePolygon(*replies)
        overrides.setdefault("min_interval", 0.0)
        overrides.setdefault("root", Path.cwd())
        overrides.setdefault("api_key", API_KEY)
        overrides.setdefault("api_secret", API_SECRET)
        overrides.setdefault("base_url", "https://polygon.test/api/")
        config = Config(**overrides)

        async def no_sleep(seconds: float) -> None:
            return None

        api = PolygonApi(
            config, transport=httpx.MockTransport(fake), sleep=no_sleep
        )
        monkeypatch.setattr(server, "config", config)
        monkeypatch.setattr(server, "api", api)
        monkeypatch.setattr(server.asyncio, "sleep", no_sleep)
        return fake

    return install


# -------------------------------------------------------------------- signing


def test_signature_matches_an_independently_computed_hash():
    # The documented string: <rand>/<method>?k=v&…#<secret>, every parameter
    # including apiKey and time, sorted by name then value.
    signed = sign(
        "problem.saveTest",
        {"problemId": 42, "testset": "tests", "testIndex": 1},
        api_key="KEY",
        secret="SEC",
        rand="abcdef",
        now=1000,
    )
    query = "apiKey=KEY&problemId=42&testIndex=1&testset=tests&time=1000"
    expected = hashlib.sha512(f"abcdef/problem.saveTest?{query}#SEC".encode()).hexdigest()
    assert signed["apiSig"] == "abcdef" + expected


def test_signature_is_computed_over_raw_values_not_encoded_ones():
    """The trap that only bites on a value needing escaping.

    `std::wcmp.cpp` is the one every upload hits: sign the percent-encoded
    form and Polygon — which decodes before it verifies — answers
    `apiKey: Incorrect signature`.
    """
    signed = sign(
        "problem.setChecker",
        {"problemId": 42, "checker": "std::wcmp.cpp"},
        api_key="KEY",
        secret="SEC",
        rand="abcdef",
        now=1000,
    )
    query = "apiKey=KEY&checker=std::wcmp.cpp&problemId=42&time=1000"
    expected = hashlib.sha512(f"abcdef/problem.setChecker?{query}#SEC".encode()).hexdigest()
    assert signed["apiSig"] == "abcdef" + expected


def test_the_verb_split_covers_the_new_methods():
    # clearScript changes something and has to be POSTed like every other
    # write; cautions only reads, so it stays a GET.
    assert "problem.saveStatement" in WRITE_METHODS
    assert "problem.clearScript" in WRITE_METHODS
    assert "problem.cautions" not in WRITE_METHODS


async def test_read_verb_is_get(polygon):
    fake = polygon(ok({"timeLimit": 2000}))
    await server.polygon_problem_info(42)
    assert fake.last.verb == "GET"


async def test_write_verb_is_post(polygon):
    fake = polygon(ok(None))
    await server.polygon_clear_script(42, "tests")
    assert fake.last.verb == "POST"
    assert fake.last.method == "problem.clearScript"


# ------------------------------- an empty string is a value, not an omission


async def test_save_statement_sends_an_empty_field_rather_than_dropping_it(polygon):
    """The whole point of the `str | None` signatures.

    Polygon has no delete-statement method, so a re-upload blanks the previous
    problem's prose by storing `""`. Collapsing `""` to "leave alone" — which
    is what `value or None` does — would silently leave the old text standing.
    """
    fake = polygon(ok(None))
    result = await server.polygon_save_statement(
        42, lang="english", legend="Đề bài", notes="", tutorial=""
    )
    assert result["ok"] is True
    sent = fake.last.params
    assert sent["legend"] == "Đề bài"
    assert sent["notes"] == ""
    assert sent["tutorial"] == ""


async def test_save_statement_omits_a_field_left_as_none(polygon):
    fake = polygon(ok(None))
    await server.polygon_save_statement(42, lang="english", legend="body")
    assert "notes" not in fake.last.params
    assert "scoring" not in fake.last.params


async def test_update_info_distinguishes_an_empty_value_from_leave_alone(polygon):
    """`""` is transmitted; `None` is omitted.

    The same mechanism `polygon_save_statement` depends on, pinned on the other
    tool that has it. `value or None` would collapse the two, and the whole
    point of these signatures is that a caller can tell Polygon "store an empty
    value" and not merely "leave this alone".
    """
    fake = polygon(ok(None))
    await server.polygon_problem_update_info(42, input_file="", output_file="")
    assert fake.last.params["inputFile"] == ""
    assert fake.last.params["outputFile"] == ""

    fake = polygon(ok(None))
    await server.polygon_problem_update_info(42, time_limit_ms=2000)
    assert "inputFile" not in fake.last.params
    assert fake.last.params["timeLimit"] == "2000"


# --------------------------------------------------------------- line endings


def test_read_text_verbatim_keeps_crlf(tmp_path: Path):
    """Text mode would silently rewrite these to LF.

    The skill spends a phase on line endings because Polygon builds tests under
    a Windows toolchain whose strict testlib `eoln()` wants CRLF; a server that
    normalises on the way up would undo that decision invisibly.
    """
    path = tmp_path / "test_1.inp"
    path.write_bytes(b"3\r\n1 2 3\r\n")
    assert read_text_verbatim(path) == "3\r\n1 2 3\r\n"


def test_read_text_verbatim_keeps_lf(tmp_path: Path):
    path = tmp_path / "gentest.cpp"
    path.write_bytes(b"int main() {\n  return 0;\n}\n")
    assert read_text_verbatim(path) == "int main() {\n  return 0;\n}\n"


def test_read_text_verbatim_refuses_non_utf8(tmp_path: Path):
    path = tmp_path / "blob.bin"
    path.write_bytes(b"\xff\xfe\x00")
    with pytest.raises(PolygonError, match="not UTF-8"):
        read_text_verbatim(path)


async def test_save_file_uploads_crlf_untouched(polygon, tmp_path: Path):
    source = tmp_path / "validator.cpp"
    source.write_bytes(b'#include "testlib.h"\r\nint main() {}\r\n')
    fake = polygon(ok(None), root=tmp_path)
    result = await server.polygon_save_file(
        42, file_type="source", name="validator.cpp", path=str(source)
    )
    assert result["ok"] is True
    assert fake.last.params["file"] == '#include "testlib.h"\r\nint main() {}\r\n'


# ---------------------------------------------------------------- the root


def test_root_defaults_to_the_working_directory(monkeypatch, tmp_path: Path):
    monkeypatch.chdir(tmp_path)
    monkeypatch.delenv("POLYGON_MCP_ROOT", raising=False)
    monkeypatch.setenv("POLYGON_API_KEY", "k")
    monkeypatch.setenv("POLYGON_API_SECRET", "s")
    assert Config.from_env().root.resolve() == tmp_path.resolve()


def test_root_is_overridden_by_the_environment(monkeypatch, tmp_path: Path):
    monkeypatch.setenv("POLYGON_MCP_ROOT", str(tmp_path / "problems"))
    assert Config.from_env().root == tmp_path / "problems"


def test_an_unexpanded_placeholder_counts_as_unset(monkeypatch, tmp_path: Path):
    """`.mcp.json` substitutes `${VAR}`; an unset VAR arrives as the literal.

    Left alone, a root of "${POLYGON_MCP_ROOT}" refuses every path read while
    complaining about a directory nobody named, and a key of
    "${POLYGON_API_KEY}" reaches Polygon and comes back "Incorrect signature"
    instead of "no credentials".
    """
    monkeypatch.chdir(tmp_path)
    monkeypatch.setenv("POLYGON_MCP_ROOT", "${POLYGON_MCP_ROOT}")
    monkeypatch.setenv("POLYGON_API_KEY", "${POLYGON_API_KEY}")
    monkeypatch.setenv("POLYGON_API_SECRET", "${POLYGON_API_SECRET}")
    config = Config.from_env()
    assert config.root.resolve() == tmp_path.resolve()
    assert config.has_credentials is False


def test_a_relative_path_is_anchored_to_the_root_not_the_cwd(
    monkeypatch, tmp_path: Path
):
    """The model's shell can `cd`; this server's working directory cannot.

    Anchoring a relative `path=` to the root is the only reading that stays
    true when the two drift apart.
    """
    root = tmp_path / "problem"
    (root / "outputs").mkdir(parents=True)
    wanted = root / "outputs" / "validator.cpp"
    wanted.write_text("int main(){}", encoding="utf-8")

    elsewhere = tmp_path / "elsewhere"
    elsewhere.mkdir()
    monkeypatch.chdir(elsewhere)

    assert resolve_local_path("outputs/validator.cpp", root) == wanted.resolve()


def test_a_relative_path_still_cannot_escape_the_root(tmp_path: Path):
    root = tmp_path / "problem"
    root.mkdir()
    (tmp_path / "secret.txt").write_text("nope", encoding="utf-8")
    with pytest.raises(PolygonError, match="outside"):
        resolve_local_path("../secret.txt", root)


def test_a_path_outside_the_root_is_refused(tmp_path: Path):
    root = tmp_path / "root"
    root.mkdir()
    outside = tmp_path / "secret.txt"
    outside.write_text("nope", encoding="utf-8")
    with pytest.raises(PolygonError, match="outside"):
        resolve_local_path(str(outside), root)


def test_a_path_inside_the_root_resolves(tmp_path: Path):
    inside = tmp_path / "solution.cpp"
    inside.write_text("int main(){}", encoding="utf-8")
    assert resolve_local_path(str(inside), tmp_path) == inside.resolve()


# -------------------------------------------------------------- batch delete


async def test_delete_tests_joins_the_indices(polygon):
    fake = polygon(ok(None))
    result = await server.polygon_delete_tests(42, "tests", [1, 2, 3, 10])
    assert result["ok"] is True and result["deleted"] == 4
    assert fake.last.params["testIndices"] == "1,2,3,10"


async def test_delete_tests_with_no_indices_calls_nothing(polygon):
    fake = polygon(ok(None))
    result = await server.polygon_delete_tests(42, "tests", [])
    assert result["ok"] is True and result["deleted"] == 0
    assert fake.calls == []


async def test_delete_tests_surfaces_the_refusal_reasons(polygon):
    """All-or-nothing on the pre-check: the reasons are the whole diagnosis."""
    fake = polygon(
        failed(
            "Some tests can not be deleted.",
            {"failures": [{"index": 4, "reason": "FREEMARKER_SCRIPT_TEST"}]},
        )
    )
    result = await server.polygon_delete_tests(42, "tests", [4])
    assert result["ok"] is False
    assert result["method"] == "problem.deleteTest"
    assert result["details"]["failures"][0]["reason"] == "FREEMARKER_SCRIPT_TEST"


# ---------------------------------------------------------------- bulk points


async def test_save_test_points_sends_points_and_nothing_else(polygon):
    """A pure points update must not carry an input that would clobber a
    script-generated test."""
    fake = polygon(ok(None))
    result = await server.polygon_save_test_points(
        42, "tests", {"3": 1.0, "4": 1.0, "5": 1.5}
    )
    assert result["ok"] is True
    assert result["applied"] == [3, 4, 5]
    assert result["total_points"] == 3.5
    assert len(fake.of("problem.saveTest")) == 3
    sent = fake.of("problem.saveTest")[0].params
    assert sent["testIndex"] == "3" and sent["testPoints"] == "1"
    assert "testInput" not in sent and "testGroup" not in sent


async def test_save_test_points_stops_at_the_first_refusal(polygon):
    fake = polygon(ok(None), failed("Points are not enabled."))
    result = await server.polygon_save_test_points(
        42, "tests", {"1": 1.0, "2": 1.0, "3": 1.0}
    )
    assert result["ok"] is False
    assert result["applied"] == [1]
    assert result["failed_index"] == 2
    # It stopped rather than pressing on through the rest.
    assert len(fake.of("problem.saveTest")) == 2


# ------------------------------------------------------------ build and wait


async def test_build_and_wait_follows_the_committed_revision(polygon):
    """A package from an earlier revision must not be mistaken for this one."""
    stale = {"id": 1, "revision": 6, "state": "READY", "type": "standard"}
    running = [
        {"id": 2, "revision": 7, "state": "RUNNING", "type": "standard"},
        {"id": 3, "revision": 7, "state": "PENDING", "type": "linux"},
        {"id": 4, "revision": 7, "state": "PENDING", "type": "windows"},
    ]
    ready = [
        {"id": 2, "revision": 7, "state": "READY", "type": "standard"},
        {"id": 3, "revision": 7, "state": "READY", "type": "linux"},
        {"id": 4, "revision": 7, "state": "READY", "type": "windows"},
    ]
    fake = polygon(
        ok([{"id": 42, "name": "sum", "revision": 7}]),  # problems.list
        ok([stale]),                                     # packages, before
        ok(None),                                        # buildPackage
        ok([stale, *running]),                           # packages, poll 1
        ok([stale, *ready]),                             # packages, poll 2
    )
    result = await server.polygon_build_package_and_wait(42, timeout_seconds=60)
    assert result["ok"] is True
    assert result["revision"] == 7
    assert result["all_ready"] is True
    assert result["states"] == {
        "standard": "READY",
        "linux": "READY",
        "windows": "READY",
    }
    # The revision-6 package never entered the answer.
    assert all(p["revision"] == 7 for p in result["packages"])


async def test_build_and_wait_reports_a_failed_package_rather_than_waiting(polygon):
    fake = polygon(
        ok([{"id": 42, "revision": 3}]),
        ok([]),
        ok(None),
        ok(
            [
                {
                    "id": 9,
                    "revision": 3,
                    "state": "FAILED",
                    "type": "standard",
                    "comment": "PackageException: tle-x.cpp got RE on tests#3",
                }
            ]
        ),
    )
    result = await server.polygon_build_package_and_wait(42, timeout_seconds=60)
    assert result["ok"] is True  # the call worked; the build did not
    assert result["all_ready"] is False
    assert result["states"] == {"standard": "FAILED"}
    assert "RE on tests#3" in result["packages"][0]["comment"]


async def test_build_and_wait_does_not_report_a_stale_package_as_this_build(polygon):
    """The race this tool exists to avoid.

    Revision 7 already has a full READY set from an earlier build. Poll the
    packages the instant `buildPackage` returns — before Polygon has registered
    the new one — and every revision-matched row reads READY, so a naive
    settle-on-no-PENDING declares success for a build that has not started.

    The skill hits this exactly where it hurts: "after assigning points, commit
    and rebuild so scoring sticks". A stale pass there reports a package that
    predates the points.
    """
    stale = [
        {"id": 2, "revision": 7, "state": "READY", "type": "standard"},
        {"id": 3, "revision": 7, "state": "READY", "type": "linux"},
        {"id": 4, "revision": 7, "state": "READY", "type": "windows"},
    ]
    fresh = [
        {"id": 5, "revision": 7, "state": "READY", "type": "standard"},
        {"id": 6, "revision": 7, "state": "READY", "type": "linux"},
        {"id": 7, "revision": 7, "state": "READY", "type": "windows"},
    ]
    fake = polygon(
        ok([{"id": 42, "revision": 7}]),   # problems.list
        ok(stale),                         # packages, before
        ok(None),                          # buildPackage
        ok(stale),                         # packages: the new build is queued but unseen
        ok([*stale, {"id": 5, "revision": 7, "state": "RUNNING", "type": "standard"}]),
        ok([*stale, *fresh]),              # packages: the new build finished
    )
    result = await server.polygon_build_package_and_wait(42, timeout_seconds=60)
    assert result["ok"] is True
    assert result["built_new"] is True, "must follow the build it started"
    # The ids the call itself produced, not the ones that were already there.
    assert {p["id"] for p in result["new_packages"]} == {5, 6, 7}


async def test_build_and_wait_says_so_when_nothing_new_ever_appears(polygon):
    """Polygon reused an existing package rather than building one.

    Returning is right — hanging until the timeout would be worse — but the
    answer has to say that these packages predate the call, so the skill does
    not read them as evidence its latest fix was built.
    """
    stale = [{"id": 2, "revision": 7, "state": "READY", "type": "standard"}]
    fake = polygon(
        ok([{"id": 42, "revision": 7}]),
        ok(stale),
        ok(None),
        ok(stale),
    )
    result = await server.polygon_build_package_and_wait(42, timeout_seconds=60)
    assert result["ok"] is True
    assert result["built_new"] is False
    assert result["new_packages"] == []


async def test_build_and_wait_defaults_to_full_and_verify(polygon):
    fake = polygon(
        ok([{"id": 42, "revision": 1}]),
        ok([]),
        ok(None),
        ok([{"id": 1, "revision": 1, "state": "READY", "type": "standard"}]),
    )
    await server.polygon_build_package_and_wait(42, timeout_seconds=60)
    build = fake.of("problem.buildPackage")[0]
    assert build.params["full"] == "true"
    assert build.params["verify"] == "true"


# ------------------------------------------------- what is deliberately gone


def test_the_group_tools_do_not_exist():
    """The skill scores every test on its own and never enables groups.

    A rule that cannot be violated beats a rule that is written down, so these
    have no wrapper at all — this test is what keeps someone from helpfully
    adding one back.
    """
    for name in (
        "polygon_enable_groups",
        "polygon_set_test_group",
        "polygon_save_test_group",
        "polygon_test_groups",
        "polygon_set_interactor",
    ):
        assert not hasattr(server, name), f"{name} should not exist"


def test_save_test_has_no_group_parameter():
    import inspect

    assert "test_group" not in inspect.signature(server.polygon_save_test).parameters


# ------------------------------------------------------------------ failures


async def test_a_failure_names_the_method_and_keeps_the_comment(polygon):
    polygon(failed("problemId: Problem not found"))
    result = await server.polygon_problem_info(999)
    assert result["ok"] is False
    assert result["method"] == "problem.info"
    assert "Problem not found" in result["error"]


async def test_missing_credentials_fail_before_any_request(polygon, monkeypatch):
    fake = polygon(ok(None), api_key="", api_secret="")
    result = await server.polygon_problem_info(42)
    assert result["ok"] is False
    assert "POLYGON_API_KEY" in result["error"]
    assert fake.calls == []


async def test_whoami_reports_the_root_it_will_read_from(polygon, tmp_path: Path):
    polygon(ok([]), root=tmp_path)
    result = await server.polygon_whoami()
    assert result["ok"] is True
    assert result["path_reads_allowed_under"] == str(tmp_path)


async def test_an_unknown_solution_tag_is_caught_before_the_request(polygon):
    fake = polygon(ok(None))
    result = await server.polygon_save_solution(
        42, name="sol.cpp", tag="ACCEPTED", content="int main(){}"
    )
    assert result["ok"] is False
    assert "must be one of" in result["error"]
    assert fake.calls == []
