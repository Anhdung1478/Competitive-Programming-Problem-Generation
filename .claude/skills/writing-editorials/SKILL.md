---
name: writing-editorials
description: Write or review outputs/editorial.html as a standalone Vietnamese HTML editorial for a competitive-programming problem. Use for workflow Step 9 or whenever Codex must create, revise, or audit a Vietnamese editorial, solution writeup, tutorial HTML, editorial page, or outputs/editorial.html. Use the supplied space-dark or polygon-light theme and prioritize a validated source/solution.cpp over the Step 3 solution suite.
---

# Write Vietnamese editorials

Create one standalone, scrollable HTML page at `outputs/editorial.html`. Do not create slides, a Markdown-only explanation, or a second solution source file. Do not spawn subagents.

## Read authoritative inputs

Read all available source-of-truth files before writing:

1. `source/problem-context.md`;
2. `source/subtask.md` when present;
3. `source/solution.cpp` when present;
4. `outputs/solution/manifest.md` and the AC sources it references when present;
5. `outputs/statement.txt`, `outputs/checker.cpp`, and `outputs/validator.cpp` when present;
6. the current `outputs/editorial.html` when reviewing or updating it.

Require all prerequisite workflow gates to have completed successfully. Select the implementation to explain in this order:

1. use `source/solution.cpp` when it exists and passed the complete Step 1 audit;
2. otherwise use a validated full-scope AC solution declared in `outputs/solution/manifest.md`.

When the source solution and suite ACs exist and are valid, prioritize the algorithm and implementation choices in `source/solution.cpp`; use the AC suite as cross-check evidence or to explain genuinely different subtask approaches. A subtask-only AC may support only the matching subtask section and must never be presented as a full solution. Never explain a WA/TLE candidate as the intended solution. File existence alone is not proof of validation. If `source/solution.cpp` failed Step 1, the workflow is under a hard stop and the editorial must not be generated. If no validated full-scope implementation is available, or the selected implementation conflicts with a source-of-truth file, stop and report the inconsistency instead of inventing an algorithm.

Compare the validated source solution with every validated full-scope AC in `outputs/solution/manifest.md`. If a suite AC is materially better in asymptotic complexity, memory, robustness, or implementation simplicity, keep the required source-solution explanation as the main editorial, then add a brief `Lời giải tốt hơn` section near the bottom. State the improved idea, its complexity, and why it improves on the main approach. Do not add this section for cosmetic differences or equal-quality reformulations. Never promote an unvalidated candidate.

Take the problem name and mathematical semantics from the source-of-truth files. Use the final statement to confirm contestant-facing notation. Never infer time limits, memory limits, subtasks, points, samples, or constraints that are not provided. Omit unavailable metadata rows. If `source/subtask.md` is absent, do not invent subtasks.

## Write in Vietnamese

Write every reader-facing sentence and page label in Vietnamese, except that the rating field must be labeled exactly `Expected rating`. Keep algorithm, technique, and data-structure names in their canonical English form, including `segment tree`, `convex hull trick`, `binary search`, `divide and conquer`, `CDQ`, `BFS`, `DFS`, `DSU`, and `LCA`. Explain their problem-specific role in Vietnamese; do not add general tutorials for standard techniques.

Preserve input identifiers exactly as defined by the statement, including capitalization. Keep newly introduced notation consistent. Use KaTeX notation `\(...\)` and `\[...\]`; escape user-facing `<` and `&` in HTML.

Treat the backslashes in KaTeX delimiters as literal output bytes. When HTML is produced through JSON, JavaScript strings, shell commands, or another escaping layer, use a raw/literal payload or escape the backslashes for that layer. After writing, reread `outputs/editorial.html` from disk and verify that every inline formula still has literal `\(` and `\)` delimiters. Reject artifacts such as `(r \le n)`, `(O(n))`, or `((x,y))`; they indicate that an escaping layer swallowed the delimiters.

Write for contestants, not setters. State the mathematics directly. Remove lore, character names, historical notes, generator details, failed solutions, and implementation accidents from the main explanation unless a name is essential to the mechanic. Do not mention that a source solution was wrong. Avoid pseudocode and pasted C++.

## Choose and apply one theme

Use a theme explicitly requested by the user. Otherwise use `space-dark`.

- `space-dark`: dark JetBrains Mono page.
- `polygon-light`: light, statement-like page.

Copy the complete selected template from `references/themes/<id>.html` to `outputs/editorial.html`, then replace its marked slots and placeholder sections. Preserve that template's CSS, chrome, width, badges, square panels, and print stylesheet verbatim. Do not mix theme tokens, add a theme toggle, or depend on adjacent files. CDN fonts and KaTeX from the template are allowed.

If the user asks for a new visual style, add a separate theme template instead of modifying an existing theme's design tokens.

## Build the page

### Header

Include:

- the problem name;
- time and memory limits only when authoritative values exist;
- an `Expected rating` field containing a Codeforces-style rating as a plain number when it can be responsibly estimated;
- concise Codeforces-style tags inferred from the validated algorithm.

Do not block completion merely to ask for an estimated difficulty or tags. Clearly treat the rating as an estimate. Never write a leading `*` before it.

### Tóm tắt đề bài

Restate the mathematical task in normal Vietnamese, without the story. Keep all rules that affect correctness, followed by the authoritative constraints. Highlight two to five load-bearing facts with `<mark>` when useful, such as an operation being mandatory, strict inequalities, tie-breaking, or unusual output semantics.

When subtasks exist, list the global constraints and every subtask with its bounds and points. Otherwise list only the constraints; do not announce that there are no subtasks and do not name the contest format.

### Lời giải

Present observations in the order needed to derive the intended algorithm:

1. introduce a transformation only when the problem needs a genuinely new view;
2. state each important observation or lemma as a separate card;
3. prove every non-obvious claim closely enough to justify correctness;
4. use short bridge paragraphs to identify the current bottleneck and the next optimization;
5. let the final observation make the implementation method clear.

A card must contain a claim the reader needs later. Fold easy algebra and immediate special cases into the observation that uses them. Remove tautologies and decorative cards. Prefer a few substantial cards over many fragments.

Do not create an algorithm box, paste code, or add pseudocode. Displayed formulas, recurrences, diagrams, inline SVG, and short ASCII illustrations are allowed when they materially clarify the argument.

### Tính đúng đắn

Scale the correctness explanation to the actual difficulty.

- For an easy or standard problem, do not create a separate `Chứng minh tính đúng đắn` section or several lemma cards. Give at most one short paragraph alongside the key observation, explaining why the standard method returns the required result.
- For a problem with one non-obvious claim, prove only that claim and finish with one concise connecting sentence.
- Use a dedicated correctness section with multiple lemmas only when several non-obvious claims are genuinely necessary.

Never repeat standard textbook proofs in full. Keep enough reasoning to justify correctness, but favor clarity and brevity over formal ceremony.

### Độ phức tạp

State total time and memory complexity and ensure both match the selected implementation. State amortized versus worst-case behavior when it matters. Include per-query or per-update complexity only for problems whose operations have that shape.

### Other material

Add `Cách giải khác` only for a genuinely different correct approach or a meaningful further optimization. Keep it short and outside the main tutorial. Add a `Fun fact` section only when the user explicitly asks for one.

## Handle subtasks

Always report authoritative subtask bounds and points in the problem summary, but do not force a solution section for every subtask. Treat subtasks as constraint metadata, not as proof that distinct intended solutions exist.

Add a subtask solution section only when its constraints enable a genuinely different correct approach, proof, or optimization stage that is useful to contestants and does not already solve every later subtask being discussed. If the natural full solution already fits a subtask, cover it only in the unified solution and omit that subtask's solution section. Never invent a weaker algorithm merely to fill a subtask section; some subtasks are intentionally uninformative or deceptive.

When several meaningful solution rungs exist, put shared observations first, explain each distinct rung once, and finish with a compact comparison table. When no distinct partial solution is worth presenting, keep one unified solution and omit all subtask solution sections and the subtask comparison table.

## Validate before finishing

Verify all of the following:

- the page is valid UTF-8 Vietnamese and opens as one scrollable HTML file;
- all visible template placeholders and example cards are replaced or removed;
- inline and display KaTeX delimiters are balanced in the final file read from disk, and TeX commands such as `\le`, `\times`, and `\pmod` do not occur outside those delimiters;
- a representative inline inequality and the main displayed formula render through the template's KaTeX initialization rather than appearing as raw TeX;
- the explained algorithm, variables, edge cases, and complexity match the selected implementation;
- `source/solution.cpp` is selected whenever it exists and passed Step 1; otherwise a validated full-scope AC from `outputs/solution/manifest.md` is selected;
- when a validated full-scope AC is materially better than the selected source solution, a brief bottom section introduces that improvement and gives its correct complexity;
- the restatement and constraints match the source-of-truth files;
- the checker semantics and described output semantics agree;
- the validator's accepted input semantics and described input semantics agree;
- every non-obvious step needed for correctness has a proof;
- every subtask solution section contributes a distinct useful approach; no section merely restates a full solution that already handles later subtasks;
- no wrong-answer implementation, generator behavior, or setter-only note leaks into the tutorial;
- only one supplied theme is used and its CSS/chrome remain intact;
- no subtask, limit, rating, sample, or alternative solution is presented as authoritative without support.
