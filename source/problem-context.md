**Problem name**: Mật mã lăng mộ cổ
**Time limit**: 1s
**Memory limit**: 512 MB

**Legend**:
Trong một chuyến thám hiểm tới vùng đất của vương quốc Algoria nghìn năm tuổi, nhà khảo cổ học trẻ tuổi tên Dũng vô tình phát hiện ra một lăng mộ cổ nằm sâu dưới lòng đất. Để mở được cánh cổng đá khổng lồ tiến vào căn phòng chứa kho báu, Dũng phải giải mã được một câu đố được khắc trên bức tường.

Bức tường hiện lên hai con số khổng lồ $L$ và $R$, đại diện cho một dải phép thuật gồm các số nguyên nằm trong đoạn $[L, R]$. Bên cạnh đó là một văn tự cổ định nghĩa về \"chỉ số sức mạnh\" của một con số như sau:

Giả sử ta xét một số nguyên $X$, và viết các chữ số của $X$ ra theo thứ tự từ trái sang phải (từ hàng lớn nhất đến hàng nhỏ nhất).

\begin{enumerate}

    \item Sức mạnh của $X$ chính là độ dài của dãy con không giảm dài nhất tạo được từ dãy chữ số của $X$ (không giảm nghĩa là chữ số đứng sau phải lớn hơn hoặc bằng chữ số đứng trước).

    \item Độ thuần khiết của $X$ là số lượng các dãy con không giảm đạt được độ dài sức mạnh đó.

\end{enumerate}

Cánh cổng không yêu cầu tìm ra một con số cụ thể nào, mà nó yêu cầu Dũng phải gộp sức mạnh của cả dải số lại. Cụ thể, Dũng cần tìm ra:

\begin{itemize}

    \item Mức Sức mạnh lớn nhất có thể đạt được trong tất cả các số thuộc đoạn $[L, R]$.

    \item Tổng độ thuần khiết của tất cả các con số đạt được mức sức mạnh lớn nhất đó.

\end{itemize}

Vì $L$ và $R$ là những con số mang sức mạnh của thần linh (có thể dài tới 10.000 chữ số), số lượng các con số là quá lớn, Dũng không thể nhẩm tính bằng tay. Bạn hãy viết một chương trình giúp Dũng tìm ra Sức mạnh cực đại, Độ thuần khiết cực đại, và đếm xem có tất cả bao nhiêu số Hoàn Hảo nằm trong khoảng từ $L$ đến $R$ nhé! Vì số lượng số Hoàn Hảo có thể rất khổng lồ, bạn chỉ cần đưa ra phần dư của nó khi chia cho $998244353$.

**Input format**:
\begin{itemize}

    \item Dòng đầu tiên chứa một số nguyên dương $T$ ($1 \le T \le 20$) $-$ số lượng câu hỏi (test cases) hiển thị trên cánh cổng.

    \item $T$ dòng tiếp theo, mỗi dòng chứa hai số nguyên dương $L$ và $R$ ($1 \le L \le R \le 10^{10000}$), được cách nhau bởi một khoảng trắng. Số lượng chữ số của $L$ và $R$ có thể lên tới $10.000$ chữ số.

\end{itemize}

**Output format**:
In ra $T$ dòng, mỗi dòng là câu trả lời cho một test case tương ứng, gồm hai số nguyên cách nhau bởi một khoảng trắng lần lượt là:

\begin{itemize}

    \item Mức Sức mạnh lớn nhất tìm được trong đoạn $[L, R]$.

    \item Tổng độ thuần khiết của tất cả các con số đạt được sức mạnh lớn nhất đó (kết quả đã được chia lấy dư cho $998244353$).

\end{itemize}