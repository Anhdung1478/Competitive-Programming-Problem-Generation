**Problem name**: Quân xe
**Time limit**: 2s

**Legend**:
Trong bài tập này, ta xét một bàn cờ vua đặc biệt gồm $r$ hàng và $c$ cột. Trên đó, ta định nghĩa một quân cờ đặc biệt, đó là quân "không phải quân xe". Trong một nước đi, quân cờ "không phải quân xe" có thể đi tới bất kì ô nào trên bàn cờ mà quân xe bình thường không thể đi tới. Nói cách khác, quân cờ "không phải quân xe" trong một nước có thể đi tới tất cả các ô, trừ các ô cùng hàng, cùng cột với vị trí hiện tại. Chú ý rằng, quân cờ này trong một nước bắt buộc phải đi đến ô khác, không được đứng yên ở vị trí hiện tại.

Hãy đếm số chuỗi di chuyển hợp lệ độ dài $k$ của một quân "không phải quân xe" bắt đầu từ ô $(1, 1)$ và kết thúc tại ô $(r, c)$. Một chuỗi di chuyển hợp lệ là một dãy gồm $k + 1$ ô $(x_0, y_0)$, $(x_1, y_1)$, ..., $(x_k, y_k)$ sao cho với mọi $1 \le i \le k$, quân "không phải quân xe" có thể đi từ ô $(x_{i-1}, y_{i-1})$ đến ô $(x_i, y_i)$. Hai chuỗi di chuyển được coi là khác nhau nếu như tồn tại chỉ số $i$ sao cho ô thứ $i$ ở chuỗi này khác với ô thứ $i$ ở chuỗi kia.

**Input format**:
Gồm một dòng duy nhất chứa ba số nguyên $r, c, k$ $(1 \le r, c \le 10^{18}, 1 \le k \le 10^{18})$.

**Output format**:
In ra một số nguyên duy nhất là cách di chuyển hợp lệ độ dài $k$ theo modulo $998244353$.
