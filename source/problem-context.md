**Problem name**: Sự cân bằng
**Time limit**: 3s

**Legend**:
The Kinkou temple garden holds stone lanterns joined by raked gravel paths. Shen and Zed have agreed to settle one thing here, without blades.
The garden is a simple undirected graph with 𝑁 vertices and 𝑀 edges. The lanterns are the vertices, numbered 1 through 𝑁, and the paths are the edges. Every edge joins two different vertices, and no two edges join the same pair of vertices.
Zed arrives early and rakes over the paths he dislikes. He may also rake nothing at all. 
First, Zed deletes a set of edges of his choice. Any subset of the 𝑀 edges may be deleted, and this set may be empty, so this move is always possible, in particular when 𝑀 = 0. Deleting an edge removes that edge only: no vertex is deleted by this move, and all 𝑁 vertices remain. Zed does this once, before any vertex is deleted, and never deletes an edge again afterwards.
Then the two take turns lifting lanterns. Shen goes first; balance is a law, he says, not a kindness.
After that, the players alternately delete vertices, and Shen moves first. At any moment the current graph consists of the vertices that have not been deleted yet, together with the edges that were neither deleted by Zed nor removed along with a vertex. The degree of a vertex is the number of edges of the current graph incident to it.
On their turn, a player chooses one vertex of the current graph whose degree in the current graph is even, and deletes it; every edge incident to that vertex is deleted together with it. A vertex of odd degree may not be chosen. Degree 0 counts as even, so a vertex with no incident edge is always a legal choice.
Exactly one vertex is deleted per turn, and a player who has a legal choice must make one: passing is not allowed.
The duellist left with nothing he may lift has lost, and Zed means it to be Shen.
A player who has no legal vertex to delete when their turn begins loses, and the other player wins; this includes the case where no vertex is left at all. Both players play optimally.
Task: decide which of the two players wins the game described above, assuming both play optimally, and print that player.

**Input format**:
The first line contains two integers 𝑁 and 𝑀 — the number of vertices and the number of edges.
Each of the next 𝑀 lines contains two integers 𝑈 and 𝑉 — an edge joining vertex 𝑈 and vertex 𝑉.
The two endpoints of an edge may be given in either order, and each unordered pair of vertices appears at most once over the 𝑀 lines.

**Output format**:
Print one line containing a single word: Shen if Shen wins, or Zed if Zed wins.