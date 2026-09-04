---
name: writing-editorials
description: Write or review outputs/editorial.html as a standalone Vietnamese HTML editorial for a competitive-programming problem. Use for workflow Step 8 or whenever the agent must create, revise, or audit a Vietnamese editorial, solution writeup, tutorial HTML, editorial page, or outputs/editorial.html. Use the supplied space-dark, polygon-light, or fb-light theme and prioritize a validated source/solution.cpp over the Step 4 solution suite.
---

# Write Vietnamese editorials

Create one standalone, scrollable HTML page at `outputs/editorial.html`. Do not create slides, a Markdown-only explanation, or a contest-wide page collecting several problems. Do not spawn subagents. Write a companion source file only under the opt-in rule in `Companion implementation`.

An editorial is a **derivation, not a summary**. The goal is not a short page; the goal is that a strong contestant can follow the reasoning, reconstruct the algorithm, and implement it without guessing a missing step. It should read like a contestant explaining a solution they found themselves, not like a solution translated into Vietnamese after the fact.

## Read authoritative inputs

Read all available source-of-truth files before writing:

1. `source/problem-context.md` — including its constraints and subtask ladder;
2. `source/solution.cpp` when present;
3. `outputs/solution/manifest.md` and the AC sources it references when present;
4. `outputs/statement.txt`, `outputs/checker.cpp`, and `outputs/validator.cpp` when present;
5. the current `outputs/editorial.html` when reviewing or updating it.

Require all prerequisite workflow gates to have completed successfully. Select the implementation to explain in this order:

1. use `source/solution.cpp` when it exists and passed the complete Step 1 audit;
2. otherwise use a validated full-scope AC solution declared in `outputs/solution/manifest.md`.

When the source solution and suite ACs exist and are valid, prioritize the algorithm and implementation choices in `source/solution.cpp`; use the AC suite as cross-check evidence or to explain genuinely different subtask approaches. A subtask-only AC may support only the matching subtask section and must never be presented as a full solution. Never explain a WA/TLE candidate as the intended solution. File existence alone is not proof of validation. If `source/solution.cpp` failed Step 1, the workflow is under a hard stop and the editorial must not be generated. If no validated full-scope implementation is available, or the selected implementation conflicts with a source-of-truth file, stop and report the inconsistency instead of inventing an algorithm.

Compare the validated source solution with every validated full-scope AC in `outputs/solution/manifest.md`. If a suite AC is materially better in asymptotic complexity, memory, robustness, or implementation simplicity, keep the required source-solution explanation as the main editorial, then add a brief `Lời giải tốt hơn` section near the bottom. State the improved idea, its complexity, and why it improves on the main approach. Do not add this section for cosmetic differences or equal-quality reformulations. Never promote an unvalidated candidate.

Take the problem name and mathematical semantics from the source-of-truth files. Use the final statement to confirm contestant-facing notation. Never infer time limits, memory limits, subtasks, points, samples, or constraints that are not provided. Omit unavailable metadata rows. If `source/problem-context.md` states no subtasks, do not invent them.

### Cross-check the restatement before deriving anything

The restatement is load-bearing: every paragraph of the derivation is built on top of it, so a restatement error silently invalidates the whole page. Check it against `source/problem-context.md` and `outputs/statement.txt` before writing prose, and re-check it when reviewing an existing editorial. The failures that actually happen:

- **Subtask points and bounds.** Copy the split and the per-rung bounds from `source/problem-context.md`; do not assume an even distribution or reuse a split from another problem.
- **Output format.** A grid of characters is not one character per line; a single number is not a list. Confirm against `outputs/checker.cpp` when it exists.
- **Problem-defining guarantees.** Acyclic versus cyclic, directed versus undirected, self-loops and parallel edges allowed or not, connectivity guaranteed or not, values distinct or not. A guarantee missed here produces whole paragraphs handling a case the problem excludes.
- **The exact object being optimized or counted.** Getting the quantity subtly wrong — a cost over pairs of the input rather than pairs of the structure, a maximum where the problem asks for a count — reverses the derivation that follows.

When a mismatch is found, fix the restatement first, then re-derive every part of the solution that leaned on the wrong claim. Do not patch the restatement and leave the derivation as it was.

## Calibrate to the reader

Ask who still needs this editorial, and set skip depth to that reader. Length is not difficulty: a longer writeup does not make a harder problem, and a hard problem does not license a five-page essay.

| Kind of problem | Assume they know | Still explain |
|---|---|---|
| Classical or first-technique (range sum, an introductory DSU-on-tree) | almost nothing about that technique | the structure itself in a few sentences, then the problem |
| Around 1400–1900 | the usual technique names | a one-line reminder only when the twist is unusual |
| 2000+, or the last rung of an OI problem | segment tree, BIT, DSU, binary lifting, standard DP | only the problem-specific twist |

This sets how much to assume. The sections below set what to explain; the two are independent, and compact and complete are not opposites. Proofs follow the same calibration: an easy or immediate step folds into the claim that uses it, a non-obvious or counter-intuitive one gets a real proof with real mathematics.

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

## Stay on the main content

Write only what the reader needs to reconstruct the solution: the observations, why they
are true, the algorithm they force, and the complexity. Cut everything else. Do not warm
up to a point, do not restate an observation after proving it, do not add bridge
paragraphs that only announce what comes next, and do not close a section by summarizing
what it just said.

Say each thing once, in the place where it is needed. If a sentence can be deleted without
leaving a gap the reader must guess across, delete it.

This trims padding, not reasoning. The line above still holds: the target is the minimum
explanation required for independent understanding. A proof, a state definition, or a
transition derivation is main content and stays even when it is long.

## Write in Vietnamese

Write every reader-facing sentence and page label in Vietnamese, except the three header metadata rows, which are labeled exactly `Time limit`, `Memory limit`, and `Expected rating`. Keep algorithm, technique, and data-structure names in their canonical English form, including `segment tree`, `convex hull trick`, `binary search`, `divide and conquer`, `CDQ`, `BFS`, `DFS`, `DSU`, and `LCA`. Explain their problem-specific role in Vietnamese; do not add general tutorials for standard techniques.

Read [`references/vi-glossary.md`](references/vi-glossary.md) before writing. It fixes the page chrome labels, the terms that stay English, the statement label order, and the canonical Vietnamese gloss for graph, tree, game, and combinatorics vocabulary. Follow it rather than translating word for word. If the user corrects a term, update the glossary in the same turn instead of only patching the HTML.

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

### Question-then-answer bridges

At the widest seams of the derivation, voice the reader's own question at the moment it arises and answer it immediately. The question marks the seam; the answer carries the reasoning across it.

> Làm sao đếm được số cách xây mà không phải thử mọi tổ hợp đầu-cuối? Hãy đọc các thao tác theo chiều ngược của thời gian.

> Viết lại như vậy có ích gì? Chi phí bây giờ không còn phụ thuộc vào xâu ban đầu nữa.

Ask only what the reader is genuinely wondering at that point, and answer in the very next clause or sentence. Never leave a question hanging and never ask one for effect. One or two per editorial, at the biggest jumps, is typical; more turns the page into an interrogation. This device is the exception to the ban on meta-narration below, and it is the only one.

### Avoid AI writing signals

- Do not use `—` as a default clause connector; prefer commas, periods, or parentheses.
- Do not use binary-contrast templates: `Đây không phải là X, mà là Y`, `Ta không quan tâm X, ta chỉ quan tâm Y`. State the reasoning directly instead.
- Do not use fake emphasis: `Đây chính là mấu chốt`, `Điều thú vị là`, `Đây là một insight rất quan trọng`. If something matters, show why.
- Do not use meta-narration: `Hãy cùng xem`, `Bây giờ chúng ta sẽ`, `Trong phần này chúng ta sẽ tìm hiểu`, `Như đã đề cập ở trên`.
- Do not cycle synonyms for a fixed technical term. Repeat `đỉnh`, `cạnh`, `component`, `trạng thái`, `đáp án` as-is; precision beats lexical variety.
- Do not convert a logical chain into bullets. Bullets are for genuinely independent items.
- Do not add a generic closing summary paragraph.

## Choose and apply one theme

`space-dark` is the default. The other themes are opt-in: use one only when the user names it.

| id | Look |
|---|---|
| `space-dark` | Dark JetBrains Mono page. Terminal prefixes on the headings. The default. |
| `polygon-light` | Light serif page with a navy title bar and statement-like chrome. |
| `fb-light` | Light feed layout: white panels on a gray canvas, rounded corners, all-monospace type, accent-colored `>` and `[+]` heading prefixes. |

Copy the complete selected template from `references/themes/<id>.html` to `outputs/editorial.html`, then replace its marked slots and placeholder sections. Preserve that template's CSS, chrome, width, badges, panels, print stylesheet, and any trailing script verbatim. Do not mix theme tokens, add a theme toggle, or depend on adjacent files. CDN fonts and KaTeX from the template are allowed.

`fb-light` carries two behaviors that constrain how the body is written:

- **A collapsible title.** Put `<div class="pt-body">` immediately after `<h1 class="problem-title">` and wrap the metadata block, the tags, and every section inside it, closing it just before the footer. Without that wrapper the title has nothing to collapse. Copy the trailing toggler script verbatim; its print stylesheet forces collapsed content open so a closed page still prints in full.
- **Paragraph guide dots.** Every top-level prose paragraph directly inside a section gets a faint dot in the left gutter, so the reader can see which reasoning step they have reached. Write one reasoning step per `<p>`.

State the complexity in the theme's `.complexity` div with KaTeX inside. Never use `<pre class="complexity">`: KaTeX does not render inside `<pre>`.

If the user asks for a new visual style, add a separate theme template instead of modifying an existing theme's design tokens.

## Build the page

### Header

Include:

- the problem name;
- `Time limit` and `Memory limit` rows, only when authoritative values exist;
- an `Expected rating` field containing a Codeforces-style rating as a plain number when it can be responsibly estimated;
- concise Codeforces-style tags inferred from the validated algorithm.

Do not block completion merely to ask for an estimated difficulty or tags. Clearly treat the rating as an estimate. Never write a leading `*` before it.

### Tóm tắt đề bài

Restate the mathematical task in normal Vietnamese, without the story. Keep all rules that affect correctness, followed by the authoritative constraints. Highlight two to five load-bearing facts with `<mark>` when useful, such as an operation being mandatory, strict inequalities, tie-breaking, or unusual output semantics.

Order the section as restatement → `Yêu cầu` → `Giới hạn` → `Subtask`. `Yêu cầu` states what to compute or print and is the last line of the restatement, immediately above `Giới hạn`; it never comes after the subtask list.

When subtasks exist, list the global constraints and every subtask with its bounds and points. Name them `Subtask 1`, `Subtask 2`, … in the visible text, never \(g_1\) or `g1`; an HTML `id` may stay `g1`. Otherwise list only the constraints; do not announce that there are no subtasks and do not name the contest format.

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

#### What earns a card

A card interrupts the prose, so it must earn the interruption. Reserve it for a claim the whole solution rests on and the reader must carry forward — the one that, removed, breaks the derivation. Routine steps stay in the flow as prose: small algebra, an immediate special case, a state that is obviously sufficient, a fact the previous sentence already implies. When in doubt, no card.

Pivotal claims come in recognizable shapes. Test the shape before boxing:

- **The problem-closing step**: the formula that turns the last piece of reasoning into the DP or into the answer. It is the hardest thing on the page to see and the last piece of the puzzle.
- **A representation rewrite that strips the input's influence**: a cost rewritten as a sum over pairs of symbols no longer depends on the input object at all. State the payoff next to the formula.
- **A transformation that opens the solution**: reading the operations backwards in time, filling a table inside out, exchanging the roles of index and value.

A typical editorial uses zero or one card, occasionally two when the pivots are genuinely distinct. They must never form a ladder. If you find yourself writing `Nhận xét 1`, `Nhận xét 2`, `Nhận xét 3` for routine steps, move them back into the prose; number cards only when several genuinely build on each other. Some problems read perfectly well with no card at all — do not invent one to look complete, and do not create a `Bổ đề` that does not exist. If a lemma *is* the problem, keep its proof in full and drop the surrounding cards instead.

Remove tautologies and decorative cards. If removing a card would not change the next step, remove it. Prefer a few substantial cards over many fragments, and do not turn every small idea into its own heading.

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

## Companion implementation

Write `outputs/solution.cpp` only when the user explicitly asks for code that matches the editorial. It is never produced by default and is not part of Step 8.

- **Mirror the editorial, not the source solution.** Implement exactly the algorithm the page derives. Where the explained algorithm and `source/solution.cpp` deliberately differ, follow the editorial; a reader who implements the page must get this file.
- **Comments say what is happening, not why it is correct.** The role of each array, which step of the derivation a block performs, what a formula's terms mean in code. The reasoning and the proofs stay on the page. Add a why-comment only when it fits in one short clause, and never comment trivial code such as input reading, output loops, or a plain `sort`.
- Keep it self-contained GNU C++17, and follow the repository's C++ I/O convention: `cin`/`cout` with `ios_base::sync_with_stdio(false); cin.tie(nullptr);`, never `scanf`/`printf`.
- Accumulate modular sums in `long long` before applying `%` when two reduced values can sum past `INT_MAX`.
- **Verify against the whole test set, not just the samples.** Run every generated input and diff against the expected answers. A sample-only check stays clean while a size or format defect hides in the large tests.

This file does not replace anything in `outputs/solution/`, and it is never used as the validated implementation the editorial explains.

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
- the restatement and constraints match the source-of-truth files, including subtask points and bounds, output format, problem-defining guarantees, and the exact quantity being optimized or counted;
- the statement section runs restatement → `Yêu cầu` → `Giới hạn` → `Subtask`, and the wording follows `references/vi-glossary.md`;
- the complexity is a `.complexity` div with KaTeX inside, never `<pre class="complexity">`;
- cards are limited to pivotal claims: no `Nhận xét 1/2/3` ladder over routine steps, no invented `Bổ đề`;
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
- only one supplied theme is used and its CSS, chrome, and trailing script remain intact; on `fb-light`, `<div class="pt-body">` wraps the whole body and each `<p>` carries one reasoning step;
- when the user asked for a companion implementation, `outputs/solution.cpp` mirrors the editorial's algorithm, compiles, and passes the full test set;
- no subtask, limit, rating, sample, or alternative solution is presented as authoritative without support.
