# Solution suite — Quân xe

## Time limit

The authoritative time limit is **2 seconds**, from `source/problem-context.md`.

All suite sources mirror the official guarded `antirook.inp`/`antirook.out` redirection and otherwise use standard input/output.

## Candidates

| File | Verdict | Scope | Algorithm | Time | Memory |
|---|---|---|---|---:|---:|
| `ac-full-coordinate-factorization.cpp` | AC | Full constraints | Factor row and column choices into two independent 2-state matrix powers | `O(log k)` | `O(1)` |
| `wa-only-forbids-current-cell.cpp` | WA | Full input domain | Treat the board as a complete graph without self-loops | `O(log k)` | `O(1)` |
| `tle-iterate-four-states.cpp` | TLE | Semantically correct full algorithm | Apply the correct four-category transition once per move | `O(k)` | `O(1)` |

Only one AC solution is generated. The useful full alternatives use the same constant-size state compression; coordinate factorization is the materially different implementation retained here.

## AC design

### `ac-full-coordinate-factorization.cpp`

Every legal move independently changes the row and the column. Therefore a board path is exactly a pair consisting of a valid row-coordinate sequence and a valid column-coordinate sequence. For each dimension, a 2-state matrix tracks whether the coordinate equals its target. This differs from `source/solution.cpp`, which combines both predicates in one 4-state matrix.

Validation evidence: matched `source/solution.cpp` on 1,405 cases, including dimensions equal to one, zero-answer cases, ordinary random values, multiples of the modulus, and values near `10^18`. Five maximum-scale local runs took about 0.009 seconds each.

## Potential wrong and slow solutions

### `wa-only-forbids-current-cell.cpp`

Defect: it misreads “not in the same row or column” as merely “not the current cell”, allowing moves that change only one coordinate.

Counterexample:

```text
1 2 1
```

The correct output is `0`; this candidate prints `1`. A future generator should include one-row and one-column boards.

### `tle-iterate-four-states.cpp`

The recurrence is correct and matched the official solution on 1,403 moderate-`k` cases, but it iterates once for every move. It passes the first three subtasks and can pass weak tests with moderate `k`, while `r=c=2`, `k=10^18` exceeded the authoritative 2-second limit locally. A future generator should include `k` near `10^18` and nondegenerate dimensions.

## Oracle policy

- Full domain: `source/solution.cpp` and `ac-full-coordinate-factorization.cpp`.
- Intentional WA/TLE candidates are never used as oracles.
