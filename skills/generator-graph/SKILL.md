---
name: generator-graph
description: Generate general graph structures for gentest.cpp. Use for directed/undirected graphs, simple graphs, multigraphs, connectivity constraints, DAGs, sparse/dense cases, or graph-specific subtasks.
---

# Graph generation

Before generating a graph, the specification must answer:

- directed or undirected;
- loops allowed?;
- parallel edges allowed?;
- connected?;
- DAG?;
- exact/maximum `m`;
- weighted?;
- weight bounds;
- vertex indexing;
- any guaranteed path/reachability/property.

If any property is unspecified and changes legal inputs, ask the user.

## Choose `n` then legal `m`

Derive the legal maximum in `long long`.

Examples:

- simple undirected without loops: `n*(n-1)/2`;
- simple directed without loops: `n*(n-1)`;
- DAG under a chosen topological order: `n*(n-1)/2`.

Respect lower bounds implied by connectivity.

Do not use `int` for expressions like `n*(n-1)`.

## Profiles

Use a mixture of:

- minimum graph;
- sparse;
- tree + few extra edges;
- medium density;
- near-maximum density;
- complete/near-complete when legal;
- path-like;
- star/hub;
- bipartite-like;
- clustered/community;
- adversarial degree distribution;
- reachability/no-reachability profiles when relevant.

## Connected graphs

For a connected simple graph:

1. generate a spanning tree by construction;
2. place tree edges in a hash/set;
3. add legal non-duplicate edges until `m`.

At high density, rejection sampling can become slow. Prefer:

- enumerating candidate edges then sampling/shuffling when feasible;
- complement generation for near-complete graphs;
- structured dense construction.

## Directed graphs

Do not obtain all directed tests merely by randomly orienting undirected graphs if direction is algorithmically important.

Include:

- long directed paths;
- sources/sinks;
- many back edges if cycles are legal;
- SCC-heavy graphs;
- DAG layers;
- unreachable regions when legal.

## Multigraphs/loops

Only generate duplicates or self-loops when explicitly permitted.

When permitted, include them intentionally in some profiles because they expose many solution bugs.

## Weights

Use `genValue` profiles and explicit boundary values.

Include values that stress:

- overflow;
- equality/ties;
- zero or negative values if legal;
- maximum path-sum/distance ranges.

## Validation

Before printing, assert all specified graph invariants and exactly the declared edge count.
