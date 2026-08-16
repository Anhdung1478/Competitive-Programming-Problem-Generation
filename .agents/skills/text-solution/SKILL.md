---
name: text-solution
description: Create or review outputs/text-solution.txt as a concise Vietnamese explanation of a competitive-programming solution. Use for workflow Step 8 or whenever the textual solution must match source/solution.cpp or the generated outputs/codex-solution.cpp.
---

# Write `text-solution.txt`

## Read the authoritative inputs

Read these files before writing:

1. `source/problem-context.md`;
2. `source/subtask.md` when present;
3. `source/solution.cpp` when present; otherwise `outputs/codex-solution.cpp`;
4. the current `outputs/text-solution.txt` when reviewing or updating it.

Use `source/solution.cpp` as the implementation to explain when it exists. Otherwise, explain `outputs/codex-solution.cpp`. Do not invent algorithmic steps that are absent from the selected implementation.

## Write the explanation

Write `outputs/text-solution.txt` in Vietnamese.

Keep it easy to understand and reasonably short while retaining the key reasoning needed to learn and implement the solution. Explain only the problem-specific solution: state what each technique is used for and how it contributes to solving this problem.

Start with a `Tóm tắt đề bài` section of one short paragraph. State only the essential input objects, constraints or relations needed to understand the task, and the required output. Do not copy the full statement, add story details, or reveal the solution in this section.

## Terminology

Write the explanatory prose in Vietnamese, but write names of algorithms, techniques, and data structures in their canonical English form. Do not translate established terms into Vietnamese. For example, write `segment tree`, `sparse table`, `binary lifting`, `monotonic stack`, `priority queue`, `heavy-light decomposition`, and `lowest common ancestor (LCA)`. Preserve conventional acronyms and capitalization such as `BFS`, `DFS`, `DSU`, and `LCA`. Explain in Vietnamese what a technique does for the problem when necessary, while keeping its name in English.

Use `block` instead of `khối` when referring to a contiguous group or partition used by the algorithm. Write common moduli in compact exponent form: use `10^9+7` and `10^9+9`, not `1000000007` and `1000000009`.

Do not include general tutorials or definitions of standard algorithms or data structures such as heavy-light decomposition, centroid decomposition, or segment trees.

Use plain-text notation, not LaTeX. Write indexed values with parentheses, for example:

- `a(i)`, not `a[i]`;
- `dp(i, j, k)`, not `dp[i][j][k]`.

Cover only the material needed to communicate the problem and solution, in this order:

1. a brief problem statement;
2. the central observation or reduction;
3. the processing steps and the purpose of the main maintained values or data structures;
4. essential correctness reasoning;
5. time and memory complexity;
6. important edge-case handling only when it is not obvious from the main method.

## Validate

Before finishing, verify that:

- every described step exists in the selected implementation;
- variable meanings, indexing, data flow, and edge cases match the code;
- the stated time and memory complexity match the implementation and constraints;
- the opening problem summary is accurate, brief, and contains no algorithmic explanation;
- the explanation does not contradict `source/problem-context.md` or `source/subtask.md`;
- every algorithm, technique, and data-structure name uses its canonical English terminology and conventional capitalization;
- algorithmic groups use `block`, not `khối`, and common moduli use compact exponent notation;
- the file contains no LaTeX commands or square-bracket array notation.
