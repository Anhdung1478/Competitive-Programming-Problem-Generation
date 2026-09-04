# Competitive Programming Problem Generation

Skill pack for **Claude Code and Cursor** that takes an idea and an intended solution to a
Codeforces Polygon package: OI/ICPC format, subtask scoring, statement, checker, a small
solution zoo (AC / WA / TLE), an input validator, a config-first generator, a test script,
and a Vietnamese HTML editorial. This repository is also a **Claude Code
marketplace**, and it installs from the command line, so it can be used in place or
installed on another machine.

A **fork** of [LLaammTTeerr/competitive-programming](https://github.com/LLaammTTeerr/competitive-programming)
— that repo is a Claude Code plugin for contest *solving* and setting; this fork keeps the
setting side and drops the solving/contest side. It ships no `.mcp.json` and no MCP code.

The pipeline itself — step order, gates, and the source-of-truth rules — is the
[`creating-problems`](skills/creating-problems/SKILL.md) skill. Rigor defaults live in
[`preference.yml`](preference.yml) (`heavy`, the default, vs `light-weight`). Read both
before asking anything they already answer.

## Skills

`creating-problems` is the umbrella: it owns the order and the gates, and drives the
per-step skills below. Ask for a whole package and it runs the pipeline; ask for one
artifact and you can invoke that step's skill directly. In Claude Code they are invoked
as `cp-problem-generation:<skill>`.

| Step | Skill | What it does |
|---|---|---|
| — | `creating-problems` | The workflow: step order, the mandatory ambiguity gate, the Step 1 hard stop, the C++ I/O convention, the Step 4 solution-suite contract, and the cross-artifact consistency gate |
| 1 | `validate-solution` | Audits `source/solution.cpp` against `problem-context.md` and `subtask.md` — constraints, input semantics, complexity, overflow, correctness. A failure is a hard stop for everything downstream |
| 2 | `polygon-statement` | Writes `outputs/statement.txt` in Vietnamese Polygon simpleTex: name, legend, input, output, subtasks. The statement comes early, so the checker and the solution suite are built against finished wording |
| 2b | `tex-statement` | Optional: renders the finished statement into `outputs/tex-statement.tex` (standalone vnolymp LaTeX) and verifies the PDF. A rendering step, not a second author |
| 3 | `polygon-checker` | Writes `outputs/checker.cpp` with testlib — deterministic, constructive, optimization, and special-output checkers. Skipped when the output is unique, in favour of a Polygon standard checker |
| 5 | `polygon-validator` | Writes `outputs/validator.cpp`, a testlib input validator — format, bounds, subtask/group constraints, structural invariants. Written before the test plan, so it is the machine-readable definition of the legal input domain. Validates input files, not contestant output |
| 6 | `generator-config` | Writes `outputs/generator-config.md`: the design contract turning constraints, subtasks, and solution weaknesses into the test plan. Must be complete before anything is generated |
| 7 | `generating-tests` | Derives `outputs/test-script.txt` from that contract — the generated test set with its subtask/rate/seed options and adversarial profile coverage — then implements `outputs/gentest.cpp` with testlib, turning those semantic options into concrete input and delegating structures to the generator skills below. Every generated test must be accepted by the Step 5 validator |
| 8 | `writing-editorials` | `outputs/editorial.html`, a standalone Vietnamese editorial (themes in `skills/writing-editorials/references/themes/`) |

Step 4 — the solution suite in `outputs/solution/` and its `manifest.md` — has no skill of
its own; it is specified directly in `creating-problems` and audited by
`validate-solution`.

Structure generators, driven by `generating-tests`:

| Skill | For |
|---|---|
| `generator-tree` | Trees, rooted trees, parent arrays, tree-constrained subtasks |
| `generator-graph` | Directed/undirected graphs, multigraphs, connectivity, DAGs, sparse/dense |
| `generator-array` | Arrays, permutations, value ordering, duplicates, monotonicity, extremes |
| `generator-number-theory` | Primes, divisors, gcd/lcm, factorization, modular structure |
| `generator-string` | Alphabets, substrings, borders, palindromes, periodicity, runs |

And one cross-cutting skill, `yagni-principles` — keep every generated artifact minimal;
it applies alongside all of the above.

## Layout

```
Competitive-Programming-Problem-Generation/
├── preference.yml            # rigor profiles — read first, every run
├── .claude-plugin/
│   ├── plugin.json           # Claude Code plugin manifest ("skills": ["./skills"])
│   └── marketplace.json      # lets this repo install as a Claude Code marketplace
├── skills/                   # canonical skills — edit here
│   ├── creating-problems/SKILL.md      # the workflow: step order, gates, loop-backs
│   ├── validate-solution/SKILL.md
│   ├── polygon-statement/SKILL.md      (+ references/theme.md)
│   ├── polygon-checker/SKILL.md
│   ├── generator-config/SKILL.md
│   ├── generating-tests/SKILL.md       (+ references/generator-patterns.md)
│   ├── polygon-validator/SKILL.md
│   ├── generator-{tree,graph,array,number-theory,string}/SKILL.md
│   ├── writing-editorials/SKILL.md     (+ references/themes/*.html)
│   ├── tex-statement/SKILL.md
│   └── yagni-principles/SKILL.md
└── install.sh  install.ps1   # command-line installers (see Installing)
```

A **problem repository** the pack is pointed at is Polygon-shaped — `source/` for the
authoritative problem, `outputs/` for everything generated. Nothing has to be copied into
it except a `preference.yml`, and only if you want a per-problem rigor profile:

```
<problem-root>/
├─ preference.yml                    # optional; heavy vs light-weight rigor profile
├─ source/
│  ├─ problem-context.md             # REQUIRED — the authoritative problem
│  ├─ solution.cpp                   # optional intended/official solution
│  └─ subtask.md                     # optional subtask limits and scoring
└─ outputs/
   ├─ statement.txt                  # Step 2
   ├─ tex-statement.tex              # Step 2b, on request
   ├─ checker.cpp                    # Step 3, unless a standard checker fits
   ├─ solution/
   │  ├─ manifest.md                 # verdict, scope, complexity, evidence per candidate
   │  ├─ ac-full-<slug>.cpp          # full-scope AC — at least one
   │  ├─ ac-subtask-<id>-<slug>.cpp  # scoped AC, opt-in only
   │  ├─ wa-<slug>.cpp               # plausible WA
   │  └─ tle-<slug>.cpp              # plausible TLE
   ├─ validator.cpp                  # Step 5 — the legal input domain
   ├─ generator-config.md            # Step 6 — the design contract
   ├─ test-script.txt                # Step 7
   ├─ gentest.cpp                    # Step 7
   └─ editorial.html                 # Step 8
```

## Setup

**Prerequisites:** `g++` and `git`. `testlib.h` for the checker/validator/generator work,
and a LaTeX toolchain only if you use the optional `tex-statement` step.

**testlib.** Official Codeforces/Polygon
[MikeMirzayanov/testlib](https://github.com/MikeMirzayanov/testlib). Do not copy
`testlib.h` into a problem folder — keep one cached checkout and point at it.

**Optional — Polygon upload.** Uploading a reviewed package to Codeforces Polygon goes
through [gsh20040816/cf-polygon-mcp](https://github.com/gsh20040816/cf-polygon-mcp), a
Polygon API MCP server configured **outside this pack** (in your `.mcp.json` / agent
settings, with `POLYGON_API_KEY` and `POLYGON_API_SECRET`). This pack ships no
`.mcp.json`, no MCP code, and **no secret ever lives in this repository**. Without that
server the artifacts in `outputs/` are uploaded to Polygon by hand.

## Installing

**Claude Code — same machine.** Clone into `~/.claude/skills/`. Anything there with a
`.claude-plugin/plugin.json` auto-loads as `<name>@skills-dir`; no install step:

```bash
git clone https://github.com/Anhdung1478/Competitive-Programming-Problem-Generation ~/.claude/skills/cp-problem-generation
# then, in Claude Code:  /reload-plugins
```

**Claude Code — elsewhere.** Install it as a marketplace:

```
/plugin marketplace add https://github.com/Anhdung1478/Competitive-Programming-Problem-Generation
/plugin install cp-problem-generation@cp-problem-generation
```

**Cursor.** Clone into your personal skills folder (available to every project) or into
`.agents/skills/` of one project:

```bash
git clone https://github.com/Anhdung1478/Competitive-Programming-Problem-Generation ~/.cursor/skills/competitive-programming-problem-generation     # personal
git clone https://github.com/Anhdung1478/Competitive-Programming-Problem-Generation .agents/skills/competitive-programming-problem-generation      # one project
```

Then reload or restart Cursor so the skills are picked up.

**One line (any target).** Targets: `claude` (default,
`~/.claude/skills/cp-problem-generation`), `cursor` (`~/.cursor/skills/…`), `project`
(`./.agents/skills/…`).

```bash
# macOS / Linux / git-bash
curl -fsSL https://raw.githubusercontent.com/Anhdung1478/Competitive-Programming-Problem-Generation/main/install.sh | sh -s -- cursor

# Windows PowerShell (default target: claude)
irm https://raw.githubusercontent.com/Anhdung1478/Competitive-Programming-Problem-Generation/main/install.ps1 | iex
```

An existing install updates itself with `git pull` — the installers detect an existing
clone and pull instead of clobbering.

## Using it

In your problem repository, fill in `source/problem-context.md` (plus
`source/solution.cpp` and `source/subtask.md` when you have them). Copy `preference.yml`
there too if you want a per-problem rigor profile; otherwise the pack's copy is used.
Then:

```text
Read the project sources and prepare this problem.
```

That reaches `creating-problems`, which sequences the rest. To regenerate one artifact
only, say so explicitly — the agent still runs the
prerequisite consistency and ambiguity gates that affect it.
