# Codeforces tags

The only names `outputs/problem.json` → `tags` may hold, spelled exactly as
Codeforces spells them — lowercase, spaces and hyphens included. Fetched from
the Codeforces API (`problemset.problems`) on 2026-11-13.

## Usable (35)

One per line, most-used first; the order carries no other meaning.

```text
greedy
math
implementation
dp
constructive algorithms
data structures
brute force
binary search
sortings
graphs
dfs and similar
trees
number theory
combinatorics
strings
bitmasks
two pointers
geometry
dsu
divide and conquer
games
shortest paths
probabilities
hashing
flows
matrices
fft
graph matchings
string suffix structures
ternary search
meet-in-the-middle
2-sat
expression parsing
chinese remainder theorem
schedules
```

## Excluded (3)

| Tag | Why not here |
|---|---|
| `*special` | Marks a special contest's problem, not a technique |
| `interactive` | This pack builds no interactive problems |
| `communication` | A run-twice interactive format; same reason |

## How many

Over the 11,194 rated problems: median **3** tags, 90% carry **≤ 5**, and 1.4%
carry none. So a `problem.json` carries **1–5**, usually 2–3. More than 5 means
some tag names a construct that appears rather than an idea the solution needs.

## Refreshing this list

Codeforces adds a tag rarely (`communication` is the newest). To re-check,
collect every `tags[]` entry from `https://codeforces.com/api/problemset.problems`
and diff the set against the 38 above.
