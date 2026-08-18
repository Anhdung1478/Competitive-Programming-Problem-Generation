# Agent setup

Copy this package into the root of a competitive-programming problem repository.

Expected layout:

```text
<problem-root>/
├─ AGENTS.md
├─ source/
│  ├─ problem-context.md
│  ├─ solution.cpp                 # optional official/full solution
│  └─ subtask.md                   # optional
├─ outputs/
│  ├─ solution/
│  │  ├─ manifest.md
│  │  ├─ ac-full-<slug>.cpp         # optional full-scope AC
│  │  ├─ ac-subtask-<id>-<slug>.cpp # optional scoped AC
│  │  ├─ wa-<slug>.cpp              # optional plausible WA
│  │  └─ tle-<slug>.cpp             # optional plausible TLE
│  └─ validator.cpp                 # Polygon testlib input validator
└─ .agents/
   └─ skills/
      ├─ validate-solution/
      ├─ polygon-checker/
      ├─ polygon-statement/
      ├─ generator-config/
      ├─ test-script/
      ├─ polygon-gentest/
      ├─ polygon-validator/
      ├─ generator-tree/
      ├─ generator-graph/
      ├─ generator-array/
      ├─ generator-number-theory/
      ├─ generator-string/
      └─ writing-editorials/
```

Codex discovers repository-local skills from `.agents/skills`. Each skill is intentionally focused on one part of the pipeline.

Step 3 creates one or two independently designed correct candidates in `outputs/solution/` and, when meaningful, plausible WA/TLE candidates. `manifest.md` records every candidate's verdict, scope, complexity, validation evidence, known weakness, and the adversarial profiles used by later generator steps.

The generation flow is config-first: Step 5 writes `outputs/generator-config.md`, Step 6 derives `outputs/test-script.txt` from it, Step 7 implements `outputs/gentest.cpp`, and Step 8 validates generated input with `outputs/validator.cpp` before Step 9 writes the editorial.

Typical request to the agent:

```text
Read the project sources and execute the problem-preparation workflow in AGENTS.md.
```

If you only want one artifact regenerated, state that explicitly. The agent should still run all prerequisite consistency/ambiguity gates that affect that artifact.

The supplied `writing-statements-SKILL.md` was used as a source for statement-writing discipline, but this package intentionally targets **Polygon simpleTex** rather than the vnolymp LaTeX/PDF build workflow.

The supplied `gentest.txt` was used as a source for the generator style, especially `testlib.h`, named options, `genValue`, and tree-generation profiles. The reusable tree material in this package also fixes/avoids fragile patterns from a one-off problem-specific generator.


## Polygon test-script seed format

Generated script lines use a positional seed:

```text
gentest 84GOGVE16X --subtask 1 --rate 0.87 > $
gentest 807H8KNO3J --subtask 1 --rate 0.94 > $
```

The seed is always immediately after `gentest`. Do not use `--seed`.
