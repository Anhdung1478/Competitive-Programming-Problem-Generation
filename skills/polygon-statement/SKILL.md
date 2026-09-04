---
name: polygon-statement
description: Create or review outputs/statement.txt for a Codeforces Polygon problem in Vietnamese Polygon simpleTex. Use for workflow Step 2 or when writing the problem name, legend, input, output, and subtasks. Do not use the vnolymp standalone LaTeX/PDF workflow.
---

# Write `statement.txt` for Polygon simpleTex

This skill owns problem prose and notation only. It does not invent algorithmic constraints or test data.

## Read first

1. `source/problem-context.md` — source of truth for the problem, its constraints, and its subtask ladder.
2. `outputs/problem.json` — Step 0's shaped numbers. Take from it: `name` and `title.vi` for the heading, `io` for whether the problem reads `stdin`/`stdout` or a named `.inp`/`.out` pair, `limits` when the statement quotes them, and each rung's `id` and `points` for the Subtask section.
3. `source/solution.cpp` when present — for notation only.

`problem-context.md` wins on conflict. Where it and `outputs/problem.json` disagree, write what the markdown says and report the conflict; do not edit either file to make them agree.

## Language

Use Vietnamese unless the user explicitly requests another language.

Prefer standard competitive-programming Vietnamese:

- “Cho ...”
- “Hãy ...”
- “Dòng đầu tiên chứa ...”
- “In ra ...”
- “Với mỗi ...”
- “Đảm bảo rằng ...”

Avoid unnecessarily literary prose.

## Required order

`statement.txt` must contain, in this order:

1. **Tên bài**
2. **Đề bài**
3. **Input**
4. **Output**
5. **Subtask**

Do not add samples, notes, explanations, or extra sections unless the user requests them or the repository defines them as required.

## Story/flavor

Open with at most a short sentence or short paragraph that makes the problem more vivid.

The story must map directly to the mathematical objects and must not introduce rules that are absent from the formal task.

After the flavor text, state the task plainly.

## Precision rules

Before writing, resolve:

- meaning of each symbol;
- whether indices are 0-based or 1-based from the contestant's perspective;
- inclusive bounds;
- whether duplicate values/edges are allowed;
- graph direction/connectivity/loops/parallel edges where relevant;
- multi-test scope and sum constraints;
- what happens in degenerate/no-solution cases;
- whether a valid answer always exists;
- whether output is unique.

If any of these materially affects the statement and is not supported by source files, ask the user instead of inventing it.

## simpleTex conventions

Use only Polygon-friendly simpleTex constructs.

Preferred constructs:

```tex
$n$, $m$, $a_i$
$1 \le i \le n$
$a_1, a_2, \ldots, a_n$
\textbf{Yêu cầu:}
\begin{itemize}
\item ...
\end{itemize}
```

Use `\textbf{...}` for emphasis and `$...$` for mathematics.

Do not add document-level LaTeX such as:

```tex
\documentclass
\usepackage
\begin{document}
```

Do not use vnolymp-specific build commands/environments.

## Recommended concrete file shape

Use clear section labels in the local combined file:

```tex
\textbf{Tên bài:} <tên bài>

\textbf{Đề bài}

<short story>

<formal description>

\textbf{Yêu cầu:} <exact task>

\textbf{Input}

<input format>

\textbf{Output}

<output format>

\textbf{Subtask}

\begin{itemize}
    \item Subtask $1$ ($x\%$ số điểm): ...
    ...
\end{itemize}
```

If the local import tooling expects a different section delimiter, preserve that tooling's format while keeping the same logical order.

## Input section

Describe line structure before constraints.

For multi-test input, state clearly:

- where `T` appears;
- which lines repeat for each test;
- whether any sum constraint spans all tests.

Every variable referenced later must have been introduced.

## Output section

State:

- what to print;
- one line vs multiple lines;
- per-test behavior;
- ordering requirements;
- special no-solution output;
- case sensitivity when printing words;
- tolerance when floating point is involved.

The checker must implement exactly this contract.

## Subtasks

Copy constraints from `problem-context.md`, and take each rung's percentage from `outputs/problem.json`: `subtasks[].points` against `polygon.total_points` in `preference.yml`, so a 40-point rung out of 100 prints as $40\%$. Do not recalculate or “improve” either.

If `problem-context.md` states no subtasks, do not fabricate them. Ask whether the intended setup is a single full-score group or whether subtask information is missing.

Check that percentages sum to 100% when percentages are provided.

## Final ambiguity review

Read the finished statement as a contestant who has not seen the solution.

Flag any sentence that permits two materially different implementations.

Check that all constraints quoted in prose agree with `problem-context.md`.
