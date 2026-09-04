---
name: generator-number-theory
description: Generate number-theoretic values for gentest.cpp. Use when primes, divisors, multiples, gcd/lcm, factorization, prime powers, modular structure, or arithmetic edge cases are central to the problem.
---

# Number-theory generation

Do not rely on uniform random integers alone.

First resolve exact bounds and whether zero/negative values are legal.

## Useful value families

When legal and relevant, cover:

- `1`;
- minimum and maximum bounds;
- primes near boundaries;
- small primes;
- prime powers `p^k`;
- semiprimes;
- products of several small primes;
- highly composite/divisor-rich values;
- powers of two;
- neighboring values around powers/primes;
- coprime pairs;
- pairs with large gcd;
- chains where each divides the next;
- many equal multiples of one base;
- values sharing all but one prime factor.

## Prime generation

For moderate bounded ranges, precompute primes with a sieve.

For large 64-bit ranges, use a deterministic 64-bit primality test when actual primality is required; do not repeatedly trial-divide huge random candidates.

Document the chosen method in `generator-config.md`.

## GCD/LCM construction

Prefer construction over rejection:

- choose base gcd `g`, then generate coprime multipliers;
- choose prime-factor masks;
- build divisor chains from a factored base.

Check 64-bit overflow before multiplying.

## Divisors/multiples

If `a | b` is required, choose `a` first and generate a legal multiplier within bounds.

If `a` must not divide `b`, construct a violating prime factor/exponent intentionally rather than hoping random values work.

## Modular edge cases

Include:

- `0`, `1`, `MOD-1`, `MOD`, `MOD+1` when within legal raw input range;
- values congruent to the same residue;
- values that produce negative intermediate residues if negatives are legal.

Only use the actual problem modulus; never assume `1e9+7`.

## Validation

Recompute the required arithmetic property before printing every specially constructed case.
