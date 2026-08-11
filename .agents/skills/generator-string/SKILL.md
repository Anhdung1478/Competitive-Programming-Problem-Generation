---
name: generator-string
description: Generate strings and string-pattern adversarial cases for gentest.cpp. Use when input contains strings, alphabets, substrings, matching, borders, palindromes, periodicity, runs, or lexicographic constraints.
---

# String generation

Resolve:

- length bounds;
- exact alphabet;
- case sensitivity;
- whether empty string is legal;
- whitespace allowed inside strings?;
- distinctness constraints;
- relation between multiple strings.

## Profiles

Use a mixture of:

- one repeated character;
- uniform random alphabet;
- two-character alternating;
- periodic strings with short period;
- long runs;
- palindrome;
- near-palindrome with one mismatch;
- many equal prefixes;
- many equal suffixes;
- border-heavy strings;
- no-border/random strings;
- lexicographically minimum/maximum;
- one rare character among repeats;
- repeated pattern plus a disruptive suffix/prefix.

For pattern matching problems, jointly construct text and pattern to cover:

- no match;
- one match;
- many overlapping matches;
- match at beginning/end;
- all positions match;
- long common prefix before failure.

## Large strings

At `rate≈1`, keep pattern diversity. Do not generate every large string uniformly random; random strings often miss worst cases for prefix-function, hashing, trie, suffix, or automaton logic.

## Output

Use `cout << s << '\n';` for token strings.

If spaces are legal inside strings, follow the exact input format and do not use token-based assumptions.
