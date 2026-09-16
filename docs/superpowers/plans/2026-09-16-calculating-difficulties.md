# `calculating-difficulties` Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Ship a new skill, `cp-problem-generation:calculating-difficulties`, that estimates a Codeforces-style rating for a prepared problem, writes `outputs/difficulty.md`, and can state a measured accuracy instead of asserting one.

**Architecture:** Wiring first, data second. Tasks 1-10 build the skill and its four pointer edits against a small starter anchor set, so the pack is runnable and testable before any expensive phase begins. Tasks 11-13 build the 80-problem corpus. Tasks 14-18 calibrate and freeze the numbers. One build-time Python tool (`fetch-corpus.py`) owns every mechanical step — sampling, fetching, splitting, blinding, checking, scoring — so nothing that decides a number is done by hand.

**Tech Stack:** Markdown skill files, YAML config, Python 3 (stdlib only — verified `python` is 3.14.6 on this machine), `curl`, `git`, `grep`. Subagents via the Agent tool for summarization and blind evaluation.

**Spec:** `docs/superpowers/specs/2026-09-04-difficulty-recognition-design.md`

**Process doc (created by Task 1):** `docs/superpowers/process/calculating-difficulties.md` — the living run state. Read it first in every new session.

## Global Constraints

- Every path in this plan is relative to the repository root, `E:/Project/plugins/Competitive-Programming-Problem-Generation`. Run every command from there.
- **Target definition, verbatim, never reworded:** *the rating the full-constraint version would receive as a single all-or-nothing problem in a modern Div1/Div2 round.*
- Rating scale `800-3500`, rounded to the nearest `100`. Full problem only — never a per-subtask rating, never a Vietnamese tier.
- **±150 is the noise floor.** Do not tune toward, or claim, accuracy better than ±200.
- Draft prerequisite floors (Task 4; Task 17 may shift them as a block): two pointers 1200, DSU 1400, segment tree 1700, digit DP 1900, lazy propagation / SOS DP 2000, FFT 2100, centroid decomposition 2200, suffix automaton 2300.
- Adjustments are capped at **±300 total**: `+100` per extra independent insight (cap `+200`), `+100` disguise, `+100` genuinely heavy implementation, `-100..-200` textbook exercise, `-100` constraint leakage.
- Corpus filter: rated ∧ (Div. 1 ∨ Div. 2) ∧ contest start ≥ 2018-01-01 ∧ rating in `[1100, 2699]`. 8 bands of 200, 10 problems per band, 80 total.
- Split is **7 anchors / 3 eval per band** (56 / 24), deterministic under a fixed seed.
- Targets: `MAE ≤ 200`, `≥65%` within ±200, `|bias| ≤ 75`. A 24-problem eval set carries roughly ±40 standard error — never report a smaller movement as an improvement.
- Tuning is capped at **3 iterations**. Bias first (shift all floors by one constant), then band compression.
- **Band rule at freeze (Task 18):** `MAE ≤ 200` → emit `±200`; `MAE ≤ 300` → emit `±300`; `MAE > 300` → emit a range only, e.g. `2200-2600 (ước lượng)`. `SKILL.md` states the measured MAE, the eval `n`, and the date in all three cases. The skill ships either way; it never claims an interval it did not earn.
- `references/` is runtime. `calibration/` is build-time and must never be named by `SKILL.md`, by any file `SKILL.md` names, or by any runtime instruction. Neither directory ever moves to the plugin root.
- The skill is offline at runtime. Network use belongs to `fetch-corpus.py` only.
- `.cache-cf-corpus/` at the repo root is gitignored and holds statement text. It is **not** the session scratchpad — the scratchpad is session-specific and disappears between sessions.
- Never re-run `sample` once `corpus.md` is committed: the CF problemset grows, so a re-run silently selects a different 80 and invalidates every measurement taken so far.
- `yagni-principles` applies to every file this plan creates. No feature that no task consumes.
- Commit after each task. Do not squash tasks into one commit. Update the process doc's ledger in the same commit as the task it describes.

---

### Task 1: The process doc — the entry point for every later session

**Files:**
- Create: `docs/superpowers/process/calculating-difficulties.md`

**Interfaces:**
- Consumes: nothing.
- Produces: the ledger every later task updates, and the two "read first" pointers a fresh session enters through.

- [ ] **Step 1: Write the check that must fail now**

Run:
```bash
ls docs/superpowers/process/calculating-difficulties.md
```
Expected now: `No such file or directory`.

- [ ] **Step 2: Create the process doc**

Create `docs/superpowers/process/calculating-difficulties.md` with exactly this content:

```markdown
# calculating-difficulties — run state

This file is **state, not method**. It says where the work stopped, what it cost,
and what must not be paid for twice. It never explains how to do a task.

## Read first, in this order

1. `docs/superpowers/specs/2026-09-04-difficulty-recognition-design.md` — why, and the shape.
2. `docs/superpowers/plans/2026-09-16-calculating-difficulties.md` — what to do, task by task.

Do not start work before both are read. Then return here for the current position.

## Where we are

Task 1 done. Next: Task 2 (`fetch-corpus.py`, sample + fetch verbs).

## Phase ledger

| Task | Phase | Status | Artifact | Verify it is really done | Cost | Date |
|---|---|---|---|---|---|---|
| 1 | A wiring | done | `docs/superpowers/process/calculating-difficulties.md` | this file exists | — | 2026-09-16 |
| 2 | A wiring | not started | `skills/calculating-difficulties/calibration/fetch-corpus.py` | `python …/fetch-corpus.py` prints usage | 0 agents | — |
| 3 | A wiring | not started | `skills/calculating-difficulties/SKILL.md` | frontmatter `name:` matches the directory | 0 agents | — |
| 4 | A wiring | not started | `references/tag-floors.md` | 8 technique rows | 0 agents | — |
| 5 | A wiring | not started | `references/anchors.md` (starter) | 8 rows, ratings from the API | 0 agents | — |
| 6 | A wiring | not started | `skills/creating-problems/SKILL.md` | `grep -c "Step 8a" ` ≥ 3 | 0 agents | — |
| 7 | A wiring | not started | `skills/writing-editorials/SKILL.md` | `grep -c "difficulty.md"` ≥ 2 | 0 agents | — |
| 8 | A wiring | not started | `preference.yml` | `grep -c step8a_difficulty` = 1 | 0 agents | — |
| 9 | A wiring | not started | `README.md` | `grep -c calculating-difficulties` ≥ 3 | 0 agents | — |
| 10 | A wiring | not started | smoke-test transcript | both branches produced a file | 0 agents | — |
| 11 | B corpus | not started | `calibration/corpus.md` + `.cache-cf-corpus/` | 80 rows, 80 `.txt` files | 0 agents, curl only | — |
| 12 | B corpus | not started | `calibration/eval-set.md`, blind copies | `check` prints `ALL CHECKS PASSED` | 0 agents | — |
| 13 | B corpus | not started | `references/anchors.md` (real, 56) | 56 rows, `check` still passes | ~6 agents | — |
| 14 | C calibrate | not started | `calibration/predictions-baseline.md` | 24 rows | ~5 agents | — |
| 15 | C calibrate | not started | `calibration/predictions-round1.md` | 24 rows | ~5 agents | — |
| 16 | C calibrate | not started | `calibration/metrics.md` | baseline + round 1 sections | 0 agents | — |
| 17 | C calibrate | not started | `metrics.md` rounds 2-3 | ≤ 3 rounds total | ~5 agents/round | — |
| 18 | C calibrate | not started | frozen `SKILL.md` | no `uncalibrated` string remains | 0 agents | — |

## Never re-run without a reason

- **`sample` (Task 11).** The CF problemset grows, so a second run picks a different 80 and
  invalidates every measurement taken against the first. `corpus.md` is committed; it is the
  frozen sample. Re-run only to deliberately start a new calibration generation.
- **Summarization (Task 13).** ~6 agents over 56 statements. `references/anchors.md` having
  56 rows means it is done. Re-run only if the anchor id set changed.
- **Each eval round (Tasks 14, 15, 17).** ~5 agents each. Each round writes its own
  `predictions-round<N>.md` and appends a section to `metrics.md` — never overwrite a round,
  because the comparison between rounds is the only evidence that tuning helped.
- **Statement fetching (Task 11)** is cheap (curl, no model) but `.cache-cf-corpus/` is
  gitignored, so a fresh clone must re-run `fetch`. That is expected and costs no tokens.

## Deviations from the plan

(none yet)

## Before ending a session

1. Update **Where we are** and the ledger row you touched.
2. Commit. An uncommitted change makes every row in this file a lie.
3. If you stopped mid-task, say so in **Where we are** — name the step number you reached.
```

- [ ] **Step 3: Verify the pointers resolve**

Run:
```bash
grep -o "docs/superpowers/[a-z]*/[0-9-]*[a-z-]*\.md" docs/superpowers/process/calculating-difficulties.md | sort -u | while read -r f; do test -e "$f" && echo "OK  $f" || echo "MISSING  $f"; done
```
Expected: `OK docs/superpowers/specs/2026-09-04-difficulty-recognition-design.md` and `OK docs/superpowers/plans/2026-09-16-calculating-difficulties.md`. Any `MISSING` means a typo in a path — fix it before committing.

- [ ] **Step 4: Commit**

```bash
git add docs/superpowers/process/calculating-difficulties.md
git commit -m "docs: add the calculating-difficulties run-state doc"
```

---

### Task 2: `fetch-corpus.py` — sampling and fetching

**Files:**
- Create: `skills/calculating-difficulties/calibration/fetch-corpus.py`
- Modify: `.gitignore`

**Interfaces:**
- Consumes: nothing.
- Produces: `sample`, `fetch` and `starter` verbs; the `corpus.md` table format `| id | rating | div | date | band | role |` that Tasks 5, 11, 12, 13, 14 and 16 all read; the cache layout `.cache-cf-corpus/<id>.txt`.
- Later tasks extend the same file: Task 12 adds `split`, `blind`, `check`; Task 14 adds `metrics`.

**Why `curl` and not `urllib` for statements:** verified on this machine — the CF **API** answers `urllib` normally, but a **statement page** returns `HTTP 403` to `urllib` even with a browser User-Agent, while plain `curl` returns `200` with 64 KB of HTML. Do not "simplify" the subprocess call away.

- [ ] **Step 1: Write the check that must fail now**

Run:
```bash
python skills/calculating-difficulties/calibration/fetch-corpus.py
```
Expected now: `can't open file ... No such file or directory`.

- [ ] **Step 2: Create the script**

Create `skills/calculating-difficulties/calibration/fetch-corpus.py`:

```python
#!/usr/bin/env python
"""Build-time corpus tool for the calculating-difficulties skill.

This never runs at skill runtime. The skill itself is offline.

    sample    pick 10 rated Div1/Div2 problems per 200-point band -> corpus.md
    starter   pick 1 per band and fetch it, for the provisional anchor set
    fetch     download every sampled statement into the cache as plain text

WARNING: do not re-run `sample` once corpus.md is committed. The Codeforces
problemset grows, so a second run selects a different 80 problems and silently
invalidates every measurement taken against the first.
"""
import html
import json
import random
import re
import subprocess
import sys
import time
import urllib.request
from pathlib import Path

HERE = Path(__file__).resolve().parent          # .../calculating-difficulties/calibration
SKILL = HERE.parent                             # .../calculating-difficulties
REPO = SKILL.parents[1]                         # repository root
CACHE = REPO / ".cache-cf-corpus"
CORPUS = HERE / "corpus.md"

SEED = 20260916
CUTOFF = 1514764800                             # 2018-01-01 UTC
BANDS = [(1100 + 200 * i, 1299 + 200 * i) for i in range(8)]
PER_BAND = 10
UA = "Mozilla/5.0 (compatible; cp-problem-generation calibration)"
COLUMNS = ["id", "rating", "div", "date", "band", "role"]


def api(path):
    req = urllib.request.Request("https://codeforces.com/api/" + path,
                                 headers={"User-Agent": UA})
    with urllib.request.urlopen(req, timeout=120) as resp:
        payload = json.load(resp)
    if payload.get("status") != "OK":
        sys.exit("CF API refused %s: %s" % (path, payload.get("comment")))
    return payload["result"]


def division(contest_name):
    d1 = "Div. 1" in contest_name
    d2 = "Div. 2" in contest_name
    if d1 and d2:
        return "Div1+2"
    if d1:
        return "Div1"
    if d2:
        return "Div2"
    return None


def band_of(rating):
    return (rating - 1100) // 200


def candidates():
    problems = api("problemset.problems")["problems"]
    contests = {c["id"]: c for c in api("contest.list?gym=false")}
    pool = {b: [] for b in range(len(BANDS))}
    for p in problems:
        rating = p.get("rating")
        if rating is None or not (1100 <= rating <= 2699):
            continue
        contest = contests.get(p.get("contestId"))
        if not contest or contest.get("startTimeSeconds", 0) < CUTOFF:
            continue
        div = division(contest.get("name", ""))
        if not div:
            continue
        pool[band_of(rating)].append({
            "id": "%s%s" % (p["contestId"], p["index"]),
            "rating": rating,
            "div": div,
            "date": time.strftime("%Y-%m-%d", time.gmtime(contest["startTimeSeconds"])),
            "role": "",
        })
    return pool


def pick(pool, per_band, seed):
    rng = random.Random(seed)
    rows = []
    for b in range(len(BANDS)):
        entries = sorted(pool[b], key=lambda e: e["id"])
        if len(entries) < per_band:
            sys.exit("band %d has only %d candidates" % (BANDS[b][0], len(entries)))
        rows.extend(sorted(rng.sample(entries, per_band), key=lambda e: e["id"]))
    return rows


def write_corpus(rows):
    lines = [
        "# Corpus — the frozen sample",
        "",
        "Build-time only. Never read at skill runtime, never named by SKILL.md.",
        "Written by `fetch-corpus.py`. Do not edit the table by hand.",
        "",
        "| id | rating | div | date | band | role |",
        "|---|---|---|---|---|---|",
    ]
    for e in rows:
        b = band_of(int(e["rating"]))
        lines.append("| %s | %s | %s | %s | %d-%d | %s |" % (
            e["id"], e["rating"], e["div"], e["date"], BANDS[b][0], BANDS[b][1], e["role"]))
    CORPUS.write_text("\n".join(lines) + "\n", encoding="utf-8")


def read_corpus():
    if not CORPUS.exists():
        sys.exit("%s does not exist — run `sample` first" % CORPUS)
    rows = []
    for line in CORPUS.read_text(encoding="utf-8").splitlines():
        if not line.startswith("|") or line.startswith("| id ") or set(line) <= set("|- "):
            continue
        cells = [c.strip() for c in line.strip("|").split("|")]
        rows.append(dict(zip(COLUMNS, cells)))
    return rows


def to_text(page_html):
    match = re.search(r'<div class="problem-statement">(.*?)</html>', page_html, re.S)
    body = match.group(1) if match else page_html
    body = re.sub(r"<script.*?</script>", " ", body, flags=re.S)
    body = re.sub(r"<br\s*/?>", "\n", body)
    body = re.sub(r"</(p|div|li)>", "\n", body)
    body = re.sub(r"<[^>]+>", " ", body)
    body = html.unescape(body)
    body = re.sub(r"[ \t]+", " ", body)
    body = re.sub(r"\n\s*\n+", "\n\n", body)
    return body.strip()


def fetch_one(problem_id):
    match = re.match(r"(\d+)([A-Za-z]\d*)$", problem_id)
    if not match:
        return "malformed id"
    url = "https://codeforces.com/problemset/problem/%s/%s" % match.groups()
    proc = subprocess.run(["curl", "-sS", "-m", "60", "-A", UA, url],
                          capture_output=True, text=True,
                          encoding="utf-8", errors="replace")
    if proc.returncode != 0:
        return "curl exit %d" % proc.returncode
    if "problem-statement" not in proc.stdout:
        return "no statement in response"
    text = to_text(proc.stdout)
    if len(text) < 200:
        return "extracted only %d chars" % len(text)
    CACHE.mkdir(exist_ok=True)
    (CACHE / ("%s.txt" % problem_id)).write_text(text, encoding="utf-8")
    return None


def cmd_sample():
    if CORPUS.exists():
        sys.exit("%s already exists. Re-sampling invalidates every measurement taken "
                 "against it. Delete it deliberately if that is what you mean." % CORPUS)
    rows = pick(candidates(), PER_BAND, SEED)
    write_corpus(rows)
    print("sampled %d problems into %s" % (len(rows), CORPUS))


def cmd_starter():
    rows = pick(candidates(), 1, SEED + 99)
    print("| id | rating | div | date |")
    for e in rows:
        err = fetch_one(e["id"])
        print("| %s | %s | %s | %s |%s" % (
            e["id"], e["rating"], e["div"], e["date"], "" if err is None else "  FAILED: " + err))
        time.sleep(1.5)
    print("statements cached in %s" % CACHE)


def cmd_fetch():
    rows = read_corpus()
    missing = [r for r in rows if not (CACHE / ("%s.txt" % r["id"])).exists()]
    print("%d already cached, %d to fetch" % (len(rows) - len(missing), len(missing)))
    failures = []
    for i, row in enumerate(missing, 1):
        err = fetch_one(row["id"])
        print("  [%d/%d] %s%s" % (i, len(missing), row["id"],
                                  "" if err is None else "  FAILED: " + err))
        if err:
            failures.append(row["id"])
        time.sleep(1.5)
    print("cached: %d / %d" % (len(rows) - len(failures), len(rows)))
    if failures:
        print("re-run `fetch` to retry: %s" % " ".join(failures))
        sys.exit(1)


VERBS = {"sample": cmd_sample, "starter": cmd_starter, "fetch": cmd_fetch}

if __name__ == "__main__":
    if len(sys.argv) != 2 or sys.argv[1] not in VERBS:
        sys.exit("usage: fetch-corpus.py {%s}" % "|".join(VERBS))
    VERBS[sys.argv[1]]()
```

- [ ] **Step 3: Ignore the cache directory**

In `.gitignore`, after the line `/competitive-programming-main/`, add:

```
# Codeforces statement cache for the calculating-difficulties calibration.
/.cache-cf-corpus/
```

- [ ] **Step 4: Verify the script runs and the verbs are wired**

Run:
```bash
python skills/calculating-difficulties/calibration/fetch-corpus.py
```
Expected: `usage: fetch-corpus.py {sample|starter|fetch}` and a non-zero exit.

Run:
```bash
python skills/calculating-difficulties/calibration/fetch-corpus.py fetch
```
Expected: `... corpus.md does not exist — run `sample` first`. This proves `read_corpus` fails loudly instead of silently returning nothing.

- [ ] **Step 5: Commit**

```bash
git add skills/calculating-difficulties/calibration/fetch-corpus.py .gitignore
git commit -m "feat: add the build-time corpus tool for calculating-difficulties"
```

- [ ] **Step 6: Update the ledger**

Set Task 2's row to `done` with today's date, and **Where we are** to `Task 2 done. Next: Task 3 (SKILL.md v0).` Amend into the same commit:

```bash
git add docs/superpowers/process/calculating-difficulties.md
git commit --amend --no-edit
```

---

### Task 3: `SKILL.md` v0 — the method, marked uncalibrated

**Files:**
- Create: `skills/calculating-difficulties/SKILL.md`

**Interfaces:**
- Consumes: nothing yet — it names `references/tag-floors.md` (Task 4) and `references/anchors.md` (Task 5), which exist by the time Task 10 runs it.
- Produces: the `outputs/difficulty.md` contract that Task 7 (`writing-editorials`) reads, and the `## Calibration status` section that Task 18 rewrites.

- [ ] **Step 1: Write the check that must fail now**

Run:
```bash
grep -c "calculating-difficulties" skills/calculating-difficulties/SKILL.md
```
Expected now: `No such file or directory`.

- [ ] **Step 2: Create the skill**

Create `skills/calculating-difficulties/SKILL.md`:

````markdown
---
name: calculating-difficulties
description: Estimate a Codeforces-style difficulty rating (800-3500) for a prepared problem and record it in outputs/difficulty.md. Use for workflow Step 8a, whenever Step 8 runs, or whenever the user asks how hard a problem is — estimate difficulty, expected rating, Codeforces rating, độ khó, ước lượng rating, bài này rating bao nhiêu, bài này khó cỡ nào. Rates the whole problem only: never a per-subtask rating, never a Vietnamese tier. Requires a solution validated by Step 1; with none it writes `not estimable` instead of guessing.
---

# Estimate problem difficulty

Write one file, `outputs/difficulty.md`, in the problem repository. Change nothing else — Step 8 reads this file and copies the number; it does not re-estimate.

## What the number means

**The rating the full-constraint version would receive as a single all-or-nothing problem in a modern Div1/Div2 round.**

It is a **comparability figure, not a prediction**. It tells a Codeforces-literate reader what class of problem this is. It does not predict how a particular group of students will perform, and it says nothing about individual subtasks. The output file must repeat this, or the number will be over-trusted.

Codeforces ratings are fitted from contest performance — roughly the rating at which in-contest solve probability is 50%. Tags and constraints are not inputs to that fit. There is no formula to recover; this skill places a problem against real rated problems instead.

## Gate: is this estimable at all?

Estimate only when the implementation being rated was validated:

1. `source/solution.cpp` exists and passed the complete Step 1 audit; or
2. a full-scope AC in `outputs/solution/manifest.md` is declared validated.

File existence is not proof of validation. With neither, **stop and write the `not estimable` file** in `Failure branch` below. Do not guess from the statement alone, and do not block the workflow waiting for a solution.

## Pass A — features, no numbers

Read `source/problem-context.md` and the validated implementation. Write down, before considering any number:

- the prerequisite techniques the intended solution genuinely requires;
- the count of **independent** non-obvious observations — two restatements of one idea count once;
- the single hardest step, in one sentence;
- implementation weight: how much code, and how error-prone;
- whether the constraints force the intended solution, or a simpler one also fits;
- whether the statement disguises a standard object (a graph described as a story, a DP described as a game).

**Write no rating in this pass.** Separating extraction from scoring is what stops the estimate being reverse-engineered from a number you already had in mind.

## Pass B — the prerequisite floor

Read [`references/tag-floors.md`](references/tag-floors.md). Take the hardest technique the solution *requires* — not one that merely appears — and read off its floor.

A floor is not an estimate. It is the level below which this problem cannot land, whatever the code looks like. It exists to block the common failure of rating a digit-DP problem 1400 because the implementation came out clean.

If the solution requires nothing on the list, the floor is `1100`.

## Pass C — anchor placement

**This pass produces the estimate and carries the accuracy.** Judging magnitude on an unfamiliar scale is hard; comparing two concrete problems is not. Do the second.

Read [`references/anchors.md`](references/anchors.md). In the window `[floor, floor+600]`, pick 2-3 anchors and place this problem against them:

- one that is clearly **harder** than this problem — one sentence saying why;
- one **similar** — one sentence;
- one clearly **easier** — one sentence.

State the estimate this placement implies. If no anchor in the window is comparable, widen to `[floor, floor+800]` and say in the output that the placement was weak.

## Pass D — adjustments, capped at ±300

Apply only what Pass A actually found:

| Adjustment | When |
|---|---|
| `+100` each, cap `+200` | each independent insight beyond the first |
| `+100` | the statement disguises a standard object |
| `+100` | genuinely heavy implementation, not merely long |
| `-100` to `-200` | a textbook exercise in its technique |
| `-100` | constraint leakage — `n ≤ 20` broadcasts "bitmask" |

**The total is capped at ±300**, and that cap is the point: deltas must never override the anchors. If the deltas want more than ±300, the anchor placement in Pass C was wrong — redo Pass C instead of raising the cap.

## Pass E — gates and output

1. Round to the nearest `100`.
2. Clamp to `[800, 3500]`.
3. Attach the interval from `Calibration status` below.
4. Write `outputs/difficulty.md`.

## The output file

Write exactly this shape:

```markdown
# Độ khó ước lượng

**Expected rating: 2300 ± 200 (ước lượng)**

Con số này là mức để so sánh, không phải dự đoán kết quả của học sinh. Nó cho biết
bài này thuộc nhóm nào trên thang Codeforces, và không áp dụng cho từng subtask.

## Cơ sở

- **Sàn kỹ thuật:** 1900 — digit DP là kỹ thuật khó nhất mà lời giải bắt buộc dùng.
- **Số quan sát độc lập:** 2
- **Bước khó nhất:** <one sentence>
- **Khối lượng cài đặt:** <one sentence>

## Đối chiếu

| Bài | Rating | So sánh |
|---|---|---|
| 1800D | 1600 | Dễ hơn: chỉ cần một quan sát, không có trạng thái phụ |
| … | … | … |

## Điều chỉnh

| Điều chỉnh | Lý do |
|---|---|
| +100 | <reason> |

Tổng điều chỉnh: +100 (giới hạn ±300).

## Độ tin cậy

<high / medium / low, and one sentence saying why — a weak anchor window, an
unusual technique, or a solution whose complexity depends on input shape>

---
Nguồn: kỹ năng `calculating-difficulties`, <calibration status line>.
```

Keep it to that. This file is an audit trail, not an essay.

## Failure branch

No validated implementation ⇒ write `outputs/difficulty.md` containing:

```markdown
# Độ khó ước lượng

**Expected rating: not estimable**

Chưa có lời giải nào được kiểm định ở Step 1, nên không thể ước lượng độ khó.
<one sentence naming what is missing>

---
Nguồn: kỹ năng `calculating-difficulties`.
```

Then continue. A missing estimate is not a workflow failure — Step 8 renders `chưa xác định` and moves on. Never replace the absence with a guess.

## Calibration status

**Uncalibrated.** The floors and anchors here are drafts; no accuracy has been measured yet. Emit `± 200` as the interval and say `chưa hiệu chuẩn` in the source line of the output file.
````

- [ ] **Step 3: Verify the frontmatter and the runtime/build-time boundary**

Run:
```bash
head -4 skills/calculating-difficulties/SKILL.md
grep -c "calibration/" skills/calculating-difficulties/SKILL.md
```
Expected: the frontmatter block with `name: calculating-difficulties` matching the directory name, and `0` for the second command. **A non-zero count is a hard failure** — `SKILL.md` naming `calibration/` would pull the eval set into runtime context and make the blind split fake.

- [ ] **Step 4: Commit**

```bash
git add skills/calculating-difficulties/SKILL.md
git commit -m "feat: add the calculating-difficulties skill (uncalibrated)"
```

- [ ] **Step 5: Update the ledger** — Task 3 `done`, **Where we are** → `Task 3 done. Next: Task 4 (tag-floors.md).`, then `git commit --amend --no-edit` after `git add` of the process doc.

---

### Task 4: `references/tag-floors.md`

**Files:**
- Create: `skills/calculating-difficulties/references/tag-floors.md`

**Interfaces:**
- Consumes: the draft floors in Global Constraints.
- Produces: the floor table Pass B reads and Task 17 shifts as a block.

- [ ] **Step 1: Write the check that must fail now**

Run:
```bash
grep -c "segment tree" skills/calculating-difficulties/references/tag-floors.md
```
Expected now: `No such file or directory`.

- [ ] **Step 2: Create the file**

Create `skills/calculating-difficulties/references/tag-floors.md`:

```markdown
# Prerequisite floors

The hardest technique a solution **requires** sets a floor: the level below which the
problem cannot land, whatever the code looks like. A floor is not an estimate — Pass C
produces the estimate.

A technique that merely appears does not count. Ask whether a solver who does not know it
can still solve the problem within the constraints. If yes, it is not a prerequisite.

| Technique | Floor |
|---|---|
| two pointers, prefix sums, sorting + greedy | 1200 |
| DSU, basic graph traversal with a twist, binary search on answer | 1400 |
| segment tree / BIT, standard DP over one dimension, shortest paths | 1700 |
| digit DP, bitmask DP over subsets, tree DP with rerooting | 1900 |
| lazy propagation, SOS DP, matrix exponentiation, flows | 2000 |
| FFT/NTT, convex hull trick, heavy-light decomposition | 2100 |
| centroid decomposition, link-cut-free offline tricks, Mo's on trees | 2200 |
| suffix automaton, suffix tree, advanced string automata | 2300 |

Nothing on the list required ⇒ floor `1100`.

Two techniques from the same row do not stack: take the single hardest, then let Pass D
charge at most `+200` for the extra independent insights they represent.

<!-- Calibration: these are the drafted values. Task 17 may shift the whole column by one
constant to remove signed bias; it does not re-order the rows. -->
```

- [ ] **Step 3: Verify**

Run:
```bash
grep -c "^| " skills/calculating-difficulties/references/tag-floors.md
```
Expected: `9` — the header row plus 8 technique rows.

- [ ] **Step 4: Commit**

```bash
git add skills/calculating-difficulties/references/tag-floors.md
git commit -m "feat: add draft prerequisite floors"
```

- [ ] **Step 5: Update the ledger** — Task 4 `done`, **Where we are** → `Task 4 done. Next: Task 5 (starter anchors).`, amend.

---

### Task 5: `references/anchors.md` — provisional starter set

The starter set exists so Task 10 can run the skill end to end before the corpus exists. Task 13 replaces this file wholesale. Every rating here comes from the CF API — **never write a rating from memory**, because a fabricated label poisons every placement made against it.

**Files:**
- Create: `skills/calculating-difficulties/references/anchors.md`

**Interfaces:**
- Consumes: `fetch-corpus.py starter` (Task 2).
- Produces: the anchor row format `| id | rating | div | prereq | intended solution |` that Task 13 reuses unchanged.

- [ ] **Step 1: Write the check that must fail now**

Run:
```bash
grep -c "^| " skills/calculating-difficulties/references/anchors.md
```
Expected now: `No such file or directory`.

- [ ] **Step 2: Pull 8 real problems, one per band**

Run:
```bash
python skills/calculating-difficulties/calibration/fetch-corpus.py starter
```
Expected: a markdown table of 8 rows — id, rating, div, date — and 8 files in `.cache-cf-corpus/`. Any row ending in `FAILED:` means re-run the verb; CF occasionally drops a connection.

- [ ] **Step 3: Read the 8 cached statements**

Read each `.cache-cf-corpus/<id>.txt` printed by Step 2. They are ~2 KB each. For each, write one sentence naming the intended solution and one prerequisite tag drawn from `tag-floors.md`'s left column.

- [ ] **Step 4: Create the file**

Create `skills/calculating-difficulties/references/anchors.md` with the ids, ratings and divisions **exactly as the API printed them** in Step 2:

```markdown
# Anchors

Real Codeforces problems with their true ratings. Pass C places the problem being rated
against these, in the window `[floor, floor+600]`.

Ratings are labels, not estimates — they come from the Codeforces API. Do not adjust one
because a problem "feels" harder.

| id | rating | div | prereq | intended solution |
|---|---|---|---|---|
| <id from Step 2> | <rating from Step 2> | <div> | <tag> | <one sentence> |
```

**Provisional set — 8 anchors, one per band.** Task 13 replaces this table with 56 anchors drawn from the frozen corpus. The format does not change.

- [ ] **Step 5: Verify every rating against the API**

Run:
```bash
grep "^| [0-9]" skills/calculating-difficulties/references/anchors.md | awk -F'|' '{print $2, $3}'
```
Compare each line against Step 2's printed table. Any mismatch is a transcription error — fix it now, because Pass C treats these as ground truth.

- [ ] **Step 6: Commit**

```bash
git add skills/calculating-difficulties/references/anchors.md
git commit -m "feat: add provisional anchor set (8, one per band)"
```

- [ ] **Step 7: Update the ledger** — Task 5 `done`, **Where we are** → `Task 5 done. Next: Task 6 (Step 8a wiring).`, amend.

---

### Task 6: `creating-problems` — wire in Step 8a

**Files:**
- Modify: `skills/creating-problems/SKILL.md` (routing table ~line 42; optional-steps list ~lines 63-72; new section after Step 8 ~line 543)

**Interfaces:**
- Consumes: the `outputs/difficulty.md` contract from Task 3.
- Produces: the Step 8a step name every later task and the README refer to.

- [ ] **Step 1: Write the check that must fail now**

Run:
```bash
grep -c "Step 8a" skills/creating-problems/SKILL.md
```
Expected now: `0`.

- [ ] **Step 2: Add the routing row**

Replace:
```
| Only an HTML editorial | `writing-editorials` |
```
with:
```
| Only an HTML editorial | `writing-editorials` |
| Only an estimated difficulty/rating | `calculating-difficulties` |
```

- [ ] **Step 3: Extend the optional-steps list**

Replace:
```
- Step 8 `outputs/editorial.html` — only after Step 7 has produced the test
  suite.
```
with:
```
- Step 8 `outputs/editorial.html` — only after Step 7 has produced the test
  suite. Step 8a runs with it.
- Step 8a `outputs/difficulty.md` — mandatory whenever Step 8 runs, and
  available on its own any time after Step 1. It never blocks: with no
  validated solution it records `not estimable` and the workflow continues.
```

Also replace `Three steps are optional and are skipped by default; produce them only on explicit request:` with `Four steps are optional and are skipped by default; produce them only on explicit request:`.

- [ ] **Step 4: Add the Step 8a section**

After the Step 8 section (which ends with `...must not be presented as a full solution.`) and before `## Step 9 (optional) — upload to Codeforces Polygon`, insert:

```markdown
## Step 8a — `outputs/difficulty.md`

Use the `calculating-difficulties` skill. It runs whenever Step 8 runs, and may be run
alone at any point after Step 1. It reads the validated solution and
`source/problem-context.md`, and writes `outputs/difficulty.md`: the estimate with its
interval, the prerequisite floor, the anchors compared, every adjustment applied, and a
confidence line.

The estimate is a comparability figure on the Codeforces scale, not a prediction of how
any particular group will perform, and never a per-subtask rating.

With no validated solution the file records `not estimable` plus the reason, and the
workflow continues — Step 8 then renders `chưa xác định`. Never let a guess stand in for
a missing validation, and never block Step 9 on this step.
```

- [ ] **Step 5: Verify**

Run:
```bash
grep -c "Step 8a" skills/creating-problems/SKILL.md
grep -c "Four steps are optional" skills/creating-problems/SKILL.md
```
Expected: `4` or more for the first, `1` for the second.

- [ ] **Step 6: Commit**

```bash
git add skills/creating-problems/SKILL.md
git commit -m "feat: add Step 8a to the creating-problems workflow"
```

- [ ] **Step 7: Update the ledger** — Task 6 `done`, **Where we are** → `Task 6 done. Next: Task 7 (writing-editorials consumes).`, amend.

---

### Task 7: `writing-editorials` — consume, never estimate

This is the task that removes the defect in spec §1. After it, no skill in the pack estimates a rating as a side effect of writing prose.

**Files:**
- Modify: `skills/writing-editorials/SKILL.md` (authoritative-inputs list ~lines 14-20; header field ~line 177; the escape hatch ~line 180; the review checklist ~line 301)

**Interfaces:**
- Consumes: `outputs/difficulty.md` from Task 3's contract.
- Produces: nothing downstream.

- [ ] **Step 1: Write the check that must fail now**

Run:
```bash
grep -c "difficulty.md" skills/writing-editorials/SKILL.md
```
Expected now: `0`.

- [ ] **Step 2: Add the file to the authoritative inputs**

Replace:
```
5. the current `outputs/editorial.html` when reviewing or updating it.
```
with:
```
5. `outputs/difficulty.md` when present — the Step 8a estimate;
6. the current `outputs/editorial.html` when reviewing or updating it.
```

- [ ] **Step 3: Replace the header field**

Replace:
```
- an `Expected rating` field containing a Codeforces-style rating as a plain number when it can be responsibly estimated;
```
with:
```
- an `Expected rating` field copied verbatim from `outputs/difficulty.md` — including its interval and the `(ước lượng)` marker — or `chưa xác định` when that file is absent or records `not estimable`;
```

- [ ] **Step 4: Replace the escape hatch**

Replace:
```
Do not block completion merely to ask for an estimated difficulty or tags. Clearly treat the rating as an estimate. Never write a leading `*` before it.
```
with:
```
Do not estimate the rating here. Step 8a owns it: copy `outputs/difficulty.md`'s number or write `chưa xác định`. Do not block completion to ask for a difficulty or tags, do not re-derive a number the file already carries, and never write a leading `*` before it.
```

- [ ] **Step 5: Tighten the review checklist**

Replace:
```
- no subtask, limit, rating, sample, or alternative solution is presented as authoritative without support.
```
with:
```
- no subtask, limit, rating, sample, or alternative solution is presented as authoritative without support;
- the `Expected rating` row is a copy of `outputs/difficulty.md` or `chưa xác định` — never a number this editorial invented.
```

- [ ] **Step 6: Verify both directions**

Run:
```bash
grep -c "difficulty.md" skills/writing-editorials/SKILL.md
grep -c "when it can be responsibly estimated" skills/writing-editorials/SKILL.md
```
Expected: `4` for the first and `0` for the second. The second is the one that matters — it proves the old licence to invent a number is gone, not merely supplemented.

- [ ] **Step 7: Commit**

```bash
git add skills/writing-editorials/SKILL.md
git commit -m "refactor: writing-editorials consumes the Step 8a estimate instead of inventing one"
```

- [ ] **Step 8: Update the ledger** — Task 7 `done`, **Where we are** → `Task 7 done. Next: Task 8 (preference.yml).`, amend.

---

### Task 8: `preference.yml` — the rigor override

**Files:**
- Modify: `preference.yml` (`never_relaxed` list ~line 38; `overrides` ~line 68-71)

**Interfaces:**
- Consumes: the Step 8a name from Task 6.
- Produces: nothing downstream.

- [ ] **Step 1: Write the check that must fail now**

Run:
```bash
grep -c "step8a_difficulty" preference.yml
```
Expected now: `0`.

- [ ] **Step 2: Add the never-relaxed rule**

After the line ending `...the document is recompiled, in both profiles."`, add:

```yaml
      - "Step 8a's `not estimable` rule: with no Step 1-validated solution the difficulty is recorded as not estimable in both profiles. A guess never stands in for a missing validation."
```

- [ ] **Step 3: Add the override block**

After the `step8_editorial` block (ending `...Full proof-level derivation of every step is optional."`), add:

```yaml
      step8a_difficulty:
        - "Step 8a runs whenever Step 8 runs, in both profiles; it is cheap and it never blocks."
        - "Compare against one anchor instead of two or three, and skip the harder/easier bracketing when the single anchor is clearly comparable. Passes A, B, D and E are unchanged — the floor, the ±300 cap, and the rounding/clamping gates are not relaxed."
        - "A one-line confidence note is enough; the adjustment table may be omitted when no adjustment was applied."
```

- [ ] **Step 4: Verify the YAML still parses**

Run:
```bash
python -c "import sys; sys.exit(0 if __import__('re') else 0)"
grep -n "step8a_difficulty" preference.yml
grep -c "not estimable" preference.yml
```
Expected: the `step8a_difficulty:` key at one line inside `overrides:`, and `2` for the last command (one in `never_relaxed`, one in the override block).

If a YAML parser is available, prefer it:
```bash
python -c "import yaml,sys; yaml.safe_load(open('preference.yml',encoding='utf-8')); print('YAML OK')"
```
Expected: `YAML OK`. If `yaml` is not installed, skip this — do not install a dependency for one check; the `grep`s above are sufficient.

- [ ] **Step 5: Commit**

```bash
git add preference.yml
git commit -m "feat: add the step8a_difficulty rigor override"
```

- [ ] **Step 6: Update the ledger** — Task 8 `done`, **Where we are** → `Task 8 done. Next: Task 9 (README).`, amend.

---

### Task 9: `README.md` — three insertions

**Files:**
- Modify: `README.md` (skills table ~line 37; Layout block ~line 75; outputs tree ~line 110)

**Interfaces:**
- Consumes: the Step 8a name and the skill's directory layout.
- Produces: nothing downstream.

- [ ] **Step 1: Write the check that must fail now**

Run:
```bash
grep -c "calculating-difficulties" README.md
```
Expected now: `0`.

- [ ] **Step 2: Add the skills-table row**

After the row beginning `| 8 | \`writing-editorials\` |`, add:

```
| 8a | `calculating-difficulties` | Runs with Step 8: estimates a Codeforces-style rating for the finished problem into `outputs/difficulty.md` — a prerequisite floor, placement against real rated anchors, capped adjustments, and a confidence line. Step 8 copies that number and never re-estimates. With no validated solution it records `not estimable` and the workflow continues |
```

- [ ] **Step 3: Add the Layout line**

Replace:
```
    ├── writing-editorials/SKILL.md     # optional Step 8 (+ references/themes/*.html)
```
with:
```
    ├── writing-editorials/SKILL.md     # optional Step 8 (+ references/themes/*.html)
    ├── calculating-difficulties/SKILL.md # Step 8a (+ references/{tag-floors,anchors}.md,
    │                                     #   calibration/ — build-time only, never at runtime)
```

- [ ] **Step 4: Add the outputs-tree line**

Replace:
```
   └─ editorial.html                 # Step 8, on request
```
with:
```
   ├─ editorial.html                 # Step 8, on request
   └─ difficulty.md                  # Step 8a, with Step 8
```

- [ ] **Step 5: Verify**

Run:
```bash
grep -c "calculating-difficulties" README.md
grep -c "difficulty.md" README.md
```
Expected: `3` and `2`.

- [ ] **Step 6: Commit**

```bash
git add README.md
git commit -m "docs: document Step 8a and the calculating-difficulties skill"
```

- [ ] **Step 7: Update the ledger** — Task 9 `done`, **Where we are** → `Task 9 done. Next: Task 10 (smoke test).`, amend.

---

### Task 10: End-to-end smoke test — both branches

`grep` proves an instruction is present. Only this proves it works. Phase A is not done until the skill has produced both kinds of file.

**Files:**
- Create (scratchpad, never committed): a throwaway problem repository.
- Modify: `docs/superpowers/process/calculating-difficulties.md` (record the result).

**Interfaces:**
- Consumes: Tasks 3, 4, 5.
- Produces: the confirmation Phase B builds on.

- [ ] **Step 1: Build a throwaway problem repository**

Use the scratchpad directory named in your environment (it is session-specific — never put it in the repo). Create `<scratchpad>/smoke/source/problem-context.md`:

```markdown
**Problem name**: Đếm số đẹp
**Time limit**: 1 s
**Memory limit**: 256 MB

**Legend**: Cho hai số nguyên L và R. Đếm các số nguyên x trong [L, R] sao cho tổng
các chữ số của x chia hết cho 10.

**Input format**: Một dòng chứa L và R.

**Output format**: Một số nguyên — số lượng x thỏa mãn.

**Constraints**: 1 <= L <= R <= 10^18

**Subtask**: 1) 40% — R <= 10^6.  2) 60% — không ràng buộc thêm.
```

Create `<scratchpad>/smoke/source/solution.cpp`:

```cpp
#include <bits/stdc++.h>
using namespace std;

string digits;
long long memo[20][10][2];
bool seen[20][10][2];

long long go(int pos, int sum, int tight) {
    if (pos == (int)digits.size()) return sum == 0;
    if (seen[pos][sum][tight]) return memo[pos][sum][tight];
    seen[pos][sum][tight] = true;
    int hi = tight ? digits[pos] - '0' : 9;
    long long total = 0;
    for (int d = 0; d <= hi; d++)
        total += go(pos + 1, (sum + d) % 10, tight && d == hi);
    return memo[pos][sum][tight] = total;
}

long long count_upto(long long n) {
    if (n < 0) return 0;
    digits = to_string(n);
    memset(seen, 0, sizeof(seen));
    return go(0, 0, 1);
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    long long l, r;
    cin >> l >> r;
    cout << count_upto(r) - count_upto(l - 1) << '\n';
    return 0;
}
```

State is `(position, digit sum mod 10, tight)` — the shape Pass B must recognize as digit DP. Compile it:

```bash
g++ -O2 -o <scratchpad>/smoke/sol <scratchpad>/smoke/source/solution.cpp && echo COMPILED
```
Expected: `COMPILED`. This stands in for the Step 1 audit — note in the transcript that you are treating it as validated for the purposes of the test.

- [ ] **Step 2: Run the skill — the estimating branch**

Follow `skills/calculating-difficulties/SKILL.md` against that repository, Passes A through E, and write `<scratchpad>/smoke/outputs/difficulty.md`.

Expected, and each is a pass/fail check:
- Pass B lands on floor `1900` — digit DP is required, not incidental.
- Pass C compares against anchors in `[1900, 2500]` from the 8-row starter set. With only 8 anchors the window may hold one or none: confirm the skill then widens to `[floor, floor+800]` and **says the placement was weak**, rather than inventing a comparison. This is the behaviour that matters here, not the number.
- The file carries every section of the template: estimate with interval, the comparability caveat, Cơ sở, Đối chiếu, Điều chỉnh, Độ tin cậy.
- Adjustments total ≤ ±300.
- The rating is a multiple of 100 inside `[800, 3500]`.

- [ ] **Step 3: Run the skill — the `not estimable` branch**

```bash
mv <scratchpad>/smoke/source/solution.cpp <scratchpad>/smoke/solution.cpp.away
```

Run the skill again against the same repository with no other change.

Expected: `outputs/difficulty.md` is rewritten with `**Expected rating: not estimable**`, names the missing validation, and the run reports the workflow continuing rather than stopping. If the skill produced a number from the statement alone, `SKILL.md`'s gate is too weak — fix the gate wording and re-run both branches before moving on.

- [ ] **Step 4: Record the outcome**

In the process doc, set Task 10 to `done` and add one line under **Deviations from the plan** if either branch needed a `SKILL.md` fix — naming what was changed and why.

- [ ] **Step 5: Commit**

```bash
git add docs/superpowers/process/calculating-difficulties.md skills/calculating-difficulties/SKILL.md
git commit -m "test: smoke-test both Step 8a branches end to end"
```

Do not commit anything from the scratchpad.

---

### Task 11: Freeze the 80-problem sample and fetch every statement

Phase B begins. This is the first task that costs real time (~4 minutes of polite fetching); it costs no model tokens.

**Files:**
- Create: `skills/calculating-difficulties/calibration/corpus.md`
- Create (gitignored): `.cache-cf-corpus/*.txt`

**Interfaces:**
- Consumes: `fetch-corpus.py` verbs `sample` and `fetch` (Task 2).
- Produces: `corpus.md` with 80 rows and an empty `role` column, which Task 12 fills; 80 cached statements Tasks 12-15 read.

- [ ] **Step 1: Confirm the filter still yields what the spec claims**

Run:
```bash
python - <<'PY'
import sys
sys.path.insert(0, "skills/calculating-difficulties/calibration")
import importlib.util
spec = importlib.util.spec_from_file_location("fc", "skills/calculating-difficulties/calibration/fetch-corpus.py")
fc = importlib.util.module_from_spec(spec); spec.loader.exec_module(fc)
pool = fc.candidates()
for b, (lo, hi) in enumerate(fc.BANDS):
    print(lo, hi, len(pool[b]))
PY
```
Expected: 8 bands, each with well over 10 candidates. Measured on 2026-09-16: 358, 371, 337, 365, 356, 320, 390, 372. A band under 10 aborts the sample — if that ever happens, the filter or CF itself changed, and that is a finding for the user, not something to work around.

- [ ] **Step 2: Freeze the sample**

Run:
```bash
python skills/calculating-difficulties/calibration/fetch-corpus.py sample
```
Expected: `sampled 80 problems into ...corpus.md`.

Run it a second time. Expected: it refuses, naming the existing file. That refusal is the guard against silently re-sampling.

- [ ] **Step 3: Fetch the statements**

Run:
```bash
python skills/calculating-difficulties/calibration/fetch-corpus.py fetch
```
Expected: `0 already cached, 80 to fetch`, then 80 progress lines, then `cached: 80 / 80`. Re-run the verb if it exits non-zero; it skips what is already cached.

- [ ] **Step 4: Verify the cache**

Run:
```bash
ls .cache-cf-corpus/*.txt | wc -l
grep -c "^| [0-9]" skills/calculating-difficulties/calibration/corpus.md
git status --short .cache-cf-corpus
```
Expected: `80`, `80`, and **empty output** from the last one — the cache must be invisible to git.

- [ ] **Step 5: Commit**

```bash
git add skills/calculating-difficulties/calibration/corpus.md
git commit -m "data: freeze the 80-problem calibration corpus"
```

- [ ] **Step 6: Update the ledger** — Task 11 `done` with the date, cost `0 agents, ~4 min curl`, **Where we are** → `Task 11 done. Next: Task 12 (split + blind + check).`, amend.

---

### Task 12: `split`, `blind`, `check` — and the contamination guard

The disjointness assertion here is the load-bearing test of the whole plan. If an eval problem ever reaches `anchors.md`, every number downstream is fake and nothing else would notice.

**Files:**
- Modify: `skills/calculating-difficulties/calibration/fetch-corpus.py`
- Create: `skills/calculating-difficulties/calibration/eval-set.md`
- Create (gitignored): `.cache-cf-corpus/blind/eval-NN.txt`

**Interfaces:**
- Consumes: `corpus.md` (Task 11).
- Produces: the `role` column filled 56 anchor / 24 eval; `eval-set.md` mapping `slot → id, rating, band`; blind statements named by slot only. Tasks 14-16 join predictions to truth through the slot.

- [ ] **Step 1: Write the failing check**

Run:
```bash
python skills/calculating-difficulties/calibration/fetch-corpus.py check
```
Expected now: `usage: fetch-corpus.py {sample|starter|fetch}` with a non-zero exit — the verb does not exist yet.

- [ ] **Step 2: Add the three verbs**

In `fetch-corpus.py`, add these constants next to `PER_BAND`:

```python
EVAL_PER_BAND = 3
EVAL = HERE / "eval-set.md"
ANCHORS = SKILL / "references" / "anchors.md"
```

Then add the three functions before `VERBS`:

```python
def cmd_split():
    rows = read_corpus()
    if any(r["role"] for r in rows):
        sys.exit("roles are already assigned in %s — re-splitting would move problems "
                 "between the anchor and eval sets and invalidate every measurement" % CORPUS)
    rng = random.Random(SEED + 1)
    by_band = {}
    for row in rows:
        by_band.setdefault(row["band"], []).append(row)
    for band in sorted(by_band):
        entries = sorted(by_band[band], key=lambda e: e["id"])
        chosen = {e["id"] for e in rng.sample(entries, EVAL_PER_BAND)}
        for entry in entries:
            entry["role"] = "eval" if entry["id"] in chosen else "anchor"
    write_corpus(rows)

    evals = sorted([r for r in rows if r["role"] == "eval"], key=lambda r: r["id"])
    lines = [
        "# Eval set — held out",
        "",
        "Build-time only. Never read at skill runtime, never named by SKILL.md, and never",
        "shown to an agent that is about to predict a rating. The whole accuracy claim",
        "rests on these 24 problems being unseen by the rubric.",
        "",
        "| slot | id | rating | band |",
        "|---|---|---|---|",
    ]
    for n, row in enumerate(evals, 1):
        lines.append("| eval-%02d | %s | %s | %s |" % (n, row["id"], row["rating"], row["band"]))
    EVAL.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print("split: %d anchors, %d eval -> %s" % (len(rows) - len(evals), len(evals), EVAL))


def read_eval():
    if not EVAL.exists():
        sys.exit("%s does not exist — run `split` first" % EVAL)
    rows = []
    for line in EVAL.read_text(encoding="utf-8").splitlines():
        if not line.startswith("| eval-"):
            continue
        cells = [c.strip() for c in line.strip("|").split("|")]
        rows.append(dict(zip(["slot", "id", "rating", "band"], cells)))
    return rows


def cmd_blind():
    out = CACHE / "blind"
    out.mkdir(parents=True, exist_ok=True)
    for row in read_eval():
        src = CACHE / ("%s.txt" % row["id"])
        if not src.exists():
            sys.exit("missing %s — run `fetch` first" % src)
        body = src.read_text(encoding="utf-8")
        # Drop the leading "D. Problem Title" line so the slot cannot be traced by name.
        body = body.split("\n", 1)[1].lstrip() if "\n" in body else body
        (out / ("%s.txt" % row["slot"])).write_text(body, encoding="utf-8")
    print("wrote %d blind statements to %s" % (len(read_eval()), out))


def cmd_check():
    rows = read_corpus()
    failures = []
    if len(rows) != len(BANDS) * PER_BAND:
        failures.append("corpus has %d rows, expected %d" % (len(rows), len(BANDS) * PER_BAND))
    anchors = {r["id"] for r in rows if r["role"] == "anchor"}
    evals = {r["id"] for r in rows if r["role"] == "eval"}
    unassigned = [r["id"] for r in rows if r["role"] not in ("anchor", "eval")]
    if unassigned:
        failures.append("unassigned rows: %s" % " ".join(unassigned))
    overlap = anchors & evals
    if overlap:
        failures.append("CONTAMINATION: %s in both sets" % " ".join(sorted(overlap)))
    for band in sorted({r["band"] for r in rows}):
        n_eval = sum(1 for r in rows if r["band"] == band and r["role"] == "eval")
        if n_eval != EVAL_PER_BAND:
            failures.append("band %s has %d eval, expected %d" % (band, n_eval, EVAL_PER_BAND))
    if ANCHORS.exists():
        text = ANCHORS.read_text(encoding="utf-8")
        leaked = sorted(e for e in evals if re.search(r"\|\s*%s\s*\|" % re.escape(e), text))
        if leaked:
            failures.append("CONTAMINATION: eval ids present in anchors.md: %s" % " ".join(leaked))
    if EVAL.exists():
        listed = {r["id"] for r in read_eval()}
        if listed != evals:
            failures.append("eval-set.md lists %d ids, corpus marks %d" % (len(listed), len(evals)))
    for failure in failures:
        print("FAIL  %s" % failure)
    if failures:
        sys.exit(1)
    print("anchors: %d   eval: %d   disjoint: yes" % (len(anchors), len(evals)))
    print("ALL CHECKS PASSED")
```

Extend the dispatch table:

```python
VERBS = {"sample": cmd_sample, "starter": cmd_starter, "fetch": cmd_fetch,
         "split": cmd_split, "blind": cmd_blind, "check": cmd_check}
```

- [ ] **Step 3: Prove `check` actually fails on contamination**

A guard that has never failed is not known to work. Run:

Back the file up to your session scratchpad (not into the repo, where `git status` would show it), duplicate one row, and confirm the guard trips:

```bash
cp skills/calculating-difficulties/calibration/corpus.md "$SCRATCH/corpus.backup.md"
python - <<'PY'
from pathlib import Path
p = Path("skills/calculating-difficulties/calibration/corpus.md")
lines = p.read_text(encoding="utf-8").splitlines()
rows = [l for l in lines if l.startswith("| ") and not l.startswith("| id ")
        and not set(l) <= set("|- ")]
lines.append(rows[0])          # the same problem id twice
p.write_text("\n".join(lines) + "\n", encoding="utf-8")
print("injected a duplicate of:", rows[0])
PY
python skills/calculating-difficulties/calibration/fetch-corpus.py check
```
Expected: `FAIL corpus has 81 rows, expected 80`, a `FAIL unassigned rows:` line, and a non-zero exit. Set `SCRATCH` to the scratchpad directory named in your environment first.

Restore:

```bash
cp "$SCRATCH/corpus.backup.md" skills/calculating-difficulties/calibration/corpus.md
python skills/calculating-difficulties/calibration/fetch-corpus.py check
```
Expected after restoring: `FAIL  unassigned rows: ...` and a non-zero exit — still failing, because the split has not run yet. That is correct; Step 4 is what makes it pass.

- [ ] **Step 4: Split, blind, check**

```bash
python skills/calculating-difficulties/calibration/fetch-corpus.py split
python skills/calculating-difficulties/calibration/fetch-corpus.py blind
python skills/calculating-difficulties/calibration/fetch-corpus.py check
```
Expected: `split: 56 anchors, 24 eval -> ...eval-set.md`; `wrote 24 blind statements`; then `anchors: 56   eval: 24   disjoint: yes` and `ALL CHECKS PASSED`.

- [ ] **Step 5: Verify the split is deterministic and the blind copies are anonymous**

```bash
python skills/calculating-difficulties/calibration/fetch-corpus.py split
head -3 .cache-cf-corpus/blind/eval-01.txt
grep -rl "eval-01" skills/calculating-difficulties/references/ || echo "no slot names in references — good"
```
Expected: the second `split` refuses (roles already assigned); `eval-01.txt` starts with statement prose, **not** a `D. Title` line; and the third prints the `good` message.

- [ ] **Step 6: Commit**

```bash
git add skills/calculating-difficulties/calibration/fetch-corpus.py skills/calculating-difficulties/calibration/corpus.md skills/calculating-difficulties/calibration/eval-set.md
git commit -m "data: split the corpus 56/24 and add the contamination guard"
```

- [ ] **Step 7: Update the ledger** — Task 12 `done`, **Where we are** → `Task 12 done. Next: Task 13 (summarize 56 anchors).`, amend.

---

### Task 13: Summarize the 56 anchors — subagents

The first task that spends real tokens. ~6 subagents, each handling 9-10 statements.

**Files:**
- Modify: `skills/calculating-difficulties/references/anchors.md` (replaces the 8-row starter wholesale)

**Interfaces:**
- Consumes: `corpus.md` role column, `.cache-cf-corpus/<id>.txt`.
- Produces: the 56-row anchor table Pass C reads and Tasks 15/17 measure.

- [ ] **Step 1: List the anchor ids**

```bash
grep "^| [0-9]" skills/calculating-difficulties/calibration/corpus.md | awk -F'|' '$7 ~ /anchor/ {print $2, $3, $4}' | tr -d ' ' | head -60
```
Expected: 56 lines of `id|rating|div`. Split them into 6 batches of 9-10.

- [ ] **Step 2: Dispatch one subagent per batch**

For each batch, dispatch a subagent with this prompt, substituting the batch's rows:

```
You are summarizing Codeforces problems for a difficulty-calibration anchor table.
Work only from the local files; do not fetch anything from the network.

For each problem below, read .cache-cf-corpus/<id>.txt and produce one table row:

| <id> | <rating> | <div> | <prereq> | <one sentence naming the intended solution> |

Rules:
- <prereq> is ONE tag copied from the left column of
  skills/calculating-difficulties/references/tag-floors.md, choosing the hardest
  technique the intended solution genuinely requires. If none applies, write "none".
- The sentence names the actual algorithm and the key observation, in under 25 words.
  "Use DP" is useless; "DP over digits carrying sum mod 10 and a tight flag" is useful.
- The true rating is given to you. Use it as a cross-check: if your reading of the
  problem is wildly inconsistent with its rating, say so by appending "  <!-- unsure -->"
  to that row rather than adjusting your summary to fit the number.
- Output ONLY the table rows, one per line, no preamble.

Problems:
<id> <rating> <div>
...
```

- [ ] **Step 3: Assemble `anchors.md`**

Replace the table in `skills/calculating-difficulties/references/anchors.md` with all 56 rows, sorted by rating then id. Keep the header prose from Task 5 and replace the `Provisional set` line with:

```markdown
**56 anchors**, 7 per 200-point band from 1100 to 2699, drawn from the frozen corpus in
`calibration/corpus.md`. Ratings come from the Codeforces API and are labels, not
estimates. Rows marked `<!-- unsure -->` had a summary the summarizer could not reconcile
with the true rating; prefer another anchor when one is available.
```

- [ ] **Step 4: Verify the count, the tags, and the contamination guard**

```bash
grep -c "^| [0-9]" skills/calculating-difficulties/references/anchors.md
grep "^| [0-9]" skills/calculating-difficulties/references/anchors.md | awk -F'|' '{print $5}' | sort | uniq -c | sort -rn
python skills/calculating-difficulties/calibration/fetch-corpus.py check
```
Expected: `56`; a tag distribution with no single tag holding more than ~20 rows (a heavy skew means the summarizers defaulted to one label — re-run that batch); and `ALL CHECKS PASSED`, which now also asserts that **no eval id leaked into `anchors.md`**.

- [ ] **Step 5: Commit**

```bash
git add skills/calculating-difficulties/references/anchors.md
git commit -m "data: replace the starter anchors with 56 calibrated anchors"
```

- [ ] **Step 6: Update the ledger** — Task 13 `done`, cost `6 agents`, **Where we are** → `Task 13 done. Next: Task 14 (metrics verb + baseline).`, amend.

---

### Task 14: The `metrics` verb and the Phase 0 baseline

The baseline runs on the **same 24 problems** as every later round, by agents that have never seen the skill. Same problems with and without the rubric is a comparison; different problems is not.

**Files:**
- Modify: `skills/calculating-difficulties/calibration/fetch-corpus.py`
- Create: `skills/calculating-difficulties/calibration/predictions-baseline.md`

**Interfaces:**
- Consumes: `eval-set.md`, `.cache-cf-corpus/blind/`.
- Produces: the `metrics` verb and the predictions format `| slot | predicted |` that Tasks 15 and 17 reuse.

- [ ] **Step 1: Write the failing check**

```bash
python skills/calculating-difficulties/calibration/fetch-corpus.py metrics
```
Expected now: the usage line and a non-zero exit.

- [ ] **Step 2: Add the `metrics` verb**

Add to `fetch-corpus.py`, before `VERBS`:

```python
def cmd_metrics():
    if len(sys.argv) != 3:
        sys.exit("usage: fetch-corpus.py metrics <predictions-file.md>")
    truth = {r["slot"]: int(r["rating"]) for r in read_eval()}
    bands = {r["slot"]: r["band"] for r in read_eval()}
    path = Path(sys.argv[2])
    preds = {}
    for line in path.read_text(encoding="utf-8").splitlines():
        if not line.startswith("| eval-"):
            continue
        cells = [c.strip() for c in line.strip("|").split("|")]
        preds[cells[0]] = int(cells[1])
    missing = sorted(set(truth) - set(preds))
    if missing:
        sys.exit("predictions missing for: %s" % " ".join(missing))
    errors = {s: preds[s] - truth[s] for s in truth}
    n = len(errors)
    mae = sum(abs(e) for e in errors.values()) / n
    bias = sum(errors.values()) / n
    within200 = 100.0 * sum(1 for e in errors.values() if abs(e) <= 200) / n
    within300 = 100.0 * sum(1 for e in errors.values() if abs(e) <= 300) / n
    print("## %s" % path.stem)
    print()
    print("n = %d   MAE = %.0f   bias = %+.0f   within200 = %.0f%%   within300 = %.0f%%"
          % (n, mae, bias, within200, within300))
    print()
    print("| band | n | MAE | bias |")
    print("|---|---|---|---|")
    for band in sorted({bands[s] for s in truth}):
        slots = [s for s in truth if bands[s] == band]
        b_mae = sum(abs(errors[s]) for s in slots) / len(slots)
        b_bias = sum(errors[s] for s in slots) / len(slots)
        print("| %s | %d | %.0f | %+.0f |" % (band, len(slots), b_mae, b_bias))
    print()
    print("| slot | true | predicted | error |")
    print("|---|---|---|---|")
    for slot in sorted(truth):
        print("| %s | %d | %d | %+d |" % (slot, truth[slot], preds[slot], errors[slot]))
```

Extend the dispatch table with `"metrics": cmd_metrics`, and relax the argument check at the bottom of the file:

```python
if __name__ == "__main__":
    if len(sys.argv) < 2 or sys.argv[1] not in VERBS:
        sys.exit("usage: fetch-corpus.py {%s}" % "|".join(VERBS))
    VERBS[sys.argv[1]]()
```

- [ ] **Step 3: Run the baseline — 5 subagents, no skill**

Split the 24 slots into 5 batches. Dispatch each with:

```
Estimate the Codeforces difficulty rating of each problem below.

For each slot, read .cache-cf-corpus/blind/<slot>.txt — that statement text is
everything you get. Do not fetch anything from the network, do not try to identify
which Codeforces problem it is, and do not read any other file in this repository.

Give your honest free-form estimate on the Codeforces scale (800-3500), rounded to
the nearest 100. Judge the full problem as a single all-or-nothing problem in a
modern Div1/Div2 round.

Output ONLY these rows, nothing else:

| <slot> | <rating> |

Slots: eval-01 eval-02 ...
```

**Do not tell them a rubric exists, and do not give them any true rating.**

- [ ] **Step 4: Record and score**

Write the 24 rows to `skills/calculating-difficulties/calibration/predictions-baseline.md` under this header:

```markdown
# Predictions — Phase 0 baseline (no rubric)

Free-form estimates from agents with no access to SKILL.md, the floors, or the anchors.
This is the number the calibrated skill must beat.

| slot | predicted |
|---|---|
```

Run:
```bash
python skills/calculating-difficulties/calibration/fetch-corpus.py metrics skills/calculating-difficulties/calibration/predictions-baseline.md
```
Expected: a block reporting `n = 24` with an MAE. Record whatever it is — a *good* baseline is a real finding, not a problem.

- [ ] **Step 5: Commit**

```bash
git add skills/calculating-difficulties/calibration/fetch-corpus.py skills/calculating-difficulties/calibration/predictions-baseline.md
git commit -m "data: add the metrics verb and the Phase 0 baseline"
```

- [ ] **Step 6: Update the ledger** — Task 14 `done`, cost `5 agents`, record the baseline MAE in the row, **Where we are** → `Task 14 done. Next: Task 15 (blind eval, round 1).`, amend.

---

### Task 15: Blind eval of rubric v0

**Files:**
- Create: `skills/calculating-difficulties/calibration/predictions-round1.md`

**Interfaces:**
- Consumes: `SKILL.md` Passes A-E, `references/*`, the blind statements.
- Produces: round 1 predictions for Task 16.

- [ ] **Step 1: Dispatch 5 subagents, each with 4-5 slots**

```
Apply a difficulty-rating rubric to each problem below.

Read these three files first:
  skills/calculating-difficulties/SKILL.md
  skills/calculating-difficulties/references/tag-floors.md
  skills/calculating-difficulties/references/anchors.md

For each slot, read .cache-cf-corpus/blind/<slot>.txt and apply Passes A through E.
There is no solution file for these problems — derive the intended solution from the
statement yourself, then rate it.

Hard rules:
- Do not fetch anything from the network and do not try to identify the problem.
- Do not open anything under skills/calculating-difficulties/calibration/. That
  directory holds the true ratings; reading it destroys the measurement.
- Do not skip Pass A. Write the features before any number.

Output ONLY these rows:

| <slot> | <rating> | <floor> | <anchor ids used> |

Slots: eval-01 ...
```

- [ ] **Step 2: Record the predictions**

Write `skills/calculating-difficulties/calibration/predictions-round1.md`:

```markdown
# Predictions — round 1 (rubric v0)

Blind: the predicting agents saw the statement text and the runtime references, never
a true rating and never the calibration directory.

| slot | predicted | floor | anchors used |
|---|---|---|---|
```

The `metrics` verb reads column 2 and ignores the rest.

- [ ] **Step 3: Verify blindness held**

```bash
grep -c "^| eval-" skills/calculating-difficulties/calibration/predictions-round1.md
```
Expected: `24`.

Then read the subagent transcripts. If any agent reported reading `calibration/`, or named a real Codeforces problem id for a slot, **discard that batch and re-run it**. A contaminated batch cannot be repaired by adjusting its numbers.

- [ ] **Step 4: Commit**

```bash
git add skills/calculating-difficulties/calibration/predictions-round1.md
git commit -m "data: blind eval of rubric v0"
```

- [ ] **Step 5: Update the ledger** — Task 15 `done`, cost `5 agents`, **Where we are** → `Task 15 done. Next: Task 16 (metrics.md).`, amend.

---

### Task 16: `metrics.md` — baseline and round 1

**Files:**
- Create: `skills/calculating-difficulties/calibration/metrics.md`

**Interfaces:**
- Consumes: the `metrics` verb, both predictions files.
- Produces: the file Task 17 appends to and Task 18 quotes.

- [ ] **Step 1: Generate both blocks**

```bash
{
  echo "# Metrics"
  echo
  echo "Every round is appended, never overwritten: the comparison between rounds is the"
  echo "only evidence that tuning helped. A 24-problem eval set carries roughly ±40"
  echo "standard error — a movement smaller than that is not an improvement."
  echo
  echo "Targets: MAE ≤ 200, ≥65% within ±200, |bias| ≤ 75."
  echo
  python skills/calculating-difficulties/calibration/fetch-corpus.py metrics skills/calculating-difficulties/calibration/predictions-baseline.md
  echo
  python skills/calculating-difficulties/calibration/fetch-corpus.py metrics skills/calculating-difficulties/calibration/predictions-round1.md
} > skills/calculating-difficulties/calibration/metrics.md
```

- [ ] **Step 2: Read the result and decide**

Check three things, in this order:

1. **Did the rubric beat the baseline?** If round 1's MAE is not at least 40 better than the baseline's, the anchors are not doing their job — say so in the process doc's **Deviations** section before continuing. It is a finding, not a failure to hide.
2. **Signed bias.** A large `|bias|` is the cheapest thing to fix (Task 17, one constant).
3. **Error by band.** Over-rating easy problems and under-rating hard ones is the characteristic rubric failure; it shows as a bias that flips sign across the bands.

- [ ] **Step 3: Verify the file has both rounds**

```bash
grep -c "^## predictions-" skills/calculating-difficulties/calibration/metrics.md
```
Expected: `2`.

- [ ] **Step 4: Commit**

```bash
git add skills/calculating-difficulties/calibration/metrics.md
git commit -m "data: record baseline and round 1 metrics"
```

- [ ] **Step 5: Update the ledger** — Task 16 `done`, record both MAEs in the row, **Where we are** → `Task 16 done. Next: Task 17 (tuning).`, amend.

---

### Task 17: Tune — bias first, then band compression

**Hard cap: 3 rounds total including round 1.** Stop at the cap even if the number is still moving; below the ±40 error bar you are fitting noise.

**Files:**
- Modify: `skills/calculating-difficulties/references/tag-floors.md`
- Modify: `skills/calculating-difficulties/SKILL.md` (only if Pass D changes)
- Create: `skills/calculating-difficulties/calibration/predictions-round2.md`, and `-round3.md` if a third round is justified
- Modify: `skills/calculating-difficulties/calibration/metrics.md`

**Interfaces:**
- Consumes: `metrics.md` round 1.
- Produces: rubric v1 — the floors Task 18 freezes.

- [ ] **Step 1: Round 2 — remove the bias**

If round 1's `|bias| > 75`, shift **every** floor in `tag-floors.md` by `-round(bias/100)*100`. One constant, applied to the whole column; do not re-order rows and do not tune individual floors. A single parameter that improves every band at once is worth more than eight parameters fitted to 24 data points.

If `|bias| ≤ 75` already, skip to Step 2 and use round 2 for band compression instead.

Re-run the eval exactly as Task 15 Step 1 (fresh subagents, same prompt, same slots), write `predictions-round2.md` with the round-1 header wording adjusted to `round 2`, then:

```bash
python skills/calculating-difficulties/calibration/fetch-corpus.py metrics skills/calculating-difficulties/calibration/predictions-round2.md >> skills/calculating-difficulties/calibration/metrics.md
```

- [ ] **Step 2: Round 3 — band compression, only if it earns its place**

Read the per-band table. If the bands at one end are systematically off in one direction while the other end is off in the other, the rubric is compressing the scale. Widen Pass C's window for the affected end — e.g. change `[floor, floor+600]` to `[floor, floor+800]` for floors at or above 2100 — and state the change in `SKILL.md` Pass C.

Run round 3 only if round 2 moved MAE by more than 40. If it did not, **stop**: two consecutive rounds inside the error bar is the spec's stop condition, and it has been met.

- [ ] **Step 3: Verify the rounds are appended, not overwritten**

```bash
grep -c "^## predictions-" skills/calculating-difficulties/calibration/metrics.md
```
Expected: `3` after round 2, `4` after round 3. A count that did not grow means `>` was used instead of `>>` — recover the lost round from git history and redo the append.

- [ ] **Step 4: Commit each round separately**

```bash
git add skills/calculating-difficulties/references/tag-floors.md skills/calculating-difficulties/calibration/
git commit -m "tune: round 2 — remove signed bias by shifting the floors"
```

- [ ] **Step 5: Update the ledger** — Task 17 `done` with the number of rounds actually run and the final MAE/bias, **Where we are** → `Task 17 done. Next: Task 18 (freeze).`, amend.

---

### Task 18: Freeze — the measured numbers replace the claim

**Files:**
- Modify: `skills/calculating-difficulties/SKILL.md` (the `## Calibration status` section, and Pass E's interval)
- Modify: `docs/superpowers/process/calculating-difficulties.md`

**Interfaces:**
- Consumes: the final block in `metrics.md`.
- Produces: the shipped skill.

- [ ] **Step 1: Write the check that must fail now**

```bash
grep -c "Uncalibrated" skills/calculating-difficulties/SKILL.md
```
Expected now: `1`. At the end of this task it must be `0`.

- [ ] **Step 2: Choose the interval from the measured MAE**

Read the final round's line in `metrics.md` and apply the band rule from Global Constraints:

| Measured MAE | Emit |
|---|---|
| ≤ 200 | `2300 ± 200 (ước lượng)` |
| ≤ 300 | `2300 ± 300 (ước lượng)` |
| > 300 | a range only: `2200-2600 (ước lượng)` |

- [ ] **Step 3: Replace the calibration status**

Replace the whole `## Calibration status` section with, substituting the real figures:

```markdown
## Calibration status

Calibrated against 56 anchors drawn from rated Div1/Div2 problems (2018 onward), and
measured blind on 24 held-out problems from the same sample.

| | value |
|---|---|
| MAE | <measured> |
| within ±200 | <measured>% |
| signed bias | <measured> |
| eval n | 24 |
| baseline MAE (no rubric) | <measured> |
| frozen | <YYYY-MM-DD> |

Emit the estimate as `<the interval chosen in Step 2>`. A 24-problem eval set carries
roughly ±40 standard error, so treat these figures as accurate to about that much and no
better. Codeforces ratings themselves quantize to 100 and carry about ±150 of inherent
noise: no method places a problem more precisely than that.
```

If the MAE missed the target, add one sentence saying so plainly — the reader is entitled to know the interval was widened rather than earned.

- [ ] **Step 4: Update Pass E's interval instruction**

In Pass E, replace `3. Attach the interval from `Calibration status` below.` with the concrete instruction matching Step 2, e.g. `3. Attach `± 200` — the measured interval from `Calibration status` below.` Also update the output template's `**Expected rating: 2300 ± 200 (ước lượng)**` line and the `<calibration status line>` placeholder in the source footer to the real wording, e.g. `hiệu chuẩn 2026-09-XX, MAE 190, n=24`.

- [ ] **Step 5: Verify nothing uncalibrated survives, and the boundary still holds**

```bash
grep -c "Uncalibrated\|chưa hiệu chuẩn" skills/calculating-difficulties/SKILL.md
grep -c "calibration/" skills/calculating-difficulties/SKILL.md
grep -rc "eval-set\|predictions-" skills/calculating-difficulties/SKILL.md skills/calculating-difficulties/references/
python skills/calculating-difficulties/calibration/fetch-corpus.py check
```
Expected: `0`, `0`, `0` for every file listed, and `ALL CHECKS PASSED`. The middle two prove the runtime half still cannot see the build-time half — the property the whole directory split exists for.

- [ ] **Step 6: Re-run the smoke test against the frozen skill**

Repeat Task 10 Steps 1-3 with the frozen `SKILL.md` and the 56-anchor table. Expected this time: Pass C finds real anchors in `[1900, 2500]` and **does not** report a weak window. The estimate should land in the 1900-2300 range for that digit-DP problem; a number outside `[1900, 2600]` means the floor or the anchors are misapplied — investigate before shipping.

- [ ] **Step 7: Close out the process doc**

Set every ledger row to `done`, set **Where we are** to `Complete. Frozen <date>, MAE <x>, n=24.`, and add a short **Deviations** entry for anything that differed from this plan.

- [ ] **Step 8: Commit**

```bash
git add skills/calculating-difficulties/ docs/superpowers/process/calculating-difficulties.md
git commit -m "feat: freeze calculating-difficulties with measured accuracy"
```

---

## Verification summary

| Property | Proved by | Task |
|---|---|---|
| The skill exists and is discoverable | frontmatter `name:` matches the directory | 3 |
| Runtime never sees the eval set | `grep -c "calibration/" SKILL.md` = 0 | 3, 18 |
| Anchors and eval set are disjoint | `check` verb, run three times | 12, 13, 18 |
| The split is reproducible | second `split` refuses; fixed seed | 12 |
| Blind means blind | slot-named, title-stripped statements + transcript review | 12, 15 |
| The editorial no longer invents a rating | `grep -c "when it can be responsibly estimated"` = 0 | 7 |
| The skill actually runs | end-to-end smoke test, both branches | 10, 18 |
| The accuracy claim is measured, not asserted | `metrics.md` with a baseline to compare against | 14, 16 |
| Expensive work is not repeated | `sample`/`split` refuse re-runs; process-doc ledger | 11, 12, 1 |
