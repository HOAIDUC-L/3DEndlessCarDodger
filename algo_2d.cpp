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
void drawCircleMidpoint(int xc, int yc, int r, bool fill) {
    int x = 0;
    int y = r;
    int p = 1 - r;

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
static const int INSIDE = 0; // 0000
static const int LEFT   = 1; // 0001
static const int RIGHT  = 2; // 0010
static const int BOTTOM = 4; // 0100
static const int TOP    = 8; // 1000

static int computeOutCode(int x, int y, int xmin, int ymin, int xmax, int ymax) {
    int code = INSIDE;
    if (x < xmin)      code |= LEFT;
    else if (x > xmax) code |= RIGHT;
    if (y < ymin)      code |= BOTTOM;
    else if (y > ymax) code |= TOP;
    return code;
}

bool cohenSutherlandClip(int &x1, int &y1, int &x2, int &y2, int xmin, int ymin, int xmax, int ymax) {
    int code1 = computeOutCode(x1, y1, xmin, ymin, xmax, ymax);
    int code2 = computeOutCode(x2, y2, xmin, ymin, xmax, ymax);
    bool accept = false;

    while (true) {
        if ((code1 | code2) == 0) {
            accept = true;
            break;
        } else if (code1 & code2) {
            break;
        } else {
            int codeOut = code1 ? code1 : code2;
            double x = 0.0, y = 0.0;

            if (codeOut & TOP) {
                x = x1 + (double)(x2 - x1) * (ymax - y1) / (double)(y2 - y1);
                y = ymax;
            } else if (codeOut & BOTTOM) {
                x = x1 + (double)(x2 - x1) * (ymin - y1) / (double)(y2 - y1);
                y = ymin;
            } else if (codeOut & RIGHT) {
                y = y1 + (double)(y2 - y1) * (xmax - x1) / (double)(x2 - x1);
                x = xmax;
            } else if (codeOut & LEFT) {
                y = y1 + (double)(y2 - y1) * (xmin - x1) / (double)(x2 - x1);
                x = xmin;
            }

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
// 5. VẼ MINIMAP RADAR 2D ĐỒNG BỘ CÁC LOẠI PHƯƠNG TIỆN GIAO THÔNG
// ============================================================================
void drawMinimap(int miniX, int miniY, int miniW, int miniH,
                 float playerX, float playerZ,
                 const std::vector<MinimapObstacle>& obstacles,
                 float roadWidth, float viewAhead, float viewBehind) {
    // 1. Nền radar mờ
    glColor4f(0.04f, 0.07f, 0.12f, 0.78f);
    glBegin(GL_QUADS);
    glVertex2i(miniX, miniY);
    glVertex2i(miniX + miniW, miniY);
    glVertex2i(miniX + miniW, miniY + miniH);
    glVertex2i(miniX, miniY + miniH);
    glEnd();

    // 2. Viền radar Neon bằng thuật toán Bresenham
    glColor3f(0.0f, 0.85f, 0.95f);
    drawLineBresenham(miniX, miniY, miniX + miniW, miniY);
    drawLineBresenham(miniX + miniW, miniY, miniX + miniW, miniY + miniH);
    drawLineBresenham(miniX + miniW, miniY + miniH, miniX, miniY + miniH);
    drawLineBresenham(miniX, miniY + miniH, miniX, miniY);

    int roadMinX = miniX + 15;
    int roadMaxX = miniX + miniW - 15;
    int roadMinY = miniY + 10;
    int roadMaxY = miniY + miniH - 10;

    // 3. Vạch biên và vạch phân làn (Cắt xén Cohen-Sutherland + Bresenham)
    glColor3f(0.75f, 0.78f, 0.82f);
    drawLineBresenham(roadMinX, roadMinY, roadMinX, roadMaxY);
    drawLineBresenham(roadMaxX, roadMinY, roadMaxX, roadMaxY);

    glColor3f(0.4f, 0.5f, 0.6f);
    int laneW = (roadMaxX - roadMinX) / 3;
    int l1_x1 = roadMinX + laneW, l1_y1 = miniY - 10, l1_x2 = roadMinX + laneW, l1_y2 = miniY + miniH + 10;
    int l2_x1 = roadMinX + 2 * laneW, l2_y1 = miniY - 10, l2_x2 = roadMinX + 2 * laneW, l2_y2 = miniY + miniH + 10;

    if (cohenSutherlandClip(l1_x1, l1_y1, l1_x2, l1_y2, roadMinX, roadMinY, roadMaxX, roadMaxY)) {
        drawLineBresenham(l1_x1, l1_y1, l1_x2, l1_y2);
    }
    if (cohenSutherlandClip(l2_x1, l2_y1, l2_x2, l2_y2, roadMinX, roadMinY, roadMaxX, roadMaxY)) {
        drawLineBresenham(l2_x1, l2_y1, l2_x2, l2_y2);
    }

    auto worldToMinimap = [roadMinX, roadMaxX, roadMinY, roadMaxY, roadWidth, viewAhead, viewBehind](float wx, float wz, int &sx, int &sy) {
        float halfW = roadWidth * 0.5f;
        float normX = (wx + halfW) / roadWidth;
        sx = roadMinX + (int)(normX * (roadMaxX - roadMinX));

        float totalZ = viewAhead + viewBehind;
        float relZ = -wz + viewBehind;
        float normY = relZ / totalZ;
        sy = roadMinY + (int)(normY * (roadMaxY - roadMinY));
    };

    // 4. [THUẬT TOÁN MIDPOINT]: Vẽ các phương tiện giao thông trên Minimap
    for (size_t i = 0; i < obstacles.size(); ++i) {
        const MinimapObstacle& obs = obstacles[i];
        if (!obs.active) continue;

        if (obs.z >= -viewAhead && obs.z <= viewBehind) {
            int ox, oy;
            worldToMinimap(obs.x, obs.z, ox, oy);

            if (ox >= roadMinX && ox <= roadMaxX && oy >= roadMinY && oy <= roadMaxY) {
                if (obs.type == 1) {
                    // Xe tải Container lớn: Chấm đỏ lớn rực rỡ cảnh báo nguy hiểm
                    glColor3f(1.0f, 0.15f, 0.15f);
                    drawCircleMidpoint(ox, oy, 5, true);
                } else if (obs.type == 2) {
                    // Xe buýt thành phố: Chấm xanh lam to
                    glColor3f(0.2f, 0.75f, 1.0f);
                    drawCircleMidpoint(ox, oy, 4, true);
                } else if (obs.type == 3) {
                    // Xe bán tải Pickup / SUV: Chấm cam nổi bật
                    glColor3f(1.0f, 0.60f, 0.1f);
                    drawCircleMidpoint(ox, oy, 3, true);
                } else {
                    // Xe Sedan / Taxi đô thị: Chấm vàng sáng
                    glColor3f(1.0f, 0.90f, 0.2f);
                    drawCircleMidpoint(ox, oy, 3, true);
                }
            }
        }
    }

    // 5. [THUẬT TOÁN MIDPOINT]: Vẽ Siêu xe người chơi (Chấm xanh Neon có viền trắng)
    int px, py;
    worldToMinimap(playerX, playerZ, px, py);
    if (px >= roadMinX && px <= roadMaxX && py >= roadMinY && py <= roadMaxY) {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawCircleMidpoint(px, py, 6, false);
        glColor3f(0.1f, 1.0f, 0.3f);
        drawCircleMidpoint(px, py, 4, true);
    }
}
