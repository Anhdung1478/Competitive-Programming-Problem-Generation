Sol 1: (DP)
State: dp[pos][tight], chỉ cho phép đặt chữ số 4 hoặc 7 ở mỗi vị trí khi không tight, và kiểm tra riêng khi tight.

Sol 2: (combinatorics)
Không cần digit DP: có thể đếm bằng cách duyệt độ dài rồi 2^len tổ hợp, cộng dồn — nhưng đây là bài mẫu tốt để thấy digit DP "giết ruồi bằng búa tạ" cỡ nào, giúp hiểu khi nào nên/không nên dùng nó.