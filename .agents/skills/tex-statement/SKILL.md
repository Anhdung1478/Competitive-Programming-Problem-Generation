---
name: tex-statement
description: Build outputs/tex-statement.tex, a standalone vnolymp LaTeX statement compiled from outputs/statement.txt, and verify the resulting PDF. Use when the user asks for a .tex statement, a printable/PDF statement, a vnolymp statement, a booklet-ready statement, or mentions olymp.sty, \exmpfile, or "đề bài bản PDF". This is a rendering step for prose that already exists; to write or change the prose itself use polygon-statement.
---

# Build `outputs/tex-statement.tex` from `outputs/statement.txt`

This skill renders an existing statement into the vnolymp LaTeX template — the Vietnamese
statement package for problems prepared on Polygon — and proves the PDF is right.

**It owns the LaTeX and the build. It does not own the prose.** `outputs/statement.txt`,
produced by `polygon-statement` in workflow Step 2, is the authoritative wording. This
skill re-marks-up those same sentences; it does not rewrite, re-translate, shorten, or
"improve" them, and it never invents a constraint, a bound, a subtask, a sample, or an
edge-case rule that the source files do not state.

## Am I the right skill?

| If the request is really about | Use |
|---|---|
| Writing or changing the statement's wording, notation, or subtask list | `polygon-statement` (Step 2), then come back here |
| The Polygon package's `statement.txt` itself | `polygon-statement` |
| The sample data the `\exmpfile` lines would point at | `test-script` / `polygon-gentest` |
| Explaining the solution to contestants | `writing-editorials` (Step 9) |

Rendering `statement.txt` into `.tex` is unambiguously here. "Fix the statement" is not —
ask which is meant before touching anything, offering both and "both, in this order".

## Read the authoritative inputs first

Read, in this order, everything that exists:

1. `outputs/statement.txt` — the prose to render. Required. If it is missing, stop and say
   Step 2 has not run; do not compose a statement here.
2. `source/problem-context.md` — the problem's source of truth.
3. `source/subtask.md` — subtask bounds and scoring.
4. `outputs/tex-statement.tex` — when reviewing or updating an existing file.

`source/*` files win over `outputs/statement.txt`, and `outputs/statement.txt` wins over
anything you remember. When they disagree, **do not silently pick one**: render the
statement faithfully, then report the disagreement with both readings and say which
artifact has to change. A LaTeX statement that quietly differs from the Polygon statement
is worse than no LaTeX statement, because both look authoritative.

## Get the template

The package is not on CTAN. Clone it shallow (908K) into a cache so a second problem costs
nothing. On this repository's platform (Windows), PowerShell:

```powershell
$VNOLYMP = Join-Path $env:LOCALAPPDATA 'vnolymp'
$REPO = 'https://github.com/LLaammTTeerr/vietnamese-polygon-statement-latex.git'

if (-not (Test-Path $VNOLYMP)) {
    # Clone aside and move into place rather than cloning straight to the cache path,
    # so a second run cannot find a directory that exists but is still half-populated.
    $staging = Join-Path $env:TEMP ("vnolymp-" + [guid]::NewGuid().ToString('N'))
    git clone --depth 1 -q $REPO (Join-Path $staging 'vnolymp')
    try { Move-Item (Join-Path $staging 'vnolymp') $VNOLYMP -ErrorAction Stop } catch {}
    Remove-Item -Recurse -Force $staging -ErrorAction SilentlyContinue
}
try { git -C $VNOLYMP pull --ff-only -q } catch {}   # offline, or lost a race
```

POSIX-shell equivalent, when running under the Bash tool:

```bash
VNOLYMP="${LOCALAPPDATA:-$HOME/.cache}/vnolymp"
REPO=https://github.com/LLaammTTeerr/vietnamese-polygon-statement-latex.git
if [ ! -d "$VNOLYMP" ]; then
    staging="$(mktemp -d)"
    git clone --depth 1 -q "$REPO" "$staging/vnolymp"
    mv "$staging/vnolymp" "$VNOLYMP" 2>/dev/null || true   # first writer wins
    rm -rf "$staging"
fi
git -C "$VNOLYMP" pull --ff-only -q 2>/dev/null || true
```

**`docs/AUTHORING.md` in that checkout is the authority on syntax. Read it; do not work
from memory and do not copy syntax out of this skill file.** It is the version that matches
the `.sty` you are about to compile, which a snippet pasted into a skill never is:
package options and the two-pass rule (§1), the `problem` key list and numbering (§2),
section commands (§3), subtasks (§4), sample tests and figures (§5), editorials (§6),
deliberate surprises (§7), migrating legacy statements (§8).

`samples/kitchen-sink/kitchen-sink.tex` is the widest worked example;
`samples/minimal/minimal.tex` is the smallest.

If the clone fails and no cache exists, stop and report it. Do not hand-roll a
`\documentclass{article}` substitute — a document that is not the template is not the
deliverable, and it will silently disagree with every other problem in a booklet.

## Where the file goes

Write exactly one file: **`outputs/tex-statement.tex`**, a `standalone` document. Build it
in place so the auxiliary files stay inside `outputs/`.

That fixed name is this repository's contract, so use it even though naming a statement
after the problem would normally read better. It also sidesteps the template's worst build
hazard: the checkout contains a `problem.tex` of its own — the name Polygon requires — plus,
in checkouts from before mid-2026, a `statement.tex` and a `contest.tex`. A file of yours
sharing one of those names can lose the `TEXINPUTS` lookup, and the engine then compiles a
document you never wrote and reports

```
! LaTeX Error: Environment problem undefined.
```

at a line inside someone else's file, with nothing in the message naming the cause.
`tex-statement.tex` collides with none of them.

Use `booklet` with `\contest{}` and `\vnolympcover` only when the user explicitly asks for
several problems in one document. For the single problem this repository holds, `standalone`
is correct — a cover page with three blanks on it is worse than no cover page.

## Translate the simpleTex sections, do not rewrite them

`outputs/statement.txt` is Polygon simpleTex with `\textbf{...}` section labels. Map its
sections onto the template's commands:

| `statement.txt` | `tex-statement.tex` |
|---|---|
| `\textbf{Tên bài:} <name>` | the problem name in the `\begin{problem}` key list |
| `\textbf{Đề bài}` body | prose straight after `\begin{problem}`, no heading |
| `\textbf{Yêu cầu:} ...` | keep as the closing `\textbf{Yêu cầu:}` line of that prose |
| `\textbf{Input}` line structure | `\InputFile` |
| `\textbf{Output}` | `\OutputFile` |
| the `Ràng buộc:` itemize | `\Constraints` — hoisted out of the input section |
| `\textbf{Subtask}` itemize | the `subtasks` environment (it emits its own "Chấm điểm" heading) |

**The target order is not the source order.** Emit the sections as

```
prose → \InputFile → \OutputFile → \Constraints → subtasks → \Examples → \Explanation
```

which is what `samples/kitchen-sink/kitchen-sink.tex` does, and a booklet whose problems
agree on it reads as one paper. `statement.txt` keeps its constraints inside the Input
section; the template puts them after the Output section, so they move past it.

Notes that carry rules — the `\textbf{Lưu ý:}` paragraphs in this repository's statements
often do — stay in the body prose where they are, not in `\Note`. Reserve `\Note` for a
genuine aside and `\Explanation` ("Giải thích") for walking through a sample.

Two things change shape rather than merely moving:

- **Constraints get hoisted.** `statement.txt` keeps its `Ràng buộc:` itemize inside the
  Input section; the template has a dedicated `\Constraints`. Move the items verbatim. A
  reader checking whether their `long long` is wide enough should not have to read prose.
- **Subtasks become the `subtasks` environment**, not a plain `itemize`. Read §4 of
  `AUTHORING.md` for its exact row syntax and keep the percentages and bounds byte-identical
  to `source/subtask.md`. Do not recompute or renumber them.

Inline math, `\texttt{}`, `\textbf{}`, and `itemize` carry over unchanged. Keep every
Vietnamese diacritic exactly as written.

`\begin{problem}`'s `time` and `memory` keys take bare numbers; the package owns the units.

**Limits are not yours to invent.** Take the time and memory limits from
`source/problem-context.md`. If it does not state them, ask the user for the authoritative
values in one question rather than writing a plausible `1` / `256`; a wrong limit printed in
a statement is indistinguishable from a real one. If the user has already announced a
provisional testing limit for the solution suite, say explicitly in your report that the
document carries a provisional number.

## Samples belong to whatever produced the tests

Sample tests come in through `\exmpfile`; inline data is impossible, not merely
discouraged, and `AUTHORING.md` §5 explains why.

**Wire up sample files; never author them.** A sample invented while typesetting has no
checker, generator, or model solution behind it, and a wrong expected output is the most
expensive error a statement can carry: it looks authoritative, contradicts the real tests,
and contestants find it before the setter does.

- **Sample files exist** (in `outputs/`, or exported from Polygon) — reference them with
  `\exmpfile` and read them, so the Input and Output sections describe the format they are
  actually in.
- **They do not exist** — the normal state in this repository, since `outputs/statement.txt`
  carries no samples — then leave `\Examples` out entirely and say so in your report. A
  `\exmpfile` pointing at a missing file does not compile, and that would forfeit the whole
  verification pass below over data that is not yours to write. Record exactly what to add
  once tests land:

  ```latex
  \Examples
  \begin{example}
  \exmpfile{ex1.in}{ex1.out}%
  \end{example}
  ```

  Then re-run this skill after Step 7 to fill the block in.

## Build

`latexmk`, `lualatex`, and `pdflatex` are on `PATH` in this environment via TinyTeX.

PowerShell:

```powershell
$env:TEXINPUTS = ".;$VNOLYMP;"     # Windows separator is ';'
latexmk -lualatex -interaction=nonstopmode -cd outputs/tex-statement.tex
```

POSIX shell:

```bash
export TEXINPUTS=".;$VNOLYMP;"     # still ';' — this is Windows TeX Live/TinyTeX
latexmk -lualatex -interaction=nonstopmode -cd outputs/tex-statement.tex
```

**Keep `.` first in `TEXINPUTS`**, and keep the trailing separator (it appends the default
search path). The leading `.` makes your own directory win the lookups described above.
`-cd` makes `latexmk` chdir into `outputs/`, so `.` is that directory and the `.aux`,
`.log`, and `.pdf` land beside the `.tex`.

### TinyTeX is a minimal distribution — expect missing packages

The TinyTeX install on this machine does not ship what the template needs. A first build
fails with `! LaTeX Error: File \`<name>.sty' not found.`, one package at a time. Resolve
each one and rebuild:

```bash
TLMGR="C:/Users/dungliem/AppData/Roaming/TinyTeX/bin/windows/tlmgr.bat"   # not on PATH
"$TLMGR" search --global --file "<name>.sty"   # bare filename; a leading '/' finds nothing
"$TLMGR" install <package>
```

Two traps when automating that loop:

- **The TeX Live package name is often not the file's name.** `expkv-def.sty` ships in
  `expkv-bundle`; there is no `expkv-def` package. When `search` returns several packages,
  prefer the one whose name is exactly the file's basename — a naive "first hit" picks
  `autofancyhdr` for `fancyhdr.sty` and loops forever.
- **`latexmk` caches the failure.** After installing, run `latexmk -C -cd
  outputs/tex-statement.tex` before rebuilding, or it reports "All targets are up-to-date"
  over a run that produced no PDF.

If `tlmgr` refuses with "tlmgr itself needs to be updated", run `"$TLMGR" update --self`
first.

**`sourcesans` will not announce itself as a missing file.** The package sets every heading
in Source Sans and raises its own `! Package vnolymp Error: Source Sans is not installed.`
— but **still produces a PDF**, silently substituting a fallback for every heading. Nothing
in the exit code or the missing-file grep catches it. Install it with
`"$TLMGR" install sourcesans` and check for `Package vnolymp Error` in the log explicitly.

**Use `latexmk`, not bare `lualatex`.** The footer's page total, the cover's overview table,
and the "Bài N." numbering all come from the `.aux` written by the previous run, so the
document needs two passes — and a single pass produces `??` and an empty table **with no
error**. `latexmk` runs the passes; a hand-written `lualatex && lualatex` is one edit away
from becoming a single pass.

To clean the auxiliary files afterwards while keeping the PDF:
`latexmk -c -cd outputs/tex-statement.tex`.

## Done means the PDF is right, not that the build exited 0

The package compiles happily while producing a document with missing glyphs, a subtask
table that does not add up, or text overflowing the page. Check the log and the text:

```bash
grep -icE 'missing character' outputs/tex-statement.log            # must be 0
grep -iE '^! |undefined control sequence|overfull|underfull' outputs/tex-statement.log
grep -iE 'Package vnolymp (Error|Warning)' outputs/tex-statement.log
pdftotext -layout -enc UTF-8 outputs/tex-statement.pdf <scratchpad>/pdftext.txt
```

All four matter, for different reasons:

- **Missing character** means a Vietnamese diacritic vanished from the PDF while still
  looking present in your `.tex`. For this repository's statements that is the single most
  likely defect, and it is silent.
- Subtask percentages that do not sum to 100 are a **warning, not an error** —
  deliberately, so work-in-progress compiles. Nothing else will ever tell you.
- `Package vnolymp Error` catches the font substitution described above, which does not
  stop the build.
- The `pdftotext` pass is where you confirm the limits panel reads the right numbers and
  the headings came out in Vietnamese.

**Write the extracted text to a file and read it; do not pipe it to the terminal.** The
Windows console codepage mangles every Vietnamese diacritic into `?`, which is
indistinguishable from the missing-glyph defect you are looking for. `-enc UTF-8` plus a
file is the only reading that means anything.

**Math variables extract as `` and that is not a defect.** LuaLaTeX embeds
`latinmodern-math.otf` without a ToUnicode map, so `pdftotext` emits one unmapped glyph per
symbol — `$N$` comes out as `` while `\le` and `\cdot` come out fine. Superscripts
flatten too, so `$10^5$` reads as `105`. What proves the glyphs really rendered is the
combination of a **zero** `missing character` count — LuaTeX emits exactly that warning for
a glyph absent from a font — with one extracted glyph per variable at the right position.
Do not "fix" this, and do not report it as a finding.

If `pdftotext` is unavailable, say so and verify the text another way rather than skipping
the check. No PDF rasteriser (`pdftoppm`, `mutool`, PyMuPDF) is installed here, so a visual
check is not available; the log-plus-extraction evidence above is the substitute.

Then read the rendered text against `outputs/statement.txt` one section at a time and
confirm every bound, symbol, and subtask percentage survived the conversion.

## Report

"It builds" is not a result. Report:

- the file written and the PDF's page count;
- what the log check found — clean, or the exact warnings;
- the limits the panel reads;
- that `\Examples` was omitted, and what to add when samples exist;
- every disagreement found between `outputs/statement.txt`, `source/problem-context.md`,
  and `source/subtask.md`, with the artifact that must change;
- any limit carried as provisional rather than authoritative.
