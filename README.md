# 3D ENDLESS CAR DODGER (ENHANCED VISUALS & MODULAR ARCHITECTURE)
## ĐỒ ÁN / BÁO CÁO GIỮA KỲ MÔN ĐỒ HỌA MÁY TÍNH (C++ & OPENGL/FREEGLUT)

---

## 1. CẤU TRÚC KIẾN TRÚC ĐA FILE (MODULAR ARCHITECTURE)

Dự án được phân tách thành các module độc lập theo đúng các nhóm kiến thức đồ họa máy tính:

```
MidTest/
├── Makefile                     # Quản lý biên dịch tự động (make, make run, make clean)
├── algo_2d.h & algo_2d.cpp      # Module 1: Thuật toán đồ họa cơ sở 2D & Minimap Radar
├── algo_3d_geometry.h & .cpp    # Module 2: Mô hình hóa 3D, Vector pháp tuyến & Biến đổi hình học
├── algo_shading_culling.h & .cpp# Module 3: Khử mặt khuất (Back-face, Z-Buffer), Tô bóng, Sương mù & Nguồn sáng
├── main.cpp                     # Module 4: Vòng lặp Game, Camera (gluLookAt), Va chạm AABB, Hạt & HUD
└── README.md                    # Tài liệu hướng dẫn sử dụng và thuyết trình thuật toán
```

---

## 2. CÁC NÂNG CẤP THẨM MỸ & THUẬT TOÁN ĐỒ HỌA MỚI

1. **Khử răng cưa đa mẫu (MSAA - Multisample Anti-Aliasing):**
   - Khởi tạo `GLUT_MULTISAMPLE` trong `glutInitDisplayMode` và kích hoạt `glEnable(GL_MULTISAMPLE)` trong `main.cpp` giúp các cạnh xiên của khối lập phương, thân xe và vạch kẻ đường mượt mà, không bị răng cưa giật cục.
2. **Sương mù chiều sâu (GL_FOG):**
   - Cài đặt trong `algo_shading_culling.cpp` với chế độ tuyến tính `GL_LINEAR` (từ $35\text{m}$ đến $160\text{m}$).
   - Ban ngày sương mù màu xanh trời nhạt, ban đêm sương mù màu xanh đen thẫm đồng bộ với `glClearColor`, làm mờ dần đường chân trời và che đi mặt cắt zFar đột ngột.
3. **Đổ bóng gầm xe (Planar Shadows):**
   - Sử dụng `GL_BLEND` vẽ bóng đổ bán nguyệt bán trong suốt màu đen mờ `(0.01, 0.01, 0.01, 0.42)` tại độ cao $Y = 0.015\text{m}$ dưới gầm xe người chơi và các xe cản đường, tạo cảm giác xe bám chặt mặt đường.
4. **Chia lưới mặt đường (Subdivided Road Grid) đón đèn pha Spotlight:**
   - Thay vì mặt đường là 1 tứ giác duy nhất, mặt đường được chia nhỏ thành lưới mắt cáo dày theo trục $Z$ ($dz = 1.8\text{m}$) và trục $X$ ($dx = 1.0\text{m}$).
   - Trong chế độ Gouraud Shading (`GL_SMOOTH`), ánh sáng nón Spotlight của đèn pha ô tô (`GL_LIGHT1`) được tính toán chuẩn xác tại hàng nghìn đỉnh, tạo ra vệt sáng loang tròn tự nhiên rọi sáng mặt đường phía trước xe vào ban đêm.
5. **Chi tiết cảnh quan & Hiệu ứng hạt khói ống xả (Particles):**
   - **Hệ thống hạt khói**: Khói xám mờ liên tục sinh ra từ 2 ống xả sau đuôi xe, cuộn lên cao, nở to dần và mờ dần theo thời gian.
   - **Kính xe trong suốt**: Kính chắn gió trước vát nghiêng sử dụng vật liệu kính xanh lam trong suốt `(0.25, 0.55, 0.85, 0.55)` kết hợp `GL_BLEND`.
   - **Dãy núi xa**: Dãy núi tuyết nhấp nhô ở cuối chân trời ẩn hiện trong màn sương mù `GL_FOG`.
   - **Hộ lan**: Hộ lan kim loại bảo vệ chạy dọc 2 bên lề đường.
6. **Cơ chế điều khiển thông minh**:
   - **Phím CÁCH (Spacebar)**: Nhấn nút Space để chơi lại ngay khi bị Game Over.
   - **Phím Toggle 1, 2, 3, 4**: Nhấn lần 1 kích hoạt chế độ đồ họa, nhấn thêm lần nữa sẽ tự động quay trở lại chế độ ban đầu!

---

## 3. HƯỚNG DẪN BIÊN DỊCH VÀ CHẠY GAME

```bash
cd /home/hduc/Documents/dhmt/MidTest
make clean && make
./game
```
*Hoặc chạy nhanh bằng lệnh:*
```bash
make run
```

---

## 4. BẢNG PHÍM TẮT ĐIỀU KHIỂN & DEMO CHO GIẢNG VIÊN

| Phím Tắt | Chức Năng | Ý Nghĩa Kỹ Thuật Demo |
| :---: | :--- | :--- |
| **`1`** | **Wireframe (Toggle)** | Khung dây, xem lưới đa giác thô (Mesh), tắt sương mù & ánh sáng. Nhấn lại để trở về cũ. |
| **`2`** | **Back-face Culling (Toggle)** | Thuật toán khử mặt sau `GL_CULL_FACE` (tắt Z-buffer). Nhấn lại để trở về cũ. |
| **`3`** | **Z-Buffer + Flat (Toggle)** | Khử mặt khuất bằng Depth-Buffer; tô bóng phẳng làm nổi bật các facet đa giác. Nhấn lại để trở về cũ. |
| **`4`** | **Gouraud + Phong (Toggle)** | Chế độ chuẩn: Tô bóng mượt nội suy theo pháp tuyến đỉnh, phản xạ Specular chân thực. |
| **`N`** | **Chu kỳ Ngày / Đêm** | Chuyển đổi giữa Mặt trời (`GL_LIGHT0`) và Đèn pha ô tô dạng nón Spotlight (`GL_LIGHT1`). |
| **`C`** | **Đổi Góc Nhìn Camera** | Chuyển đổi giữa 3 góc nhìn: Sau xe <-> Toàn cảnh từ trên cao <-> Mui xe buồng lái. |
| **`M`** | **Bật / Tắt Minimap 2D** | Hiện/ẩn màn hình Radar 2D (chạy thuật toán Bresenham, Midpoint, Cohen-Sutherland). |
| **`SPACE`** | **Chơi lại (Restart)** | Khởi động lại lượt chơi mới ngay lập tức khi xảy ra va chạm (Game Over). |
| **`A` / `D`** hoặc **`←` / `→`** | **Chuyển làn đường** | Chuyển giữa 3 làn xe, có nội suy vị trí mượt và nghiêng thân xe. |
| **`W` / `S`** hoặc **`↑` / `↓`** | **Tăng / Giảm tốc độ** | Điều chỉnh vận tốc xe trong khoảng từ $40$ đến $140\text{ km/h}$. |
| **`R`** | **Chơi lại phụ** | Phím R vẫn hoạt động song song với phím Space để chơi lại. |
| **`ESC`** | **Thoát game** | Đóng cửa sổ và giải phóng tài nguyên. |
