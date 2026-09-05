# Bug report — `gentest.cpp` must not force LF; it breaks every Polygon package build

**Date:** 2026-09-05
**Found while:** preparing and uploading `cubesum-dp-digit-rakkoon`, `snake-dp-casework`
and `lineup-minimum-topo-sort` with the `cp-problem-generation` skill pack.
**Affects:** `skills/generating-tests`, `skills/uploading-to-polygon`.
**Severity:** high — the package builds cannot succeed at all, and the error message points
away from the cause.

---

## TL;DR

A generator that forces bare-LF output makes **every** Polygon package build fail:

```
PackageException: Got exception while generating tests: Can't generate input or answer
for test 11 [Request skipped because of previous errors in the requests batch
[Reason: Validator 'validator.exe' returns exit code 3 [FAIL Expected EOLN (stdin, line 1)]]].
```

Polygon generates and validates tests under a **Windows** toolchain. testlib's strict
`eoln()` compiled that way requires **CRLF**. So `_setmode(_fileno(stdout), _O_BINARY)` in
`gentest.cpp` produces exactly the line endings Polygon's own validator rejects.

**Fix:** delete the binary-mode block. Let `cout << '\n'` use the platform's native text
mode. Nothing else changes.

---

## Symptom

Six consecutive package builds failed across two problems. The failure surfaced in two
different phases depending on incidental state:

- during test generation — `Can't generate input or answer for test N [...] Expected EOLN (stdin, line 1)`
- during statement creation — `Can't create english statement: [Unexpected verdict while
  building sample tests: CRASHED, comment: Validator 'validator.exe' returns exit code 3
  [FAIL Expected EOLN (stdin, line 1)]]`

Both are the same underlying cause. The second one only appeared once the first had been
incidentally masked.

## Why this is hard to diagnose

**The reported test index is not the test that failed.** `Can't generate input or answer
for test 11 [Request skipped because of previous errors in the requests batch [Reason: …]]`
means test 11 was *skipped*; the `Reason` belongs to some other request in the same batch.
The index moved (11, then 7, then 6, then 1) across rebuilds of unchanged data, which
invites the conclusion that the named test is meaningful. It is not.

**`(stdin, line 1)` points at the first line of that unidentified test**, which for most
problems is a single integer — so it reads like a malformed first line rather than a
global line-ending mismatch.

**Everything validated locally.** All 100 generated tests were accepted by
`outputs/validator.cpp` on the authoring machine, because the local validator was built
with `-DFOR_LINUX` and fed LF. The package is internally consistent; only Polygon's
toolchain disagrees.

## Root cause

testlib, `InStream::eoln()`, strict mode — the mode all validators run in:

```cpp
} else {   // strict
    bool returnCr = false;
#if (defined(ON_WINDOWS) && !defined(FOR_LINUX)) || defined(FOR_WINDOWS)
    if (c != CR) { reader->unreadChar(c); return false; }   // CR REQUIRED
    else { returnCr = true; c = reader->nextChar(); }
#endif
    if (c != LF) { reader->unreadChar(c); if (returnCr) reader->unreadChar(CR); return false; }
```

Under a Windows build, a line must be `CR LF`; a bare `LF` fails. Polygon's build runs
`validator.exe`, so it is on that branch.

Corroborating: Polygon rewrites a manual test's `testInput` to CRLF no matter how it is
submitted — form-urlencoded **and** multipart both store CRLF, while
`testInputForStatements` in the same request keeps LF. CRLF is Polygon's canonical
test-file form, and its validator is compiled to match.

### Direct evidence

A temporary validator that dumps the raw bytes Polygon supplies:

```cpp
#include "testlib.h"
#include <string>
int main(int argc, char *argv[]) {
    registerValidation(argc, argv);
    std::string s;
    for (int i = 0; i < 36 && !inf.eof(); ++i) {
        char c = inf.readChar();
        s += (c == '\r') ? "{CR}" : (c == '\n') ? "{LF}" : (c == ' ') ? "{SP}" : std::string(1, c);
    }
    quitf(_fail, "BYTES=[%s]", s.c_str());
}
```

Build output:

```
FAIL BYTES=[88{LF}1{SP}9999{SP}50{LF}29134{SP}29137{SP}50{LF}99999{SP}10]
```

Bare LF, byte-identical to what the generator produces locally — confirming the data is
correct and the *judging* of it is what differs.

### Confirmation

Removing the block from `outputs/gentest.cpp` and rebuilding, with nothing else changed:

```
{'standard': 'READY', 'linux': 'READY', 'windows': 'READY'}
Package created in 51029 ms
```

## The fix

Delete both parts of the binary-mode guard from `gentest.cpp`:

```cpp
// DELETE — breaks Polygon
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

// DELETE — breaks Polygon
#ifdef _WIN32
    _setmode(_fileno(stdout), _O_BINARY);   // "keep '\n' as a bare LF"
#endif
```

`cout << '\n'` then emits the host's native line ending: CRLF under Polygon's Windows
toolchain (what its validator wants), LF on a Linux host. The generator needs no
line-ending logic of its own.

Applied to all three packages in this repository; all three now build.

## Provenance — not originally a plugin defect

The skill pack never mentioned line endings; `grep -rn "_setmode\|_O_BINARY\|CRLF" skills/`
returns nothing. The `_setmode` block was introduced by the orchestrator's own instructions
to the three preparation sub-agents, which repeated a stale note from an earlier session:

> "Keep the `_setmode` guard in `gentest.cpp` (**it is correct for real judges**), but strip
> `\r\n` -> `\n` locally before feeding a generated file to a validator built with
> `-DFOR_LINUX`."

The parenthetical is false, and it is the whole bug. The first half is still true: this
Windows box emits CRLF from generators regardless, so a *local* validator built with
`-DFOR_LINUX` does need CRLF stripped before it is fed. That local workaround was
generalised into a source-code change, and that is where it went wrong.

**That stale note still exists** at
`~/.claude/projects/E--teaching-document-prep-bai-arrow-hcmus-camp-icpc-r1/memory/msys2-testlib-quirks.md`
and will mislead any future session in that project. It should be corrected or deleted.

The plugin is still worth changing, because its silence on the subject is what left the
question open to a wrong answer.

---

## Proposed plugin changes

### 1. `skills/generating-tests/SKILL.md` — add to the generator requirements

> - **Never force a line-ending mode.** Print newlines as `cout << '\n'` and leave stdout in
>   text mode. Do not call `_setmode(_fileno(stdout), _O_BINARY)`, `setmode`, or
>   `freopen(NULL, "wb", stdout)`, and do not emit `"\r\n"` explicitly. Polygon generates and
>   validates tests under a Windows toolchain whose strict testlib `eoln()` requires CRLF, so
>   forcing bare LF fails every package build with
>   `Validator 'validator.exe' returns exit code 3 [FAIL Expected EOLN (stdin, line 1)]`.
>   If a validator on the authoring machine needs different line endings, normalise the file
>   at validation time — never in the generator.

### 2. `skills/uploading-to-polygon/SKILL.md` — extend the Phase 6 failure notes

The skill already documents the `Validator 'validator.exe' returns exit code 3` case and
attributes it to "a script line whose argv does not reproduce the intended subtask/bounds,
or a generator violating a validator constraint — both upstream bugs". Add a third, and
list it first because it is both the most likely and the least visible:

> - **A generator that forces bare LF.** Check `gentest.cpp` for `_setmode`/`_O_BINARY`
>   before suspecting argv or bounds; see `generating-tests`. The give-away is that every
>   test validates locally.
>
> Two traps in reading this error:
> - `Can't generate input or answer for test N [Request skipped because of previous errors
>   in the requests batch [Reason: …]]` means test N was **skipped**. The `Reason` belongs to
>   a different test in the batch, and N changes between rebuilds of identical data. Do not
>   bisect toward test N.
> - To identify what Polygon actually feeds the validator, temporarily bind a validator that
>   dumps the first ~36 characters with `{CR}`/`{LF}`/`{SP}` markers via `quitf(_fail, …)`.
>   The package error quotes the bytes verbatim. One build settles what six builds of
>   inference will not.

### 3. `skills/uploading-to-polygon/SKILL.md` — correct/extend the sample-test section

Verified field combination that builds:

```
testInput                      = <sample input>
testUseInStatements            = true
testInputForStatements         = ""     # sent, not omitted
testOutputForStatements        = ""     # sent, not omitted
verifyInputOutputForStatements = true
testPoints                     = 0
```

Polygon then renders the test's own input with the **main** solution's answer, so the
printed example cannot go stale. Points to add:

- **An omitted `saveTest` field keeps its previous value.** Re-saving without
  `testInputForStatements` does not clear a pair left by an earlier attempt — it silently
  survives into the next build. Send empty strings explicitly. This is the same hazard the
  skill already warns about for `setValidator`/`enablePoints`, and it applies to `saveTest`
  too.
- **Supplying the pair explicitly makes Polygon judge that stored copy separately**, failing
  with `Can't create english statement: … Output for statements doesn't give OK while
  judging: tests/1`. The current skill text recommends sending the pair with
  `verify_input_output_for_statements=true`; that is the MCP path, and it does not work over
  the raw API.
- **Test indices must stay contiguous from 1.** Deleting the manual sample while the script
  occupies 2..N fails with `Tests are enumerated incorrectly for the testset 'tests'`.
  Renumber the script in the same commit.

### 4. `skills/uploading-to-polygon/SKILL.md` — `wellFormed` is not a remedy

Worth stating outright, since it is the natural first guess: `wellFormed` does **not**
normalise line endings for the validator. Tested both values on the same package across
four builds with no effect. The skill's existing "do not touch `wellFormed`" instruction is
correct; add *why*, so the next reader does not re-test it.

### 5. `skills/uploading-to-polygon/SKILL.md` — verification is not a build

`problem.buildPackage(full=false)` produces only a `standard` package, which does **not**
materialise generated tests — so the generator and validator never run and the build can go
`READY` while the package is broken. Only `full=true` produces `linux`/`windows` packages
and exercises test generation. Every already-built problem on the account used for this work
had `standard`-only packages, which is why none had ever hit this bug.

The skill's checklist already requires a full verified package; it is worth adding that a
`standard`-only `READY` is **not** evidence the tests generate.

### 6. Packaging — version the marketplace checkout

The installed plugin cache and the marketplace checkout both declare `0.1.0`, so Claude Code
never refreshes the cache, and the `Skill` tool silently loads an older pipeline missing
`shaping-problems`, `uploading-to-polygon`, Step 5b `example-test/`, and the `fb-light`
editorial theme. A package built from the cached version has no `problem.json` and no
`example-test/` — exactly the two inputs `uploading-to-polygon` requires. Bump the version
on every skills change.

### 7. `skills/uploading-to-polygon/SKILL.md` — allow a per-file tag override

The tag table maps `tle-*` -> `TL` by filename. Polygon's `verify=true` build **fails** when
a tag does not match the observed verdict:

```
PackageException: tle-dfs-enumeration.cpp got RE on tests#3
which violates tag(s): solution tag TIME_LIMIT_EXCEEDED
```

In that case (`snake-dp-casework`) the candidate recursed once per path cell, so at
`n = 10^6` the first descent was ~`3n` frames deep and it overflowed the stack *before*
reaching the time limit — RE on large `n`, TLE on medium `n`. The Step 4 measurements had
covered only `n <= 20` and the large-`n` verdict was extrapolated.

Two changes worth making:

- **State that the filename prefix is a default, not the tag.** The tag is the *measured*
  verdict; where they differ, the manifest wins. `RJ` ("any verdict except accepted") is the
  correct tag for a candidate that fails different ways on different tests, and the skill
  should name that case explicitly.
- **Give the override a machine-readable home in `manifest.md`**, e.g. a
  `Polygon tag: \`RJ\`` line in the file's section, so a re-upload cannot silently reset it
  to the prefix default.

Related guidance for `skills/creating-problems` Step 4: a TLE candidate's classification
must be measured **at the constraint bound it will actually meet in the test set**, not
extrapolated from small cases. A recursive candidate should additionally be checked for
stack depth at maximum `n`, since a crash and a timeout are different Polygon tags.

---

## Minor operational notes

- **`apiSig` is computed over raw parameter values, not URL-encoded ones.** Encoding both
  ways passes until the first value needing escaping (e.g. `checker=std::wcmp.cpp`), then
  fails with `apiKey: Incorrect signature`.
- **Rate limiting** is tighter than the skill's "~25th rapid call": a ~1.2 s gap still drew
  frequent `429`s while assigning points to 100 tests. ~2 s plus backoff is more realistic;
  budget several minutes for a points pass.
- **`problem.testInput` and `problem.tests`** can disagree on line endings. `problem.tests`
  (the JSON `input` field) is the reliable view of stored bytes.
