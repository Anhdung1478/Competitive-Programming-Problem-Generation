**Legend**:
Có $N$ người được đánh số hiệu từ $1$ đến $N$ đang chuẩn bị xếp thành một hàng dọc.

Để việc xếp hàng diễn ra theo đúng quy định, có $M$ yêu cầu được đưa ra. Yêu cầu thứ $i$ được mô tả bằng một cặp số $(a_i, b_i)$, mang ý nghĩa là: người mang số hiệu $a_i$ bắt buộc phải đứng ở vị trí nào đó phía trước người mang số hiệu $b_i$ trong hàng. (Không bắt buộc phải đứng ngay sát nhau, chỉ cần đứng trước).

Nhiệm vụ của bạn là tìm ra một cách xếp hàng cho toàn bộ $N$ người sao cho thỏa mãn tất cả $M$ yêu cầu trên.

\begin{itemize}

    \item Nếu có nhiều cách xếp hàng hợp lệ, hãy in ra cách xếp hàng có thứ tự từ điển nhỏ nhất (ưu tiên những người có số hiệu nhỏ được đứng càng gần đầu hàng càng tốt).

    \item Trong trường hợp các yêu cầu đưa ra mâu thuẫn với nhau khiến việc xếp hàng là bất khả thi, hãy đưa ra thông báo bằng lệnh $impossible$.

\end{itemize}

**Input format**:
\begin{itemize}

    \item Dòng đầu tiên chứa hai số nguyên $N$ và $M$ ($1 \le N \le 5 \cdot 10^5$, $0 \le M \le 5 \cdot 10^5$) --- lần lượt là số lượng người và số lượng yêu cầu.

    \item $M$ dòng tiếp theo, mỗi dòng chứa hai số nguyên $a_i$ và $b_i$ ($1 \le a_i, b_i \le N$, $a_i \neq b_i$) --- thể hiện yêu cầu người $a_i$ phải xếp trước người $b_i$.

\end{itemize}

**Output format**:
\begin{itemize}

    \item Nếu tồn tại cách xếp hàng hợp lệ, hãy in ra trên một dòng $N$ số nguyên cách nhau bởi khoảng trắng, thể hiện số hiệu của những người trong hàng từ vị trí đầu tiên đến vị trí cuối cùng (sao cho dãy số này có thứ tự từ điển nhỏ nhất).

    \item Nếu không tồn tại cách xếp hàng nào thỏa mãn, hãy in ra chuỗi $impossible$.

\end{itemize}