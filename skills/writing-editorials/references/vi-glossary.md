# Vietnamese editorial glossary

Canonical wording for `writing-editorials`. Read this before writing
`outputs/editorial.html`. If the user corrects a term, update **this file** in
the same turn; do not only patch the HTML.

Do not translate word for word. Use the Vietnamese that olympiad students
actually say. **Compound nouns keep Vietnamese order** (modifier first):

| English | Use | Never |
|---|---|---|
| simple graph | **đơn đồ thị** | đồ thị đơn |
| multigraph | **đa đồ thị** | đồ thị đa |

Same pattern: đơn đồ thị vô hướng, đồ thị có hướng, cây có gốc — not
"đồ thị đơn vô hướng".

---

## Page chrome is Vietnamese

Reader-facing labels are Vietnamese except the three header metadata rows,
which stay English: `Time limit`, `Memory limit`, and `Expected rating`. The
rating is a plain number with no leading `*`.

| Slot | Use |
|---|---|
| bar kicker, `<title>` suffix, footer | Lời giải |
| time limit | **Time limit** (English) |
| memory limit | **Memory limit** (English) |
| expected rating | **Expected rating** (English) |
| tags | Thẻ (the tag strings themselves stay CF-English) |
| statement section | Tóm tắt đề bài |
| solution section | Lời giải |
| complexity section | Độ phức tạp |
| other approaches | Cách giải khác |
| better solution found in the AC suite | Lời giải tốt hơn |
| observation card | Nhận xét |
| lemma card | Bổ đề |
| fun fact (only when the user asked) | Thông tin thú vị |

Tag names stay CF-English: `graphs`, `data structures`, `dp`,
`dfs and similar`, …

---

## Keep in English

Technique, structure, and algorithm names stay in their canonical English form
inside a Vietnamese sentence. Explain their problem-specific role in
Vietnamese; do not add a general tutorial for a standard technique.

### Bitwise / algebra tokens

XOR, AND, OR, NOT, mask, bit, bitmask, LSB, MSB, popcount, modulo

### Techniques and structures

DP, DFS, BFS, DSU, BCC, LCA, RMQ, HLD, MST, SCC, DAG, FFT, NTT
segment tree, BIT, Fenwick, trie, binary heap, sparse table, binary lifting
virtual tree, centroid, Euler tour
sort, brute force, greedy, two pointers, binary search, divide and conquer,
convex hull trick, CDQ
Tarjan, Kruskal, Dijkstra, Floyd–Warshall, KMP, Z-function
handshake lemma, Sprague–Grundy
amortized, non-amortized

### Graph words that stay English in a writeup

bridge, biconnected component, 2-edge-connected, 2-edge-cut, cut vertex, block

Example: "tính bridge bằng Tarjan DFS", not "tính cầu".

`Subtask` also stays English as a word; visible names are `Subtask 1`,
`Subtask 2`, … never \(g_1\) or `g1` in prose. An HTML `id` may stay `g1`.

---

## Translate

### Statement labels

| English | Vietnamese |
|---|---|
| requirement (end of restatement, **above** constraints) | **Yêu cầu** |
| constraints | **Giới hạn** |
| subtask list label | **Subtask** |
| points (subtask score) | điểm |
| no additional constraints | không ràng buộc thêm |
| subtask summary (table heading) | Tóm tắt subtask |
| insight (table heading) | Ý tưởng |

Order inside `Tóm tắt đề bài`: restatement → **Yêu cầu** → **Giới hạn** →
**Subtask**. `Yêu cầu` is the last line of the restatement, immediately above
`Giới hạn`; it does not come after the subtask list.

### Graphs

| English | Vietnamese |
|---|---|
| simple graph | đơn đồ thị |
| multigraph | đa đồ thị |
| undirected graph | đồ thị vô hướng |
| undirected simple graph | đơn đồ thị vô hướng |
| directed graph | đồ thị có hướng |
| weighted graph | đồ thị có trọng số |
| complete graph | đồ thị đầy đủ |
| bipartite graph | đồ thị hai phía |
| vertex / node | đỉnh |
| edge | cạnh |
| loop / self-loop | khuyên |
| parallel edges | cạnh kép |
| adjacent | kề |
| neighborhood | láng giềng |
| connected | liên thông |
| connected component | thành phần liên thông |
| disconnected | không liên thông |
| path | đường (đường đi) |
| shortest path | đường đi ngắn nhất |
| cycle | chu trình |
| acyclic | không chu trình |
| degree | bậc |
| even / odd degree | bậc chẵn / bậc lẻ |
| isolated vertex | đỉnh cô lập |
| leaf | lá |
| spanning tree | cây khung |
| matching | cặp ghép |
| cut (as in min-cut) | lát cắt |
| flow | luồng |
| topological order | thứ tự topo |
| strongly connected | liên thông mạnh |

### Trees

| English | Vietnamese |
|---|---|
| tree | cây |
| forest | rừng |
| rooted tree | cây có gốc |
| root | gốc |
| parent | cha |
| child | con |
| ancestor | tổ tiên |
| descendant | con cháu |
| subtree | cây con |
| depth / layer | độ sâu / lớp |
| height | chiều cao |
| binary tree | cây nhị phân |

### Games / I/O

| English | Vietnamese |
|---|---|
| two players | hai người chơi |
| first / second player | người đi trước / người đi sau |
| turn | lượt |
| optimal play | chơi tối ưu |
| winner / loser | người thắng / người thua |
| winning / losing position | thế thắng / thế thua |
| print | in |
| count | đếm |
| any valid answer | bất kỳ đáp án hợp lệ |
| multiple answers | nhiều đáp án |

### Numbers / combinatorics

| English | Vietnamese |
|---|---|
| even / odd | chẵn / lẻ |
| strictly increase | tăng nghiêm ngặt |
| pairwise | từng cặp |
| permutation | hoán vị |
| combination | tổ hợp |
| subset | tập con |
| prefix / suffix | tiền tố / hậu tố |
| cost | chi phí |
| paint / color (a vertex) | tô màu |
| query | truy vấn (the noun; `query` as a technique name stays English) |
| state | trạng thái |
| invariant | bất biến |
| answer | đáp án |

Add a row here whenever a problem forces you to invent a gloss. Keep one
canonical gloss per English word.
