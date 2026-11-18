/**
 * ============================================================================
 * MODULE 1: THUẬT TOÁN ĐỒ HỌA CƠ SỞ 2D (ALGO_2D.CPP)
 * Dự án: 3D Endless Car Dodger (C++ & OpenGL / FreeGLUT)
 * ============================================================================
 */

#include "algo_2d.h"
#include <GL/gl.h>
#include <cmath>
#include <cstdlib>

// ============================================================================
// 1. THUẬT TOÁN VẼ ĐIỂM ẢNH (PUT PIXEL)
// ============================================================================
void putPixel(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

// ============================================================================
// 2. THUẬT TOÁN BRESENHAM VẼ ĐOẠN THẲNG (BRESENHAM'S LINE ALGORITHM)
// ============================================================================
/**
 * Giải thích nguyên lý toán học cho báo cáo:
 * - Phương trình đường thẳng y = m*x + b với m = dy / dx.
 * - Thuật toán Bresenham chuyển đổi việc so sánh khoảng cách thực thành việc kiểm tra
 *   dấu của biến sai số nguyên P_k.
 * - Không sử dụng phép nhân số thực hay phép chia, tốc độ xử lý nhanh nhất trong đồ họa raster.
 * - Thuật toán dưới đây tổng quát hóa cho cả 8 góc phần tám (octants) bằng biến bước nhảy
 *   sx, sy và biến sai số tích lũy err = dx - dy.
 */
void drawLineBresenham(int x1, int y1, int x2, int y2) {
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    glBegin(GL_POINTS);
    while (true) {
        glVertex2i(x1, y1);
        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
    glEnd();
}

// ============================================================================
// 3. THUẬT TOÁN MIDPOINT VẼ ĐƯỜNG TRÒN (MIDPOINT CIRCLE ALGORITHM)
// ============================================================================
/**
 * Giải thích nguyên lý toán học cho báo cáo:
 * - Dựa trên hàm chính tắc F(x, y) = x^2 + y^2 - R^2.
 * - Khởi tạo từ đỉnh cao nhất (0, R). Tại mỗi bước x tăng 1, chọn giữa điểm E (x+1, y)
 *   hoặc SE (x+1, y-1) bằng cách kiểm tra dấu của biến quyết định p = F(x+1, y - 0.5) = 1 - R.
 * - Nếu p < 0: chọn E, p = p + 2*x + 3.
 * - Nếu p >= 0: chọn SE, p = p + 2*(x - y) + 5.
 * - Tận dụng tính đối xứng 8 phần để sinh toàn bộ đường tròn chỉ từ 1/8 cung đầu tiên.
 */
void drawCircleMidpoint(int xc, int yc, int r, bool fill) {
    int x = 0;
    int y = r;
    int p = 1 - r;

    // Hàm nội bộ vẽ 8 điểm đối xứng
    auto plot8 = [xc, yc, fill](int px, int py) {
        if (!fill) {
            glVertex2i(xc + px, yc + py);
            glVertex2i(xc - px, yc + py);
            glVertex2i(xc + px, yc - py);
            glVertex2i(xc - px, yc - py);
            glVertex2i(xc + py, yc + px);
            glVertex2i(xc - py, yc + px);
            glVertex2i(xc + py, yc - px);
            glVertex2i(xc - py, yc - px);
        } else {
            // Khi fill = true: Tô các đường quét ngang (Scanlines) để tạo hình tròn đặc
            for (int ix = xc - px; ix <= xc + px; ++ix) {
                glVertex2i(ix, yc + py);
                glVertex2i(ix, yc - py);
            }
            for (int ix = xc - py; ix <= xc + py; ++ix) {
                glVertex2i(ix, yc + px);
                glVertex2i(ix, yc - px);
            }
        }
    };

    glBegin(GL_POINTS);
    plot8(x, y);

    while (x < y) {
        x++;
        if (p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * (x - y) + 1;
        }
        plot8(x, y);
    }
    glEnd();
}

// ============================================================================
// 4. THUẬT TOÁN CẮT XÉN ĐOẠN THẲNG COHEN-SUTHERLAND (LINE CLIPPING)
// ============================================================================
// Định nghĩa mã vùng 4-bit (OutCodes)
static const int INSIDE = 0; // 0000: Nằm hoàn toàn bên trong cửa sổ
static const int LEFT   = 1; // 0001: Nằm bên trái xmin
static const int RIGHT  = 2; // 0010: Nằm bên phải xmax
static const int BOTTOM = 4; // 0100: Nằm phía dưới ymin
static const int TOP    = 8; // 1000: Nằm phía trên ymax

// Hàm tính mã vùng 4-bit cho một điểm (x, y)
static int computeOutCode(int x, int y, int xmin, int ymin, int xmax, int ymax) {
    int code = INSIDE;
    if (x < xmin)      code |= LEFT;
    else if (x > xmax) code |= RIGHT;
    if (y < ymin)      code |= BOTTOM;
    else if (y > ymax) code |= TOP;
    return code;
}

/**
 * Giải thích nguyên lý cho báo cáo:
 * - Bước 1: Tính mã vùng OutCode cho 2 đầu mút P1 và P2.
 * - Bước 2: Kiểm tra chấp nhận tức thì: Nếu (Code1 | Code2) == 0, đoạn thẳng nằm trọn bên trong.
 * - Bước 3: Kiểm tra từ chối tức thì: Nếu (Code1 & Code2) != 0, cả 2 điểm cùng nằm ngoài 1 biên.
 * - Bước 4: Nếu không thuộc 2 trường hợp trên, tìm giao điểm của đoạn thẳng với cạnh cửa sổ,
 *   thay thế điểm ngoài bằng giao điểm và lặp lại kiểm tra.
 */
bool cohenSutherlandClip(int &x1, int &y1, int &x2, int &y2, int xmin, int ymin, int xmax, int ymax) {
    int code1 = computeOutCode(x1, y1, xmin, ymin, xmax, ymax);
    int code2 = computeOutCode(x2, y2, xmin, ymin, xmax, ymax);
    bool accept = false;

    while (true) {
        if ((code1 | code2) == 0) {
            // Trường hợp 1: Chấp nhận tức thì (Cả 2 điểm đều nằm trong)
            accept = true;
            break;
        } else if (code1 & code2) {
            // Trường hợp 2: Từ chối tức thì (Cả 2 điểm cùng nằm ngoài một biên)
            break;
        } else {
            // Trường hợp 3: Đoạn thẳng cắt qua biên, cần tính giao điểm
            int codeOut = code1 ? code1 : code2;
            double x = 0.0, y = 0.0;

            // Tìm giao điểm với đường thẳng biên tương ứng
            if (codeOut & TOP) {
                // Giao điểm với cạnh trên y = ymax: x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1)
                x = x1 + (double)(x2 - x1) * (ymax - y1) / (double)(y2 - y1);
                y = ymax;
            } else if (codeOut & BOTTOM) {
                // Giao điểm với cạnh dưới y = ymin
                x = x1 + (double)(x2 - x1) * (ymin - y1) / (double)(y2 - y1);
                y = ymin;
            } else if (codeOut & RIGHT) {
                // Giao điểm với cạnh phải x = xmax: y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1)
                y = y1 + (double)(y2 - y1) * (xmax - x1) / (double)(x2 - x1);
                x = xmax;
            } else if (codeOut & LEFT) {
                // Giao điểm với cạnh trái x = xmin
                y = y1 + (double)(y2 - y1) * (xmin - x1) / (double)(x2 - x1);
                x = xmin;
            }

            // Gán lại tọa độ điểm ngoài bằng giao điểm mới và tính lại mã OutCode
            if (codeOut == code1) {
                x1 = (int)std::round(x);
                y1 = (int)std::round(y);
                code1 = computeOutCode(x1, y1, xmin, ymin, xmax, ymax);
            } else {
                x2 = (int)std::round(x);
                y2 = (int)std::round(y);
                code2 = computeOutCode(x2, y2, xmin, ymin, xmax, ymax);
            }
        }
    }
    return accept;
}

// ============================================================================
// 5. VẼ MINIMAP RADAR 2D TÍCH HỢP CÁC THUẬT TOÁN 2D TRÊN
// ============================================================================
void drawMinimap(int miniX, int miniY, int miniW, int miniH,
                 float playerX, float playerZ,
                 const std::vector<MinimapObstacle>& obstacles,
                 float roadWidth, float viewAhead, float viewBehind) {
    // 1. Vẽ nền mờ của Minimap
    glColor4f(0.05f, 0.08f, 0.12f, 0.75f);
    glBegin(GL_QUADS);
    glVertex2i(miniX, miniY);
    glVertex2i(miniX + miniW, miniY);
    glVertex2i(miniX + miniW, miniY + miniH);
    glVertex2i(miniX, miniY + miniH);
    glEnd();

    // 2. [THUẬT TOÁN BRESENHAM]: Vẽ khung viền radar màu xanh Neon
    glColor3f(0.0f, 0.9f, 0.9f);
    drawLineBresenham(miniX, miniY, miniX + miniW, miniY);
    drawLineBresenham(miniX + miniW, miniY, miniX + miniW, miniY + miniH);
    drawLineBresenham(miniX + miniW, miniY + miniH, miniX, miniY + miniH);
    drawLineBresenham(miniX, miniY + miniH, miniX, miniY);

    // Vùng an toàn hiển thị đường chạy bên trong Minimap
    int roadMinX = miniX + 15;
    int roadMaxX = miniX + miniW - 15;
    int roadMinY = miniY + 10;
    int roadMaxY = miniY + miniH - 10;

    // 3. [THUẬT TOÁN COHEN-SUTHERLAND & BRESENHAM]: Vẽ vạch biên và vạch phân làn
    // Vẽ vạch biên đường 2 bên
    glColor3f(0.8f, 0.8f, 0.8f);
    drawLineBresenham(roadMinX, roadMinY, roadMinX, roadMaxY);
    drawLineBresenham(roadMaxX, roadMinY, roadMaxX, roadMaxY);

    // Vẽ 2 vạch phân làn ở giữa (được cắt xén bằng Cohen-Sutherland đảm bảo không lọt ra ngoài)
    glColor3f(0.4f, 0.5f, 0.6f);
    int laneW = (roadMaxX - roadMinX) / 3;
    int l1_x1 = roadMinX + laneW, l1_y1 = miniY - 10, l1_x2 = roadMinX + laneW, l1_y2 = miniY + miniH + 10;
    int l2_x1 = roadMinX + 2 * laneW, l2_y1 = miniY - 10, l2_x2 = roadMinX + 2 * laneW, l2_y2 = miniY + miniH + 10;

    // Áp dụng thuật toán cắt xén Cohen-Sutherland
    if (cohenSutherlandClip(l1_x1, l1_y1, l1_x2, l1_y2, roadMinX, roadMinY, roadMaxX, roadMaxY)) {
        drawLineBresenham(l1_x1, l1_y1, l1_x2, l1_y2);
    }
    if (cohenSutherlandClip(l2_x1, l2_y1, l2_x2, l2_y2, roadMinX, roadMinY, roadMaxX, roadMaxY)) {
        drawLineBresenham(l2_x1, l2_y1, l2_x2, l2_y2);
    }

    // Hàm chuyển đổi tọa độ thế giới 3D sang tọa độ màn hình 2D Minimap
    auto worldToMinimap = [roadMinX, roadMaxX, roadMinY, roadMaxY, roadWidth, viewAhead, viewBehind](float wx, float wz, int &sx, int &sy) {
        // Tỉ lệ trục X
        float halfW = roadWidth * 0.5f;
        float normX = (wx + halfW) / roadWidth;
        sx = roadMinX + (int)(normX * (roadMaxX - roadMinX));

        // Tỉ lệ trục Z (-viewAhead ở phía trước xe, +viewBehind ở phía sau xe)
        // Trong không gian OpenGL, -Z là hướng nhìn thẳng tới trước
        float totalZ = viewAhead + viewBehind;
        float relZ = -wz + viewBehind; // relZ càng lớn càng ở phía trước
        float normY = relZ / totalZ;
        sy = roadMinY + (int)(normY * (roadMaxY - roadMinY));
    };

    // 4. [THUẬT TOÁN MIDPOINT]: Vẽ các chướng ngại vật trên Minimap
    for (size_t i = 0; i < obstacles.size(); ++i) {
        const MinimapObstacle& obs = obstacles[i];
        if (!obs.active) continue;

        // Chỉ hiển thị chướng ngại vật nằm trong tầm quét radar
        if (obs.z >= -viewAhead && obs.z <= viewBehind) {
            int ox, oy;
            worldToMinimap(obs.x, obs.z, ox, oy);

            // Kiểm tra điểm có nằm trong khung đường chạy không
            if (ox >= roadMinX && ox <= roadMaxX && oy >= roadMinY && oy <= roadMaxY) {
                if (obs.type == 0) {
                    // Thùng gỗ: Màu cam đất
                    glColor3f(0.9f, 0.5f, 0.1f);
                    drawCircleMidpoint(ox, oy, 3, true);
                } else if (obs.type == 1) {
                    // Chóp cụt phản quang: Màu vàng sáng
                    glColor3f(1.0f, 0.85f, 0.1f);
                    drawCircleMidpoint(ox, oy, 3, true);
                } else {
                    // Xe cản đường: Màu đỏ tươi
                    glColor3f(1.0f, 0.2f, 0.2f);
                    drawCircleMidpoint(ox, oy, 4, true);
                }
            }
        }
    }

    // 5. [THUẬT TOÁN MIDPOINT]: Vẽ xe của người chơi (Chấm màu xanh lá / Vàng nổi bật)
    int px, py;
    worldToMinimap(playerX, playerZ, px, py);
    if (px >= roadMinX && px <= roadMaxX && py >= roadMinY && py <= roadMaxY) {
        // Vòng ngoài màu trắng viền
        glColor3f(1.0f, 1.0f, 1.0f);
        drawCircleMidpoint(px, py, 6, false);
        // Vòng trong tô đặc màu xanh lá Neon
        glColor3f(0.1f, 1.0f, 0.2f);
        drawCircleMidpoint(px, py, 4, true);
    }
}
