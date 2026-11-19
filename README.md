# 3D ENDLESS SUPERCAR DODGER (MODULAR ARCHITECTURE)
## ĐỒ ÁN / BÁO CÁO GIỮA KỲ MÔN ĐỒ HỌA MÁY TÍNH (C++ & OPENGL/FREEGLUT)

---

## 1. CẤU TRÚC KIẾN TRÚC ĐA FILE (MODULAR ARCHITECTURE)

Dự án được phân tách thành các module độc lập theo đúng các nhóm kiến thức đồ họa máy tính:

```
MidTest/
├── Makefile                     # Quản lý biên dịch tự động (make, make run, make clean)
├── algo_2d.h & algo_2d.cpp      # Module 1: Thuật toán đồ họa cơ sở 2D & Minimap Radar
├── algo_3d_geometry.h & .cpp    # Module 2: Mô hình hóa Siêu xe, 4 Phương tiện giao thông & Pháp tuyến CCW
├── algo_shading_culling.h & .cpp# Module 3: Khử mặt khuất (Back-face, Z-Buffer), Tô bóng, Sương mù & Nguồn sáng
├── main.cpp                     # Module 4: Vòng lặp Game, Camera (gluLookAt), Va chạm AABB, Hạt & HUD
└── README.md                    # Tài liệu hướng dẫn sử dụng và thuyết trình thuật toán
```

---

## 2. BẢN ĐỒ THUẬT TOÁN ĐỒ HỌA ĐỂ TRÌNH BÀY CHO GIẢNG VIÊN

Toàn bộ các thuật toán cốt lõi đều được bảo toàn 100% và có cơ chế phím tắt trực tiếp trên bàn phím:

### A. Nhóm Thuật Toán 2D Cơ Sở (Module 1 - `algo_2d`)
1. **Thuật toán Bresenham (`drawLineBresenham`)**:
   - Vẽ khung viền radar và vạch phân làn Minimap 2D.
   - Hoàn toàn bằng số nguyên, xử lý đầy đủ mọi hệ số góc $|\Delta y| \le |\Delta x|$ và $|\Delta y| > |\Delta x|$.
2. **Thuật toán Midpoint Circle (`drawCircleMidpoint`)**:
   - Vẽ chấm vị trí xe người chơi (màu xanh neon) và các chấm phương tiện giao thông (đỏ, vàng, lam, cam).
   - Tận dụng tính đối xứng 8 phần với biến quyết định $p = 1 - R$.
3. **Thuật toán Cắt xén Cohen-Sutherland (`cohenSutherlandClip`)**:
   - Sử dụng mã vùng 4-bit (`TOP`, `BOTTOM`, `RIGHT`, `LEFT`) để cắt các vạch kẻ đường, đảm bảo không bị tràn ra ngoài khung radar.

### B. Nhóm Thuật Toán Mô Hình Hóa 3D & Biến Đổi Hình Học (Module 2 - `algo_3d_geometry`)
1. **Tính Vector Pháp Tuyến (Normal Vectors Calculation)**:
   - Tự động tính pháp tuyến mặt tam giác bằng tích có hướng: $\vec{N} = \text{Normalize}((\vec{V}_1 - \vec{V}_0) \times (\vec{V}_2 - \vec{V}_0))$.
   - Đảm bảo quy ước đỉnh ngược chiều kim đồng hồ (**CCW**) nhìn từ bên ngoài.
2. **Siêu Xe Thể Thao Người Chơi (Supercar)**:
   - Dựng từ khối hộp, chóp vát, cánh gió thể thao kép, gương chiếu hậu 2 bên, cản trước carbon.
   - Buồng lái panoramic bằng kính cong bán trong suốt (`GL_BLEND`).
   - Cụm 4 ống xả thể thao mạ chrome phát khói khi tăng tốc.
   - Mâm xe đúc hợp kim thể thao kèm cùm phanh đĩa (Brake Caliper màu đỏ) nổi bật bên trong.
   - Bánh xe tự quay quanh trục theo vận tốc di chuyển: $\theta = \frac{d}{R} \cdot \frac{180^\circ}{\pi}$.
   - Thân xe nghiêng quanh trục $Z$ khi đánh lái chuyển làn.
3. **4 Chủng Loại Phương Tiện Giao Thông Cản Đường Thực Tế**:
   - 🚛 **Xe tải Container khổng lồ (Semi-Trailer Truck)**: Thùng dài $8.2\text{m}$, gân dập nổi, 8 bánh xe hạng nặng, 2 ống xả đứng mạ chrome, chạy chậm.
   - 🚌 **Xe buýt thành phố (City Transit Bus)**: Dáng dài $6.8\text{m}$, dải kính hành khách xanh trong suốt, biển hiệu tuyến đường phát sáng.
   - 🛻 **Xe bán tải địa hình (Pickup Truck / SUV)**: Gầm cao $1.6\text{m}$, cản trước chống va đập (bull bar), thùng chở hàng có khung giằng thể thao (roll bar).
   - 🚗 **Xe Sedan / Taxi đô thị**: Dáng xe thanh thoát, màu sắc rực rỡ, có biển hiệu Taxi trên nóc.
4. **Chia lưới mặt đường (Subdivided Grid) & Đèn pha Spotlight**:
   - Mặt đường chia lưới nhỏ ($1.0\text{m} \times 1.8\text{m}$) để đèn pha ô tô dạng nón (`GL_LIGHT1`) tạo ra vệt sáng elip loang tròn mượt mà trên nền nhựa đường trong chế độ Gouraud Shading.
5. **Đổ bóng gầm xe (Planar Shadows)**:
   - Đa giác đen bán trong suốt nằm sát mặt đường ($Y = 0.015\text{m}$) dưới gầm tất cả các xe, tạo cảm giác xe bám chặt mặt đường.

### C. Nhóm Thuật Toán Khử Mặt Khuất, Tô Bóng & Chiếu Sáng (Module 3 - `algo_shading_culling`)
- **Phím `1` (Toggle)**: Chế độ Khung dây (**Wireframe** - `GL_LINE`), tắt khử khuất và chiếu sáng để xem lưới đa giác nguyên thủy.
- **Phím `2` (Toggle)**: Thuật toán khử mặt sau **Back-face Culling** (`GL_CULL_FACE`, `GL_BACK`, `GL_CCW`), tắt Z-buffer để chứng minh vì sao culling chưa đủ khử khuất hoàn toàn.
- **Phím `3` (Toggle)**: Khử mặt khuất bằng **Depth-Buffer (Z-Buffer)** kết hợp **Tô bóng phẳng (Flat Shading)** làm rõ các góc cạnh của hình khối.
- **Phím `4` (Toggle)**: Z-Buffer + **Tô bóng mượt Gouraud Shading** (`GL_SMOOTH`) + Mô hình chiếu sáng Phong đầy đủ.
- **Phím `N`**: Chu kỳ Ngày / Đêm. Ban đêm bật đèn pha nón **Spotlight** (`GL_LIGHT1`) có góc mở `GL_SPOT_CUTOFF = 42.0f`, hướng chiếu `GL_SPOT_DIRECTION = (0, -0.12, -1)` và suy giảm khoảng cách Attenuation.
- **Sương mù chiều sâu `GL_FOG`**: Hòa quyện chân trời mượt mà bằng phương trình `GL_LINEAR`.

### D. Nhóm Vòng Lặp Game, Camera & Va Chạm AABB (Module 4 - `main`)
- **Phát hiện va chạm AABB (Axis-Aligned Bounding Box)**: Kiểm tra hộp bao 3 chiều riêng biệt cho từng loại xe (Xe tải dài $8.2\text{m}$, xe buýt dài $6.8\text{m}$, xe con dài $4.0\text{m}$).
- **Hệ thống Camera (`gluLookAt`)**: Chuyển đổi qua lại bằng phím **`C`** (Sau xe <-> Toàn cảnh trên cao <-> Mui xe buồng lái).
- **Phím Cách (Spacebar)**: Chơi lại ngay lập tức khi xảy ra va chạm (Game Over).

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
