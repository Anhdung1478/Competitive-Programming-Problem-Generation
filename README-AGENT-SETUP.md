# Agent setup

Copy this package into the root of a competitive-programming problem repository.

Expected layout:

```text
<problem-root>/
├─ AGENTS.md
├─ problem-context.md
├─ solution.cpp                 # optional
├─ subtask.md                   # optional
└─ .agents/
   └─ skills/
      ├─ validate-solution/
      ├─ polygon-checker/
      ├─ polygon-statement/
      ├─ test-script/
      ├─ generator-config/
      ├─ polygon-gentest/
      ├─ generator-tree/
      ├─ generator-graph/
      ├─ generator-array/
      ├─ generator-number-theory/
      ├─ generator-string/
      └─ writing-editorials/
```

Codex discovers repository-local skills from `.agents/skills`. Each skill is intentionally focused on one part of the pipeline.

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
