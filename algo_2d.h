/**
 * ============================================================================
 * MODULE 1: THUẬT TOÁN ĐỒ HỌA CƠ SỞ 2D (ALGO_2D.H)
 * Dự án: 3D Endless Car Dodger (C++ & OpenGL / FreeGLUT)
 * 
 * Mô tả module:
 *  - Cài đặt từ đầu các thuật toán đồ họa rasterization 2D kinh điển:
 *    1. Thuật toán vẽ đoạn thẳng Bresenham (Bresenham's Line Algorithm)
 *    2. Thuật toán vẽ đường tròn Midpoint (Midpoint Circle Algorithm)
 *    3. Thuật toán cắt xén đoạn thẳng Cohen-Sutherland (Line Clipping)
 *  - Ứng dụng các thuật toán trên để xây dựng màn hình Radar / Minimap 2D
 *    hiển thị trực tiếp trên giao diện HUD ở góc màn hình.
 * ============================================================================
 */

#ifndef ALGO_2D_H
#define ALGO_2D_H

#include <vector>

// Cấu trúc thông tin chướng ngại vật rút gọn phục vụ vẽ Minimap 2D
struct MinimapObstacle {
    float x;        // Tọa độ X thế giới 3D (làn đường)
    float z;        // Tọa độ Z thế giới 3D (khoảng cách tới xe)
    int type;       // Loại: 0 = Crate, 1 = Barrier, 2 = Car
    bool active;    // Trạng thái hoạt động
};

/**
 * [THUẬT TOÁN VẼ ĐIỂM ẢNH CƠ BẢN]
 * Vẽ một điểm ảnh tại tọa độ nguyên (x, y) trên màn hình 2D bằng GL_POINTS
 */
void putPixel(int x, int y);

/**
 * [THUẬT TOÁN ĐỒ HỌA BRESENHAM - VẼ ĐOẠN THẲNG]
 * Nguyên lý:
 *  - Sử dụng toàn bộ phép tính số nguyên (chỉ cộng, trừ và dịch bit nhân 2).
 *  - Dựa trên biến quyết định sai số (Decision Variable) để chọn điểm ảnh tiếp theo.
 *  - Hỗ trợ toàn bộ 8 góc phần tám (tất cả mọi hệ số góc m và mọi hướng đi).
 * 
 * @param x1, y1 : Tọa độ điểm bắt đầu
 * @param x2, y2 : Tọa độ điểm kết thúc
 */
void drawLineBresenham(int x1, int y1, int x2, int y2);

/**
 * [THUẬT TOÁN ĐỒ HỌA MIDPOINT - VẼ ĐƯỜNG TRÒN]
 * Nguyên lý:
 *  - Tận dụng tính chất đối xứng 8 phần (8-way symmetry) của đường tròn tâm (xc, yc).
 *  - Khởi tạo x = 0, y = r, biến quyết định p = 1 - r.
 *  - Nếu p < 0: chọn điểm E (y giữ nguyên), p = p + 2x + 1
 *  - Nếu p >= 0: chọn điểm SE (y giảm 1), p = p + 2(x - y) + 1
 * 
 * @param xc, yc : Tọa độ tâm đường tròn
 * @param r      : Bán kính đường tròn
 * @param fill   : true = tô đặc hình tròn, false = vẽ viền ngoài
 */
void drawCircleMidpoint(int xc, int yc, int r, bool fill = false);

/**
 * [THUẬT TOÁN CẮT XÉN ĐOẠN THẲNG COHEN-SUTHERLAND]
 * Nguyên lý:
 *  - Chia không gian 2D thành 9 vùng bằng mã 4-bit (OutCode): TOP(1000), BOTTOM(0100), RIGHT(0010), LEFT(0001).
 *  - Kiểm tra chấp nhận tức thì: Code1 | Code2 == 0 (cả 2 điểm nằm trong cửa sổ).
 *  - Kiểm tra loại bỏ tức thì: Code1 & Code2 != 0 (cả 2 điểm cùng nằm ngoài 1 cạnh).
 *  - Trường hợp cắt: Tìm giao điểm của đoạn thẳng với biên cửa sổ rồi lặp lại.
 * 
 * @param x1, y1, x2, y2 : Tọa độ 2 đầu đoạn thẳng (sẽ bị cắt bớt nếu vượt ra ngoài)
 * @param xmin, ymin, xmax, ymax : Tọa độ hình chữ nhật cửa sổ cắt xén
 * @return true nếu đoạn thẳng còn phần nhìn thấy, false nếu bị loại bỏ hoàn toàn
 */
bool cohenSutherlandClip(int &x1, int &y1, int &x2, int &y2, int xmin, int ymin, int xmax, int ymax);

/**
 * [ỨNG DỤNG MINIMAP / RADAR 2D]
 * Vẽ màn hình radar định vị 2D thu nhỏ ở góc màn hình:
 *  - Khung viền và nền radar (dùng Bresenham).
 *  - Vạch kẻ 3 làn đường (dùng Bresenham kết hợp cắt xén Cohen-Sutherland).
 *  - Vị trí xe của người chơi (dùng Midpoint Circle tô đặc màu vàng).
 *  - Vị trí các chướng ngại vật (dùng Midpoint Circle tô đặc màu đỏ/cam).
 * 
 * @param miniX, miniY : Góc dưới bên trái của khung Minimap trên màn hình
 * @param miniW, miniH : Chiều rộng và chiều cao của khung Minimap
 * @param playerX, playerZ : Tọa độ 3D của xe người chơi
 * @param obstacles    : Danh sách chướng ngại vật đang có trên đường
 * @param roadWidth    : Chiều rộng đường đua 3D
 * @param viewAhead    : Tầm quan sát phía trước xe (3D)
 * @param viewBehind   : Tầm quan sát phía sau xe (3D)
 */
void drawMinimap(int miniX, int miniY, int miniW, int miniH,
                 float playerX, float playerZ,
                 const std::vector<MinimapObstacle>& obstacles,
                 float roadWidth, float viewAhead, float viewBehind);

#endif // ALGO_2D_H
