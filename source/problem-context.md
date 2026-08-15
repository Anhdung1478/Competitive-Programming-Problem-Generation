**Legend**:
Cho Q truy vấn, mỗi truy vấn được thêm một ký tự vào đầu xâu S hoặc xóa 1 ký tự khỏi đầu xâu S. Ban đầu xâu S rỗng. Sau mỗi truy vấn hỏi số xâu con liên tiếp phân biệt của S là bao nhiêu. 

Các ký tự trong xâu S trước và sau mỗi truy vấn chỉ gồm các ký tự alphabet in thường. Dữ liệu đảm bảo sau mỗi truy vấn độ dài xâu S vẫn >= 0. Không tồn tại trường hợp xâu S đang rỗng và có một truy vấn xoá.

**Input format**:
- Dòng đầu tiên gồm duy nhất một số Q (số truy vấn).
- Q dòng tiếp theo mỗi dòng thuộc một trong hai dạng sau:
    - *+ c*: Thêm một ký tự c vào đầu xâu S.
    - *-*: Xoá một ký tự nằm ở đầu xâu S.

**Output format**:
- In ra Q số, số thứ i là số xâu con liên tiếp phân biệt của S sau i truy vấn.