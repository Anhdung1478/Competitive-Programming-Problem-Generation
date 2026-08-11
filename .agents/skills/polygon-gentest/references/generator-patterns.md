# Generator patterns reference

These patterns are intended to be adapted into a problem-specific self-contained `gentest.cpp`.

## Numeric profile names

Recommended baseline:

```text
minimum
uniform
logarit
maximum
```

The purpose is to avoid a generator where every numeric field is uniformly random.

## Size generation

A typical size field should:

1. read its selected subtask limit;
2. derive a rate-scaled legal maximum;
3. choose a value using a profile such as `maximum`;
4. clamp to legal lower/upper bounds;
5. apply any structural relation.

Do not encode a problem's limit in `test-script.txt`.

## Structural generation by construction

Prefer:

```text
construct valid base structure
→ add optional complexity
→ assert invariants
→ shuffle presentation only if legal
→ print
```

over:

```text
generate arbitrary random data
→ repeatedly reject until valid
```

This is especially important for trees, connected graphs, dense graphs, permutations, divisor relations, and guaranteed-answer cases.

## Presentation randomization

After semantic structure is valid, optionally randomize:

- vertex labels;
- edge order;
- edge endpoint orientation for undirected edges;
- array order when ordering is not semantically constrained.

Do not randomize away distinguished roots, topological parent ordering, sorted input requirements, or any other meaningful property.
