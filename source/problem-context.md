**Problem name**: Ếch ham ăn
**Time limit**: 2s

**Legend**:
Một con ếch ham ăn đang ở trên đầm lầy toạ độ vô hạn 1, 2, 3... Có N con ruồi, ban đầu con ruồi thứ i nằm ở vị trí x(i).

Bạn được cho Q truy vấn, mỗi truy vấn thuộc một trong hai dạng sau:
* Truy vấn 1: "1 u y" tức là con ruồi thứ u di chuyển tới vị trí y.
* Truy vấn 2: "2 x" nghĩa là giả sử con ếch hiện tại đứng ở vị trí x, khi đó ếch có thể nhảy tới vị trí nào cuối cùng. Biết rằng ếch sẽ nhảy mỗi khi ăn được con ruồi, và ếch sẽ luôn ưu tiên ăn con ruồi nằm gần nhất ở bên trái hoặc cùng vị trí với nó và mỗi lần ăn con ruồi nằm ở vị trí y bất kì, ếch sẽ nhảy thêm y đơn vị sang phải (tức là nếu ếch đang ở vị trí x' thì nó sẽ nhảy sang vị trí x' + y). 

Lưu ý các truy vấn loại 2 là "giả sử" nghĩa là con ếch không thật sự ăn ruồi mà chỉ là một câu hỏi. Vậy nên sau mỗi truy vấn loại 2 các con ruồi vẫn không bị ăn.

**Input format**:
* Dòng đầu tiên gồm hai số nguyên dương N và Q (1 <= N, Q <= 2 x 10^5) - lần lượt đại diện cho số lượng con ruồi trên đầm lầy và số truy vấn.
* Dòng thứ hai gồm N số nguyên dương x(1), x(2),... x(N) (1 <= x(i) <= 10^9) - số thứ i đại diện cho vị trí của con ruồi thứ i.
* Q dòng cuối cùng, mỗi dòng là một truy vấn thuộc một trong hai dạng: "1 u y" (1 <= u <= N; 1 <= y <= 10^9) hoặc "2 x" (1 <= x <= 10^9).

**Output format**:
* Với mỗi truy vấn loại 2, in ra hai số nguyên lần lượt là vị trí cuối cùng ếch có thể tới và số lượng con ruồi mà ếch đã ăn để tới được vị trí đấy.