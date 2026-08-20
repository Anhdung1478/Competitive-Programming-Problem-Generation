**Problem name**: Chia dãy số
**Time limit**: 1s

**Legend**:
Cho dãy số $a_1, a_2,...,a_n$. Ta ký hiệu $s_{l,r} = a_{l + 1} + a_{l + 2} + ... + a_{r}$ với $1 \le l \le r \le n$.

Ví dụ, ta có dãy số $a_1 = 10, a_2 = 7, a_3 = -5$ thì khi đó $s_{1,2} = 17, s_{1,3} = 12, s_{2,3} = 2$ và $s_{1,1}, s_{2,2} = s_{3,3} = 0$.

Hãy tìm năm chỉ số $x, y, z, t$ và $u$ sao cho $0 \le x \le y \le z \le t \le u \le n$ và $-s_{0,x} + s_{x,y} - s_{y,z} + s_{z,t} - s_{t,u} + s_{u,n}$ lớn nhất.

**Input format**:
Dòng đầu tiên chứa số nguyên $n$ $(1 \le n \le 10^6)$.

Dòng thứ hai chứa $n$ số nguyên $a_1,a_2,...,a_n$ $(1 \le a_i \le 10^9)$.


**Output format**:
Gồm một số nguyên duy nhất là giá trị lớn nhất của tổng $-s_{0,x} + s_{x,y} - s_{y,z} + s_{z,t} - s_{t,u} + s_{u,n}$ với $0 \le x \le y \le z \le t \le u \le n$.