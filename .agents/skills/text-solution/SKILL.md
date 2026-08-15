---
name: text-solution
description: Create or review output/text-solution.txt as a concise Vietnamese explanation of a competitive-programming solution. Use for workflow Step 8 or whenever the textual solution must match source/solution.cpp or the generated outputs/codex-solution.cpp.
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

Do not include general tutorials or definitions of standard algorithms or data structures such as heavy-light decomposition, centroid decomposition, or segment trees.

Use plain-text notation, not LaTeX. Write indexed values with parentheses, for example:

- `a(i)`, not `a[i]`;
- `dp(i, j, k)`, not `dp[i][j][k]`.

Cover only the material needed to communicate the solution:

1. the central observation or reduction;
2. the processing steps and the purpose of the main maintained values or data structures;
3. essential correctness reasoning;
4. time and memory complexity;
5. important edge-case handling only when it is not obvious from the main method.

## Validate

Before finishing, verify that:

- every described step exists in the selected implementation;
- variable meanings, indexing, data flow, and edge cases match the code;
- the stated time and memory complexity match the implementation and constraints;
- the explanation does not contradict `source/problem-context.md` or `source/subtask.md`;
- the file contains no LaTeX commands or square-bracket array notation.
