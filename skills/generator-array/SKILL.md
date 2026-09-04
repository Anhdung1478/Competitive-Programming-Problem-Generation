---
name: generator-array
description: Generate arrays, permutations, and numeric sequence profiles for gentest.cpp. Use for array problems or when adversarial value ordering, duplicates, monotonicity, coordinate extremes, or permutation structure matters.
---

# Array and sequence generation

Resolve:

- length bounds;
- element bounds/sign;
- duplicates allowed?;
- permutation/distinctness?;
- sortedness constraints?;
- global sum constraints?;
- relation between neighboring elements if any.

## Numeric profiles

Use a deliberate mix:

- uniform random;
- all minimum;
- all maximum;
- all equal;
- two-value alternating;
- duplicate-heavy small alphabet of values;
- strictly increasing;
- strictly decreasing;
- nondecreasing/nonincreasing with runs;
- sawtooth;
- one outlier among equal values;
- sparse extremes;
- random logarithmic magnitude;
- maximum-biased/minimum-biased.

Map profiles to weaknesses of the intended algorithm.

## Permutations

Generate with testlib permutations/shuffle.

Include:

- identity;
- reverse;
- random;
- almost sorted;
- cyclic shift;
- block-reversed;
- alternating low/high.

Never accidentally introduce duplicates.

## Coupled arrays

For pairs/triples of arrays, generate them jointly when the problem imposes relations.

Do not generate independently then repair in a way that biases away difficult cases.

## Multi-test total length

Maintain a remaining budget and generate case lengths within the global sum bound.

## `rate`

Scale length and, only when appropriate, numeric magnitude separately.

A large instance (`rate≈1`) does not imply every value must be near its maximum; retain value-profile diversity at large sizes.
