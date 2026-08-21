---
name: generator-tree
description: Generate adversarial and diverse tree structures for gentest.cpp. Use whenever the problem input is a tree, rooted tree, parent array, or a graph subtask constrained to be a tree.
---

# Tree generation

Use testlib randomness and generate a valid tree by construction.

Support only tree types relevant to the current problem; typical profiles are:

- `line` — maximum depth;
- `star` — maximum degree at one hub;
- `binary` — balanced-ish deterministic binary parent structure;
- `random` — random recursive tree;
- `long` — mostly attaches near the newest nodes, producing deep trees;
- `focused` — a small set of hubs with occasional random attachments;
- `mixed` — long prefix + hub-heavy middle + random tail;
- `broom` — long path feeding a star;
- `caterpillar` — path backbone with leaves.

## Reusable implementation

A robust self-contained base is provided in `references/tree-generator.hpp`.

Read/copy the parts needed into `gentest.cpp`.

Important properties of the supplied version:

- handles small `n`;
- mutates edge orientation by reference;
- can preserve root label when a rooted problem requires it;
- shuffles edge order separately from vertex relabeling;
- validates `n-1` edges and endpoint ranges.

## Rooted trees

Do not relabel the root away when the statement gives a distinguished root.

When output is a parent array:

- respect the required root convention (`0`, `-1`, omitted parent, etc.);
- guarantee every non-root vertex has exactly one parent;
- if parent indices must be topological (`p_i < i`), do not apply arbitrary relabeling.

## Tree subtask

If a graph problem has a tree-only subtask:

- choose `n` from that subtask's limits;
- force `m = n - 1`;
- do not “generate a graph then hope it is a tree”;
- never add extra edges.

## Profile selection

Do not use only uniform random trees.

Ensure the script/config covers:

- deepest possible structure;
- high-degree structure;
- balanced structure if relevant;
- random structure;
- mixed/hub structure;
- minimum `n` allowed.

Choose profile weights based on the intended algorithm's weak cases.

## Validation

Before output:

- exactly `n-1` edges;
- all endpoints in `[1,n]` or specified index domain;
- no self-loop;
- connected;
- no duplicate undirected edge;
- root constraints preserved.
