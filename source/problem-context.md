**Legend**:
The vents under Zaun breathe warm chem-fume into Viktor's workshop, and the
night's work is already laid out on the bench: a row of hex-cores, each one a
coil wound tight around a cell of charge.

The bench holds $N$ hex-cores, numbered $1$ through $N$. Core $i$ carries a
coil of power $A_i$ and a cell holding a charge $B_i$. The \emph{output} of the
bench is
\[
  A_1 \cdot B_1 + A_2 \cdot B_2 + \cdots + A_N \cdot B_N .
\]

Tonight Viktor performs a single evolution, \textbf{exactly once} --- it is
not optional. He chooses two cores, floods the greater one's cell with fresh
reagent (setting its charge to $X$), and makes the two coils trade places.
The other core must be strictly lesser in both coil and charge.

Formally, choose an \emph{ordered} pair of distinct cores $(U, V)$ such that
\[
  A_V < A_U \qquad\text{and}\qquad B_V < B_U ,
\]
both comparisons made on the values as given in the input. Then, in one step:
\begin{itemize}
  \item the coil powers of $U$ and $V$ are swapped;
  \item $B_U$ is set to $X$.
\end{itemize}
Nothing else changes: $B_V$ is left as it was, and no other core is touched.
The output is then recomputed from the new values. At least one such pair
$(U, V)$ is guaranteed to exist.

Viktor wants the greatest output after this one operation. That value may be
smaller than the output the bench gives now.

\textbf{Task:} print the largest output achievable after performing the
operation exactly once.

**Input format**:
\begin{itemize}
  \item The first line contains two integers $N$ and $X$ --- the number of
        hex-cores and the charge that a flooded cell is left with.
  \item The second line contains $N$ integers $A_1, A_2, \ldots, A_N$ --- the
        coil powers of the cores.
  \item The third line contains $N$ integers $B_1, B_2, \ldots, B_N$ --- the
        charges of the cores.
\end{itemize}

**Output format**:
Print one integer --- the largest output of the bench achievable after
performing the operation exactly once.