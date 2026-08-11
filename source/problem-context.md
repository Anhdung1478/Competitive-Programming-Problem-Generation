**Legend**:
Có $N$ loại kẹo khác nhau, được đánh số từ $1$ đến $N$.

Trong $M$ ngày liên tiếp, mỗi ngày Hà sẽ ăn đúng một viên kẹo. Tuy nhiên, cô cảm thấy nhàm chán nếu ăn cùng một loại kẹo trong hai ngày liên tiếp. Vì vậy, Hà muốn đảm bảo rằng: $C_i \neq C_{i+1}$. với mọi $1 \le i < M$, trong đó $C_i$ là loại kẹo mà Hà ăn vào ngày thứ $i$.

Để chuẩn bị tham gia cuộc thi Miss Universe, Hà quyết định thực hiện một chế độ ăn đặc biệt theo lời khuyên của chuyên gia.

Có $K$ ngày đã được chỉ định trước. Với mỗi $1 \le i \le K$, vào ngày thứ $A_i$, Hà bắt buộc phải ăn loại kẹo $B_i$.

Nói cách khác, $C_{A_i} = B_i$.

Ở những ngày không được chỉ định trước, Hà có thể tự do lựa chọn bất kỳ loại kẹo nào trong $N$ loại, miễn là không vi phạm điều kiện không ăn cùng một loại kẹo trong hai ngày liên tiếp.

Hãy tính số lượng cách Hà có thể lựa chọn loại kẹo cho toàn bộ $M$ ngày sao cho tất cả các yêu cầu trên đều được thỏa mãn.

Hai cách ăn được xem là khác nhau nếu tồn tại ít nhất một ngày mà loại kẹo được chọn trong hai cách là khác nhau.

Vì đáp án có thể rất lớn, hãy in kết quả theo modulo 998244353.

**Input format**:

\begin{itemize} 

    \item Dòng đầu tiên chứa ba số nguyên $N, M, K$ $(1 \le N \le 10^9, 1 \le M \le 10^{18}, 0 \le K \le \min(M, 10^4))$ lần lượt là số loại kẹo, số ngày mà Hà thực hiện chế độ ăn và số ngày đã được chỉ định trước loại kẹo phải ăn.

    \item $K$ dòng tiếp theo, dòng thứ $i$ chứa hai số nguyên $A_i, B_i$ $(1 \le A_i \le M, 1 \le B_i \le N)$, trong đó: $A_i$ là ngày được chỉ định trước; $B_i$ là loại kẹo Hà bắt buộc phải ăn vào ngày đó. Các ngày $A_i$ được cho theo thứ tự tăng dần: $A_1 < A_2 < \cdots < A_K$.

Nếu $K = 0$ thì không có dòng nào được nhập thêm sau dòng đầu tiên.

\end{itemize}

**Output format**:
\begin{itemize}

    \item In ra một số nguyên duy nhất: số cách lựa chọn loại kẹo cho $M$ ngày thỏa mãn tất cả các yêu cầu của Hà. Kết quả được lấy theo modulo 998244353.

\end{itemize}