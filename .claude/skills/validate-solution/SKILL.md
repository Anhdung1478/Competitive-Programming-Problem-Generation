---
name: validate-solution
description: Validate solution.cpp against problem-context.md and subtask.md before any Polygon artifacts are generated. Use as workflow step B1 or whenever the official solution, constraints, input semantics, complexity, overflow, or correctness must be audited. A failure is a hard stop for downstream generation.
---

This skill is a reference. The canonical, complete instructions live in the shared
`.agents` skill directory so a single skill is maintained in one place only.

**Read `.agents/skills/validate-solution/SKILL.md` now and follow it in full.**

Resolve every relative path in that file (for example `references/...`) against
`.agents/skills/validate-solution/`. Do not edit anything under `.claude/skills/validate-solution/` — apply
changes to the `.agents` copy instead.
