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
- the count of **independent** non-obvious observations. Two restatements of one idea count once. A step that follows routinely once the main technique is chosen is not a separate observation, however separable it looks — reducing a range query to two prefix queries, or compressing a state to its residue, is scaffolding, not insight. Count an observation only if a solver could have the main idea and still fail to find it;
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

**Two categories are enough when the third cannot exist.** The window starts at the floor, so a problem that genuinely sits at its floor has nothing easier to compare against — that is the floor working, not a gap to paper over. Fill the categories the window supports, name the missing one in the output, and do not reach below the floor or invent a comparison to fill a slot.

**Trust the anchor's rating over your own sense of difficulty.** The characteristic failure of this pass is treating the anchors as a sanity check on a number you already formed — which leaves the scale compressed, easy problems rated too high and hard ones too low. Read the anchor's rating first, decide whether this problem is harder or easier than *that specific problem*, and let the number follow from the comparison. If your estimate ends more than `300` away from every anchor you compared against, you did not place the problem against them — redo the comparison rather than keeping the number.

State the estimate this placement implies. If no anchor in the window is comparable, widen to `[floor, floor+800]` and say in the output that the placement was weak. Widening searches upward only; it can never supply an easier anchor, so never widen for that reason.

## Pass D — adjustments, capped at ±300

Apply only what Pass A actually found:

| Adjustment | When |
|---|---|
| `+100` each, cap `+200` | each independent insight beyond the first |
| `+100` | the statement disguises a standard object |
| `+100` | genuinely heavy implementation, not merely long |
| `-100` to `-200` | a textbook exercise in its technique. `-100` is the default; charge `-200` only when the problem is the technique's bare demonstration case, with no secondary constraint and nothing combined on top |
| `-100` | constraint leakage — a bound that names the technique by itself, readable from the constraints line alone without the legend, as `n ≤ 20` broadcasts "bitmask". A bound that merely rules out brute force is not leakage |

**The total is capped at ±300**, and that cap is the point: deltas must never override the anchors. If the deltas want more than ±300, the anchor placement in Pass C was wrong — redo Pass C instead of raising the cap.

## Pass E — gates and output

1. **If Pass D pushed the number below the Pass B floor, raise it back to that floor.** The floor binds the final answer, not merely the anchor placement — otherwise a negative adjustment reopens exactly the failure Pass B exists to close. When this fires, say so in `Độ tin cậy`: the adjustments disagreed with the floor, and the floor won.
2. Round to the nearest `100`.
3. Clamp to `[800, 3500]`.
4. Attach the interval from `Calibration status` below.
5. Write `outputs/difficulty.md`.

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
