---
name: writing-editorials
description: Write or review outputs/editorial.html as a standalone Vietnamese HTML editorial for a competitive-programming problem. Use for workflow Step 9 or whenever Codex must create, revise, or audit a Vietnamese editorial, solution writeup, tutorial HTML, editorial page, or outputs/editorial.html. Use the supplied space-dark or polygon-light theme and prioritize a validated source/solution.cpp over the Step 4 solution suite.
---

# Write Vietnamese editorials

Create one standalone, scrollable HTML page at `outputs/editorial.html`. Do not create slides, a Markdown-only explanation, or a second solution source file. Do not spawn subagents.

An editorial is a **derivation, not a summary**. The goal is not a short page; the goal is that a strong contestant can follow the reasoning, reconstruct the algorithm, and implement it without guessing a missing step. It should read like a contestant explaining a solution they found themselves, not like a solution translated into Vietnamese after the fact.

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

## Build the derivation before writing prose

Reconstruct the chain below internally before producing any HTML. Every link the reader cannot fill in themselves must appear on the page.

    độ khó thật sự của bài
      → nhận xét
      → vì sao nhận xét đúng
      → vì sao nhận xét có ích
      → hệ quả
      → cách biểu diễn mới
      → trạng thái / bất biến
      → chuyển trạng thái
      → tính đúng đắn
      → độ phức tạp

For the difficulty, identify the actual obstacle: what makes the naive approach too slow, what information looks unnecessarily large, which constraint blocks the direct method, what structure the problem hides. Do not reveal the final technique before the reasoning that motivates it.

The finished page must let the reader answer: why did we look at this, why is this observation useful, how does it restrict the problem, why does that restriction suggest this algorithm, why is the state sufficient, why is the transition correct, why are all cases covered, why is the complexity fast enough. A missing answer means the explanation is incomplete.

Do not compress a non-obvious step merely because the final solution is obvious to you. Announcements such as `Từ đó ta dùng DP`, `Do đó ta duy trì một segment tree`, `Dễ thấy đáp án là...`, or `Bài toán trở thành bài toán quen thuộc...` are acceptable only when the omitted reasoning is genuinely trivial. Otherwise derive the step.

Weak:

> Ta thấy chỉ cần giữ tiền tố nhỏ nhất, nên dùng DP.

Strong:

> Giả sử đã xử lý xong `i` phần tử đầu. Trong toàn bộ tiền tố đó, thứ tự cụ thể của các phần tử không còn ảnh hưởng đến các lựa chọn phía sau; chỉ giá trị nhỏ nhất mà phần tử kế tiếp phải thỏa mãn là còn tác dụng. Vì vậy tiền tố có thể được đại diện bằng đúng giá trị này, cho ta trạng thái `dp[i][j]`.

Never sacrifice reasoning completeness to make the page shorter. The target is the minimum explanation required for independent understanding, not the minimum explanation possible.

## Write in Vietnamese

Write every reader-facing sentence and page label in Vietnamese, except that the rating field must be labeled exactly `Expected rating`. Keep algorithm, technique, and data-structure names in their canonical English form, including `segment tree`, `convex hull trick`, `binary search`, `divide and conquer`, `CDQ`, `BFS`, `DFS`, `DSU`, and `LCA`. Explain their problem-specific role in Vietnamese; do not add general tutorials for standard techniques.

Preserve input identifiers exactly as defined by the statement, including capitalization. Keep newly introduced notation consistent. Use KaTeX notation `\(...\)` and `\[...\]`; escape user-facing `<` and `&` in HTML.

Treat the backslashes in KaTeX delimiters as literal output bytes. When HTML is produced through JSON, JavaScript strings, shell commands, or another escaping layer, use a raw/literal payload or escape the backslashes for that layer. After writing, reread `outputs/editorial.html` from disk and verify that every inline formula still has literal `\(` and `\)` delimiters. Reject artifacts such as `(r \le n)`, `(O(n))`, or `((x,y))`; they indicate that an escaping layer swallowed the delimiters.

Write for contestants, not setters. State the mathematics directly. Remove lore, character names, historical notes, generator details, failed solutions, and implementation accidents from the main explanation unless a name is essential to the mechanic. Do not mention that a source solution was wrong. Avoid pseudocode and pasted C++.

### Style

Write natural Vietnamese technical prose, not translated English. Use `ta` for the solver's reasoning. Vary connectors and sentence shape: `Từ đó`, `Vì vậy`, `Do đó`, `Điều này có nghĩa là`, `Nói cách khác`, `Bây giờ chỉ còn phải`, `Vấn đề còn lại là`. Repeating one connector with the same rhythm through the page is the main symptom of mechanical prose.

Keep the density technical. "Human-sounding" does not mean jokes, slang, filler, anecdotes, or deliberately loose grammar. A formal editorial can still sound human. The target is clear, technically precise, naturally written, logically connected.

Give each paragraph a reason to exist, usually `previous fact → reasoning → consequence`. Disconnected true statements are a defect:

> Ta dùng DSU. Mỗi component được xử lý riêng. Độ phức tạp là `O(N α(N))`.

Better:

> Vì các đỉnh trong cùng một component luôn phải được xử lý cùng nhau, ta có thể gộp mỗi component thành một đơn vị, và DSU làm được việc này. Sau khi xét hết các cạnh, mỗi tập của DSU tương ứng đúng một component, nên từ đây ta chỉ làm việc trên các component thay vì từng đỉnh.

### Avoid AI writing signals

- Do not use `—` as a default clause connector; prefer commas, periods, or parentheses.
- Do not use binary-contrast templates: `Đây không phải là X, mà là Y`, `Ta không quan tâm X, ta chỉ quan tâm Y`. State the reasoning directly instead.
- Do not use fake emphasis: `Đây chính là mấu chốt`, `Điều thú vị là`, `Đây là một insight rất quan trọng`. If something matters, show why.
- Do not use meta-narration: `Hãy cùng xem`, `Bây giờ chúng ta sẽ`, `Trong phần này chúng ta sẽ tìm hiểu`, `Như đã đề cập ở trên`.
- Do not cycle synonyms for a fixed technical term. Repeat `đỉnh`, `cạnh`, `component`, `trạng thái`, `đáp án` as-is; precision beats lexical variety.
- Do not convert a logical chain into bullets. Bullets are for genuinely independent items.
- Do not add a generic closing summary paragraph.

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

Each observation card answers three things: what we notice, why it is true, why it matters. A card stating only the fact is unfinished. After an observation, say explicitly what it now allows.

When a transformation compresses or reshapes the problem, answer what can be forgotten and what must be kept. This matters most for DP states, graph compression, greedy invariants, prefix/suffix information, coordinate compression, monotonicity, DSU components, segment tree contents, automata, and flow constructions.

When introducing a state or maintained quantity, define exactly what it represents, why that information suffices, what is discarded, and how it changes. Never introduce unexplained notation: `Đặt dp[i][j] là chi phí nhỏ nhất` is not a definition until the meaning of `i`, `j`, and the cost is fixed.

Derive transitions rather than announcing them: which choices exist, what each choice does, which state it leads to, why the list is exhaustive, why the resulting value is correct. Explain a formula before presenting it, and define every symbol in it. Do not drop `dp[i] = min(dp[j] + cost(j, i))` and move on; say where each term comes from.

Do not create an algorithm box, paste code, or add pseudocode. Displayed formulas, recurrences, diagrams, inline SVG, and short ASCII illustrations are allowed when they materially clarify the argument.

A card must contain a claim the reader needs later. Fold easy algebra and immediate special cases into the observation that uses them. Remove tautologies and decorative cards. Prefer a few substantial cards over many fragments, and do not turn every small idea into its own heading.

### Tính đúng đắn

Scale the correctness explanation to the actual difficulty.

- For an easy or standard problem, do not create a separate `Chứng minh tính đúng đắn` section or several lemma cards. Give at most one short paragraph alongside the key observation, explaining why the standard method returns the required result.
- For a problem with one non-obvious claim, prove only that claim and finish with one concise connecting sentence.
- Use a dedicated correctness section with multiple lemmas only when several non-obvious claims are genuinely necessary.

Argue correctness with the concepts introduced during the derivation, in the same prose register; do not switch into unnecessary formal machinery. Where an invariant carries the argument, state it, say why it holds initially, why each operation preserves it, and why the final state gives the answer. For greedy, explain why the chosen decision is safe. For DP, explain why the state holds everything the future depends on. For a data structure, explain what each stored value means and why updates and queries preserve that meaning. Never write `hiển nhiên đúng` in place of the one sentence that makes it obvious.

Never repeat standard textbook proofs in full. Keep enough reasoning to justify correctness, but favor clarity and brevity over formal ceremony.

Mention an edge case only when it clarifies correctness or implementation, and say why it matters: `N = 1`, empty sets, duplicate values, disconnected graphs, unreachable states, overflow, binary-search boundaries, DP initialization. Do not append a generic list of edge cases.

### Độ phức tạp

State total time and memory complexity and ensure both match the selected implementation. State amortized versus worst-case behavior when it matters. Include per-query or per-update complexity only for problems whose operations have that shape.

Say where the complexity comes from rather than only naming it: which operation runs how many times, at what cost each. A bound stated without accounting for the dominant operations is not acceptable.

### Other material

Add `Cách giải khác` only for a genuinely different correct approach or a meaningful further optimization. Keep it short and outside the main tutorial. Add a `Fun fact` section only when the user explicitly asks for one.

Explain implementation details only where they are not obvious from the algorithm: why a particular data structure, how indices are represented, how lazy propagation is encoded, how the state is initialized, why `long long` is required, why an operation order matters. Do not walk through the code line by line; the reader should be able to write the implementation from the editorial alone.

## Handle subtasks

Always report authoritative subtask bounds and points in the problem summary, but do not force a solution section for every subtask. Treat subtasks as constraint metadata, not as proof that distinct intended solutions exist.

Add a subtask solution section only when its constraints enable a genuinely different correct approach, proof, or optimization stage that is useful to contestants and does not already solve every later subtask being discussed. If the natural full solution already fits a subtask, cover it only in the unified solution and omit that subtask's solution section. Never invent a weaker algorithm merely to fill a subtask section; some subtasks are intentionally uninformative or deceptive.

When several meaningful solution rungs exist, put shared observations first, explain each distinct rung once, and finish with a compact comparison table. When no distinct partial solution is worth presenting, keep one unified solution and omit all subtask solution sections and the subtask comparison table.

## Priority order

When these conflict, resolve in this order: correctness, completeness of reasoning, logical continuity, technical precision, natural Vietnamese, conciseness, stylistic polish.

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
- no major reasoning step is skipped: every state is defined, every transition explained, every important formula derived, every data structure's purpose stated, and a reader can implement the solution without guessing;
- every paragraph follows from the previous one, observations connect to consequences, and consequences connect to the algorithm;
- the complexity claim accounts for every dominant operation;
- the prose reads as Vietnamese rather than translated English, with varied connectors and no mechanically repeated sentence template;
- no AI-slop signals remain: stray `—`, fake contrasts, `mấu chốt`/`insight` filler, meta-narration, synonym cycling, excessive headings or bullets, generic closing paragraph;
- every subtask solution section contributes a distinct useful approach; no section merely restates a full solution that already handles later subtasks;
- no wrong-answer implementation, generator behavior, or setter-only note leaks into the tutorial;
- only one supplied theme is used and its CSS/chrome remain intact;
- no subtask, limit, rating, sample, or alternative solution is presented as authoritative without support.
