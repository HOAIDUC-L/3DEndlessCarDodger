# 3D ENDLESS CAR DODGER (MODULAR ARCHITECTURE)
## ĐỒ ÁN / BÁO CÁO GIỮA KỲ MÔN ĐỒ HỌA MÁY TÍNH (C++ & OPENGL/FREEGLUT)

---

## 1. CẤU TRÚC KIẾN TRÚC ĐA FILE (MODULAR ARCHITECTURE)

Dự án được phân tách thành các module độc lập theo đúng các nhóm kiến thức đồ họa máy tính:

```
MidTest/
├── Makefile                     # Quản lý biên dịch tự động (make, make run, make clean)
├── algo_2d.h & algo_2d.cpp      # Module 1: Thuật toán đồ họa cơ sở 2D & Minimap Radar
├── algo_3d_geometry.h & .cpp    # Module 2: Mô hình hóa 3D, Vector pháp tuyến & Biến đổi hình học
├── algo_shading_culling.h & .cpp# Module 3: Khử mặt khuất (Back-face, Z-Buffer), Tô bóng & Nguồn sáng
├── main.cpp                     # Module 4: Vòng lặp Game, Camera (gluLookAt), Va chạm AABB & HUD
└── README.md                    # Tài liệu hướng dẫn sử dụng và báo cáo thuật toán
```

---

## 2. NỘI DUNG VÀ Ý NGHĨA KỸ THUẬT CỦA TỪNG MODULE

### Module 1: `algo_2d.h` & `algo_2d.cpp` (Thuật toán 2D Cơ sở)
- **`putPixel(x, y)`**: Chấm điểm ảnh 2D cơ bản bằng `GL_POINTS`.
- **`drawLineBresenham(x1, y1, x2, y2)`**: Cài đặt thuật toán vẽ đoạn thẳng **Bresenham** tổng quát cho toàn bộ 8 góc phần tám, chỉ sử dụng phép cộng/trừ số nguyên và biến sai số `err = dx - dy`.
- **`drawCircleMidpoint(xc, yc, r, fill)`**: Cài đặt thuật toán vẽ đường tròn **Midpoint** tận dụng tính chất đối xứng 8 phần với biến quyết định $p = 1 - r$. Hỗ trợ vẽ viền và tô đặc theo đường quét ngang (Scanlines).
- **`cohenSutherlandClip(...)`**: Cài đặt thuật toán cắt xén đoạn thẳng **Cohen-Sutherland** với mã vùng 4-bit OutCode (`TOP`, `BOTTOM`, `RIGHT`, `LEFT`), kiểm tra chấp nhận/từ chối tức thì và tìm giao điểm chính xác.
- **`drawMinimap(...)`**: Ứng dụng vẽ màn hình Radar Minimap 2D ở góc trên bên phải màn hình:
  - Khung viền và vạch phân làn vẽ bằng Bresenham.
  - Vạch kẻ đường được cắt xén an toàn bằng Cohen-Sutherland đảm bảo không tràn ra ngoài radar.
  - Chấm xe người chơi và chướng ngại vật vẽ bằng Midpoint Circle, đồng bộ thời gian thực từ tọa độ 3D.

### Module 2: `algo_3d_geometry.h` & `algo_3d_geometry.cpp` (Mô hình hóa 3D & Vector Pháp tuyến)
- **`calcNormal(v0, v1, v2)`**: Tính tích có hướng $\vec{N} = \text{Normalize}((\vec{V}_1 - \vec{V}_0) \times (\vec{V}_2 - \vec{V}_0))$.
- **Các khối nguyên thủy CCW**:
  - `drawCuboid`: Khối hộp chữ nhật 6 mặt với vector pháp tuyến chuẩn.
  - `drawPyramid`: Khối chóp tứ giác đều S.ABCD (tính pháp tuyến từng mặt tam giác).
  - `drawFrustum`: Khối chóp cụt 4 mặt thang và 2 đáy.
  - `drawCylinder`: Hình trụ tròn đa giác với pháp tuyến tỏa tròn mượt.
  - `drawCone`: Hình nón với pháp tuyến sườn nón.
- **Lắp ghép đối tượng phức hợp (Transformations)**:
  - `drawCar`: Xe thể thao đa tầng, cánh gió, đèn pha/hậu, bánh xe tự lăn quay quanh trục bằng `glRotatef(wheelAngle, 1, 0, 0)`, thân xe nghiêng khi chuyển làn.
  - `drawTrafficCar`: Xe đối thủ cản đường.
  - `drawHouse`: Nhà phố thân hộp + mái chóp S.ABCD.
  - `drawTree`: Cây xanh thân trụ + tán nón 2 tầng.
  - `drawLamppost`: Cột đèn đường cao áp có tay đèn vươn ra lòng đường.
  - `drawRoadAndEnvironment`: Mặt đường nhựa, vạch kẻ đường trôi lùi tạo cảm giác chuyển động vô tận.

### Module 3: `algo_shading_culling.h` & `algo_shading_culling.cpp` (Khử mặt khuất & Tô bóng)
- **`applyRenderMode(mode)`**: Chuyển đổi 4 chế độ demo cho giảng viên:
  - `MODE_WIREFRAME (1)`: Khung dây `GL_LINE`, tắt khử khuất và ánh sáng.
  - `MODE_BACKFACE_CULL (2)`: Khử mặt sau `GL_CULL_FACE` (`GL_BACK`, `GL_CCW`), tắt Z-buffer.
  - `MODE_DEPTH_FLAT (3)`: Z-Buffer (`GL_DEPTH_TEST`) + Tô bóng phẳng Flat Shading (`GL_FLAT`).
  - `MODE_GOURAUD_FULL (4)`: Z-Buffer + Tô bóng mượt Gouraud (`GL_SMOOTH`) + Chiếu sáng Phong.
- **`setupLighting(isNight, carX, carZ)`**:
  - `GL_LIGHT0`: Mặt trời ban ngày / Ánh trăng ban đêm.
  - `GL_LIGHT1`: Đèn pha ô tô dạng nón **Spotlight** (`GL_SPOT_CUTOFF = 42.0f`, `GL_SPOT_DIRECTION = (0, -0.1, -1)`).
- **Vật liệu Phong**: `applyMaterial` và `applyEmissiveMaterial`.

### Module 4: `main.cpp` (Vòng lặp Game, Camera & Va chạm AABB)
- **Hệ thống Camera (`setupCamera`)**:
  - `CAM_THIRD_PERSON`: Camera bay sau đuôi xe.
  - `CAM_TOP_DOWN`: Camera nhìn thẳng từ trên cao xuống toàn cảnh.
  - `CAM_FIRST_PERSON`: Camera đặt tại nắp capo xe.
- **Va chạm AABB**: Kiểm tra giao nhau giữa hộp bao của xe và chướng ngại vật trên 3 trục tọa độ $X, Y, Z$.
- **HUD 2D (`gluOrtho2D`)**: Bảng điểm, tốc độ, quãng đường, trạng thái thuật toán đang bật, và màn hình Game Over.

---

## 3. HƯỚNG DẪN BIÊN DỊCH VÀ CHẠY GAME

### Lệnh biên dịch bằng Makefile:
```bash
make clean
make
```

### Chạy game:
```bash
./game
```
*Hoặc:*
```bash
make run
```

---

## 4. BẢNG PHÍM TẮT ĐIỀU KHIỂN & DEMO THUẬT TOÁN

| Phím Tắt | Chức Năng | Ý Nghĩa Kỹ Thuật |
| :---: | :--- | :--- |
| **`1`** | **Khung dây (Wireframe)** | Minh họa lưới đa giác thô (Mesh), tắt khử khuất và chiếu sáng. |
| **`2`** | **Back-face Culling** | Minh họa thuật toán khử mặt sau `GL_CULL_FACE` (tắt Z-buffer để thấy sự đè lấn của các mặt phía trước). |
| **`3`** | **Z-Buffer + Flat Shading** | Khử mặt khuất bằng Depth-Buffer; tô bóng phẳng làm lộ rõ các mặt facet của hình trụ và nón. |
| **`4`** | **Gouraud Shading + Phong** | Tô bóng mượt nội suy theo pháp tuyến đỉnh, phản xạ ánh sáng bóng bẩy. |
| **`N`** | **Chu kỳ Ngày / Đêm** | Chuyển đổi giữa Mặt trời (`GL_LIGHT0`) và Đèn pha ô tô Spotlight (`GL_LIGHT1`). |
| **`C`** | **Góc nhìn Camera** | Đổi giữa 3 góc nhìn: Sau xe <-> Toàn cảnh từ trên cao <-> Mui xe buồng lái. |
| **`M`** | **Bật / Tắt Minimap 2D** | Hiện/ẩn màn hình Radar 2D (chạy thuật toán Bresenham, Midpoint, Cohen-Sutherland). |
| **`A` / `D`** hoặc **`←` / `→`** | **Chuyển làn đường** | Chuyển giữa 3 làn xe, có nội suy vị trí mượt và nghiêng thân xe. |
| **`W` / `S`** hoặc **`↑` / `↓`** | **Tăng / Giảm tốc độ** | Điều chỉnh vận tốc xe từ $18$ đến $65$ đơn vị/s. |
| **`R`** | **Chơi lại (Restart)** | Khởi động lại vòng chơi khi bị Game Over. |
| **`ESC`** | **Thoát game** | Đóng cửa sổ và giải phóng bộ nhớ. |
