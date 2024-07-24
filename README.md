
# espUART_TEST_20231226_resetPower
Hướng dẫn dùng nút reset power:
1)Nhấn giữ nút reset power
2)Nhấn reset button của esp (rút nguồn và cắm lại-- giữ nút reset power trong cả quá trình)
3)Đèn vàng sáng >> esp đã nhận đc tín hiệu reset
4)Sau 5s đèn vàng tắt >> đèn xanh nhấp nháy 5 lần >> chuẩn bụ reset >> nháy 5 lần nữa: reset thành công (nếu nháy 2 lần chậm >> pzem không thể reset)
<6) Nếu đèn vàng tắt nhưng đèn xanh ko sáng >> ko bấm nút đủ 5s >> tắt tín hiệu reset và chạy tiếp tục chương trình>

Tín hiệu đèn
1, Đèn xanh lá cây nhấp nháy, kết nối mạng thành công và truyền dữ liệu thành công lên Fbdo và xuất dữ liệu offline
2, Đèn xanh lá cây sáng liên tục kết nối mạng không ổn định >> quá trình truyền bị ngẽn mạng >> mạng kém
3, Đèn xanh lục nhấp nháy, kết nối mạng không thành công, không thể ghi dữ lên Fbdo, nhưng vẫn hiển thị ra màn hình (xuất dữ liệu offline)
4, Đèn xanh lục sáng rồi đèn xanh lục và đèn xanh lá nhấp nháy xen kẽ, kết nối mạng thành công nhưng có lỗi trong quá trình ghi lên Fbdo# Power_Metter_Dev

