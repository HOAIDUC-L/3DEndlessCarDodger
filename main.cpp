/**
 * ============================================================================
 * MODULE 4: VÒNG LẶP CHÍNH, CAMERA, VA CHẠM & HUD (MAIN.CPP)
 * Dự án: 3D Endless Car Dodger (C++ & OpenGL / FreeGLUT)
 * 
 * Biên dịch:
 *    make
 * Chạy game:
 *    ./game
 * ============================================================================
 */

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <string>
#include <algorithm>

// Nạp các Module thuật toán đồ họa máy tính riêng biệt
#include "algo_2d.h"
#include "algo_3d_geometry.h"
#include "algo_shading_culling.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// 1. CÁC HẰNG SỐ HỆ THỐNG VÀ THIẾT LẬP
// ============================================================================
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;

// Tọa độ 3 làn đường (Trục X)
const float LANES[3] = { -3.5f, 0.0f, 3.5f };
const float ROAD_WIDTH = 12.0f;
const float ROAD_LENGTH = 160.0f;

// Giới hạn vận tốc
const float MIN_SPEED = 18.0f;
const float MAX_SPEED = 65.0f;

// Các góc nhìn Camera (gluLookAt)
enum CameraView {
    CAM_THIRD_PERSON = 0,   // Góc nhìn thứ ba (Sau đuôi xe)
    CAM_TOP_DOWN = 1,       // Góc nhìn toàn cảnh từ trên cao
    CAM_FIRST_PERSON = 2    // Góc nhìn buồng lái / Mui xe
};

// Loại chướng ngại vật
enum ObstacleType {
    OBS_CRATE = 0,          // Thùng gỗ
    OBS_BARRIER = 1,        // Chóp cụt phản quang
    OBS_CAR = 2             // Xe cản đường
};

// Loại cảnh quan hai bên đường
enum SceneryType {
    SCENE_TREE = 0,
    SCENE_HOUSE = 1,
    SCENE_LAMPPOST = 2
};

// ============================================================================
// 2. CẤU TRÚC DỮ LIỆU VA CHẠM AABB & VẬT THỂ
// ============================================================================

/**
 * [THUẬT TOÁN ĐỒ HỌA]: Hộp bao song song các trục tọa độ AABB
 * (Axis-Aligned Bounding Box)
 * Giải thích cho báo cáo:
 * - Để tối ưu hóa việc phát hiện va chạm thời gian thực (Real-time collision detection),
 *   ta bao bọc mỗi vật thể 3D phức tạp bằng một hộp chữ nhật có các mặt song song với
 *   3 trục tọa độ X, Y, Z.
 * - Hai hộp A và B va chạm khi và chỉ khi chúng giao nhau đồng thời trên cả 3 trục:
 *     (A.minX <= B.maxX && A.maxX >= B.minX) &&
 *     (A.minY <= B.maxY && A.maxY >= B.minY) &&
 *     (A.minZ <= B.maxZ && A.maxZ >= B.minZ)
 */
struct AABB {
    float minX, maxX;
    float minY, maxY;
    float minZ, maxZ;

    bool checkCollision(const AABB& o) const {
        return (minX <= o.maxX && maxX >= o.minX) &&
               (minY <= o.maxY && maxY >= o.minY) &&
               (minZ <= o.maxZ && maxZ >= o.minZ);
    }
};

struct Obstacle {
    ObstacleType type;
    int lane;
    float x, y, z;
    float width, height, length;
    float colorR, colorG, colorB;
    bool active;

    AABB getAABB() const {
        AABB b;
        b.minX = x - width * 0.5f;
        b.maxX = x + width * 0.5f;
        b.minY = y;
        b.maxY = y + height;
        b.minZ = z - length * 0.5f;
        b.maxZ = z + length * 0.5f;
        return b;
    }
};

struct SceneryItem {
    SceneryType type;
    float x, y, z;
    float scale;
    float r, g, b;
};

// ============================================================================
// 3. BIẾN TOÀN CỤC CỦA TRÒ CHƠI
// ============================================================================
int currentRenderMode = MODE_GOURAUD_FULL;
CameraView currentCamera = CAM_THIRD_PERSON;
bool isNightMode = false;
bool showMinimap = true; // Phím 'M' để bật/tắt Minimap 2D
bool isGameOver = false;

// Trạng thái xe người chơi
int currentLane = 1;
float playerX = LANES[1];
float playerZ = 0.0f;
float targetX = LANES[1];
float carRollAngle = 0.0f;
float wheelRotationAngle = 0.0f;

// Tốc độ và quãng đường
float currentSpeed = 28.0f;
float targetSpeed = 28.0f;
float totalDistance = 0.0f;
float score = 0.0f;

// Danh sách vật thể trong thế giới 3D
std::vector<Obstacle> obstacles;
std::vector<SceneryItem> sceneryItems;

int lastTime = 0;

// ============================================================================
// 4. KHỞI TẠO MÔI TRƯỜNG VÀ VẬT THỂ
// ============================================================================
void initScenery() {
    sceneryItems.clear();
    for (float z = 20.0f; z >= -ROAD_LENGTH; z -= 14.0f) {
        // Cây xanh 2 bên lề đường
        SceneryItem treeL = { SCENE_TREE, -10.5f - (float)(rand() % 4), 0.0f, z, 0.9f + (rand() % 30) * 0.01f, 0.0f, 0.0f, 0.0f };
        SceneryItem treeR = { SCENE_TREE,  10.5f + (float)(rand() % 4), 0.0f, z - 7.0f, 0.9f + (rand() % 30) * 0.01f, 0.0f, 0.0f, 0.0f };
        sceneryItems.push_back(treeL);
        sceneryItems.push_back(treeR);

        // Nhà cửa xen kẽ
        if ((int)std::abs(z) % 28 == 0) {
            float r = 0.75f + (rand() % 20) * 0.01f;
            float g = 0.75f + (rand() % 20) * 0.01f;
            float b = 0.70f + (rand() % 20) * 0.01f;
            SceneryItem houseL = { SCENE_HOUSE, -18.0f, 0.0f, z, 1.0f, r, g, b };
            SceneryItem houseR = { SCENE_HOUSE,  18.0f, 0.0f, z - 14.0f, 1.0f, b, g, r };
            sceneryItems.push_back(houseL);
            sceneryItems.push_back(houseR);
        }

        // Cột đèn đường
        if ((int)std::abs(z) % 28 == 14) {
            SceneryItem lampL = { SCENE_LAMPPOST, -ROAD_WIDTH * 0.5f - 0.5f, 0.0f, z, 1.0f, 0.0f, 0.0f, 0.0f };
            SceneryItem lampR = { SCENE_LAMPPOST,  ROAD_WIDTH * 0.5f + 0.5f, 0.0f, z, 1.0f, 0.0f, 0.0f, 0.0f };
            sceneryItems.push_back(lampL);
            sceneryItems.push_back(lampR);
        }
    }
}

void initObstacles() {
    obstacles.clear();
    float startZ = -35.0f;
    for (int i = 0; i < 6; ++i) {
        Obstacle obs;
        obs.lane = rand() % 3;
        obs.x = LANES[obs.lane];
        obs.y = 0.0f;
        obs.z = startZ - i * 24.0f - (rand() % 6);
        obs.type = (ObstacleType)(rand() % 3);

        if (obs.type == OBS_CRATE) {
            obs.width = 1.6f; obs.height = 1.6f; obs.length = 1.6f;
        } else if (obs.type == OBS_BARRIER) {
            obs.width = 1.8f; obs.height = 1.2f; obs.length = 1.0f;
        } else {
            obs.width = 1.75f; obs.height = 1.25f; obs.length = 3.8f;
            obs.colorR = 0.2f + (rand() % 70) * 0.01f;
            obs.colorG = 0.2f + (rand() % 70) * 0.01f;
            obs.colorB = 0.2f + (rand() % 70) * 0.01f;
        }
        obs.active = true;
        obstacles.push_back(obs);
    }
}

void resetGame() {
    isGameOver = false;
    currentLane = 1;
    playerX = LANES[1];
    targetX = LANES[1];
    carRollAngle = 0.0f;
    wheelRotationAngle = 0.0f;
    currentSpeed = MIN_SPEED;
    targetSpeed = MIN_SPEED;
    totalDistance = 0.0f;
    score = 0.0f;
    initObstacles();
    initScenery();
}

// ============================================================================
// 5. THIẾT LẬP CAMERA (CAMERA TRANSFORMATIONS)
// ============================================================================
/**
 * [THUẬT TOÁN ĐỒ HỌA]: Biến đổi Góc nhìn Quan sát (Viewing Transformation)
 * Hàm gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ)
 * xây dựng ma trận camera từ:
 *   - Vị trí đặt mắt quan sát (Eye)
 *   - Điểm nhìn ngắm tới (Center)
 *   - Vector chỉ hướng đỉnh đầu (Up)
 */
void setupCamera() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (currentCamera == CAM_THIRD_PERSON) {
        // Góc nhìn thứ ba: Bay bám sau đuôi xe mượt mà
        gluLookAt(
            playerX * 0.65f, 4.2f, playerZ + 7.5f,
            playerX * 0.9f,  1.2f, playerZ - 12.0f,
            0.0f, 1.0f, 0.0f
        );
    } else if (currentCamera == CAM_TOP_DOWN) {
        // Góc nhìn từ trên cao bao quát toàn bộ 3 làn xe
        gluLookAt(
            0.0f, 32.0f, playerZ + 2.0f,
            0.0f,  0.0f, playerZ - 22.0f,
            0.0f,  1.0f, 0.0f
        );
    } else { // CAM_FIRST_PERSON
        // Góc nhìn buồng lái / nắp capo
        gluLookAt(
            playerX, 1.15f, playerZ - 0.6f,
            playerX, 1.0f,  playerZ - 30.0f,
            0.0f,    1.0f,  0.0f
        );
    }
}

// ============================================================================
// 6. GIAO DIỆN CHỮ 2D VÀ HUD (GLUORTHO2D)
// ============================================================================
void renderBitmapString(float x, float y, void *font, const char *string) {
    glRasterPos2f(x, y);
    for (const char *c = string; *c != '\0'; ++c) {
        glutBitmapCharacter(font, *c);
    }
}

void drawHUD() {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Chuyển sang ma trận trực giao 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int winW = glutGet(GLUT_WINDOW_WIDTH);
    int winH = glutGet(GLUT_WINDOW_HEIGHT);

    // 1. Khung mờ thông số trò chơi góc trên bên trái
    glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glVertex2f(15, winH - 15);
    glVertex2f(340, winH - 15);
    glVertex2f(340, winH - 175);
    glVertex2f(15, winH - 175);
    glEnd();

    glColor3f(1.0f, 0.85f, 0.1f);
    renderBitmapString(28, winH - 40, GLUT_BITMAP_HELVETICA_18, "=== 3D CAR DODGER ===");

    char buffer[128];
    glColor3f(1.0f, 1.0f, 1.0f);
    std::snprintf(buffer, sizeof(buffer), "Diem so:     %d", (int)score);
    renderBitmapString(28, winH - 70, GLUT_BITMAP_HELVETICA_18, buffer);

    std::snprintf(buffer, sizeof(buffer), "Toc do:      %d km/h", (int)(currentSpeed * 2.2f));
    renderBitmapString(28, winH - 98, GLUT_BITMAP_HELVETICA_18, buffer);

    std::snprintf(buffer, sizeof(buffer), "Quang duong: %.1f m", totalDistance);
    renderBitmapString(28, winH - 124, GLUT_BITMAP_HELVETICA_12, buffer);

    // Hiển thị trạng thái thuật toán đang bật
    glColor3f(0.3f, 1.0f, 1.0f);
    const char* modeName = (currentRenderMode == MODE_WIREFRAME) ? "1. Wireframe (Khung day)" :
                           (currentRenderMode == MODE_BACKFACE_CULL) ? "2. Back-face Culling" :
                           (currentRenderMode == MODE_DEPTH_FLAT) ? "3. Z-Buffer + Flat" : "4. Gouraud + Phong";
    std::snprintf(buffer, sizeof(buffer), "Thuat toan:  %s", modeName);
    renderBitmapString(28, winH - 148, GLUT_BITMAP_HELVETICA_12, buffer);

    // 2. Vẽ Minimap Radar 2D ở góc trên bên phải (nếu showMinimap = true)
    if (showMinimap) {
        int miniW = 180;
        int miniH = 220;
        int miniX = winW - miniW - 20;
        int miniY = winH - miniH - 20;

        // Chuẩn bị danh sách chướng ngại vật rút gọn
        std::vector<MinimapObstacle> miniObs;
        for (size_t i = 0; i < obstacles.size(); ++i) {
            MinimapObstacle mo;
            mo.x = obstacles[i].x;
            mo.z = obstacles[i].z;
            mo.type = (int)obstacles[i].type;
            mo.active = obstacles[i].active;
            miniObs.push_back(mo);
        }

        // Gọi Module 1 để vẽ Minimap tích hợp Bresenham, Midpoint, Cohen-Sutherland
        drawMinimap(miniX, miniY, miniW, miniH, playerX, playerZ, miniObs, ROAD_WIDTH, 80.0f, 15.0f);

        glColor3f(0.0f, 1.0f, 1.0f);
        renderBitmapString(miniX + 35, miniY + miniH + 6, GLUT_BITMAP_HELVETICA_12, "MINIMAP 2D (Phim M)");
    }

    // 3. Thanh hướng dẫn phím tắt ở cạnh đáy màn hình
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(winW, 0);
    glVertex2f(winW, 40);
    glVertex2f(0, 40);
    glEnd();

    glColor3f(0.85f, 0.85f, 0.85f);
    renderBitmapString(20, 15, GLUT_BITMAP_HELVETICA_12,
        "[A/D, Mui ten]: Lan | [W/S]: Toc do | [1-4]: Che do do hoa | [N]: Ngay/Dem | [C]: Cam | [M]: Minimap | [R]: Choi lai");

    // 4. Màn hình Game Over khi va chạm
    if (isGameOver) {
        float boxW = 520, boxH = 220;
        float bx1 = (winW - boxW) * 0.5f;
        float by1 = (winH - boxH) * 0.5f;

        glColor4f(0.8f, 0.05f, 0.05f, 0.88f);
        glBegin(GL_QUADS);
        glVertex2f(bx1, by1);
        glVertex2f(bx1 + boxW, by1);
        glVertex2f(bx1 + boxW, by1 + boxH);
        glVertex2f(bx1, by1 + boxH);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(bx1, by1);
        glVertex2f(bx1 + boxW, by1);
        glVertex2f(bx1 + boxW, by1 + boxH);
        glVertex2f(bx1, by1 + boxH);
        glEnd();

        glColor3f(1.0f, 1.0f, 0.0f);
        renderBitmapString(bx1 + 145, by1 + 160, GLUT_BITMAP_HELVETICA_18, "=== GAME OVER ===");

        glColor3f(1.0f, 1.0f, 1.0f);
        renderBitmapString(bx1 + 75, by1 + 120, GLUT_BITMAP_HELVETICA_18, "Xe cua ban da va cham chuong ngai vat!");

        std::snprintf(buffer, sizeof(buffer), "Tong diem thu duoc: %d diem", (int)score);
        glColor3f(0.3f, 1.0f, 0.3f);
        renderBitmapString(bx1 + 130, by1 + 80, GLUT_BITMAP_HELVETICA_18, buffer);

        glColor3f(1.0f, 1.0f, 0.2f);
        renderBitmapString(bx1 + 115, by1 + 35, GLUT_BITMAP_HELVETICA_18, "Nhan phim [ R ] de choi lai");
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}

// ============================================================================
// 7. HÀM HIỂN THỊ CHÍNH (DISPLAY CALLBACK)
// ============================================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Kích hoạt chế độ đồ họa từ Module 3 (Khử khuất & Tô bóng)
    applyRenderMode(currentRenderMode);

    // Thiết lập Camera và Nguồn sáng
    setupCamera();
    setupLighting(isNightMode, playerX, playerZ);

    // 1. Vẽ mặt đường 3 làn xe vô tận từ Module 2
    drawRoadAndEnvironment(totalDistance, ROAD_WIDTH, ROAD_LENGTH);

    // 2. Vẽ cảnh quan hai bên đường từ Module 2
    for (size_t i = 0; i < sceneryItems.size(); ++i) {
        const SceneryItem& item = sceneryItems[i];
        if (item.type == SCENE_TREE) {
            drawTree(item.x, item.y, item.z, item.scale);
        } else if (item.type == SCENE_HOUSE) {
            drawHouse(item.x, item.y, item.z, item.scale, item.r, item.g, item.b);
        } else if (item.type == SCENE_LAMPPOST) {
            drawLamppost(item.x, item.y, item.z, item.x < 0, isNightMode);
        }
    }

    // 3. Vẽ các chướng ngại vật từ Module 2
    for (size_t i = 0; i < obstacles.size(); ++i) {
        const Obstacle& obs = obstacles[i];
        if (!obs.active) continue;

        if (obs.type == OBS_CRATE) {
            drawObstacleCrate(obs.x, obs.y, obs.z, obs.width);
        } else if (obs.type == OBS_BARRIER) {
            drawObstacleBarrier(obs.x, obs.y, obs.z, obs.width, obs.height, obs.length);
        } else {
            drawTrafficCar(obs.x, obs.y, obs.z, obs.colorR, obs.colorG, obs.colorB, wheelRotationAngle);
        }
    }

    // 4. Vẽ xe của người chơi từ Module 2
    drawCar(playerX, playerZ, carRollAngle, wheelRotationAngle, isNightMode);

    // 5. Vẽ giao diện HUD 2D và Minimap Radar
    drawHUD();

    glutSwapBuffers();
}

// ============================================================================
// 8. VÒNG LẶP CẬP NHẬT VẬT LÝ VÀ LOGIC (UPDATE / TIMER CALLBACK)
// ============================================================================
void update(int value) {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (lastTime == 0) lastTime = currentTime;
    float dt = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    if (dt > 0.05f) dt = 0.05f;

    if (!isGameOver) {
        // [NỘI SUY TUYẾN TÍNH - LERP]: Di chuyển xe mượt sang làn đích
        float dx = targetX - playerX;
        playerX += dx * 12.0f * dt;

        // Tính góc nghiêng thân xe khi đánh lái
        carRollAngle = dx * 6.5f;

        // Nội suy tốc độ xe
        currentSpeed += (targetSpeed - currentSpeed) * 4.0f * dt;

        float movedDist = currentSpeed * dt;
        totalDistance += movedDist;
        score += movedDist * 1.5f;

        // [BIẾN ĐỔI HÌNH HỌC]: Cập nhật góc quay bánh xe
        float wheelRadius = 0.36f;
        wheelRotationAngle += (movedDist / wheelRadius) * (180.0f / (float)M_PI);
        if (wheelRotationAngle > 36000.0f) wheelRotationAngle -= 36000.0f;

        // Hộp bao va chạm AABB của xe người chơi
        AABB playerBox;
        playerBox.minX = playerX - 0.85f;
        playerBox.maxX = playerX + 0.85f;
        playerBox.minY = 0.0f;
        playerBox.maxY = 1.35f;
        playerBox.minZ = playerZ - 1.9f;
        playerBox.maxZ = playerZ + 1.9f;

        // Cập nhật vị trí và kiểm tra va chạm các chướng ngại vật
        for (size_t i = 0; i < obstacles.size(); ++i) {
            Obstacle& obs = obstacles[i];
            float relativeSpeed = (obs.type == OBS_CAR) ? (currentSpeed - 8.0f) : currentSpeed;
            obs.z += relativeSpeed * dt;

            // [THUẬT TOÁN PHÁT HIỆN VA CHẠM AABB]
            if (playerBox.checkCollision(obs.getAABB())) {
                isGameOver = true;
            }

            // Tái sinh chướng ngại vật khi trôi qua sau xe
            if (obs.z > 15.0f) {
                obs.z = -ROAD_LENGTH + (rand() % 30);
                obs.lane = rand() % 3;
                obs.x = LANES[obs.lane];
                obs.type = (ObstacleType)(rand() % 3);
                if (obs.type == OBS_CAR) {
                    obs.colorR = 0.2f + (rand() % 70) * 0.01f;
                    obs.colorG = 0.2f + (rand() % 70) * 0.01f;
                    obs.colorB = 0.2f + (rand() % 70) * 0.01f;
                }
            }
        }

        // Di chuyển cảnh quan hai bên đường lùi về sau
        for (size_t i = 0; i < sceneryItems.size(); ++i) {
            sceneryItems[i].z += currentSpeed * dt;
            if (sceneryItems[i].z > 25.0f) {
                sceneryItems[i].z -= (ROAD_LENGTH + 40.0f);
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ============================================================================
// 9. XỬ LÝ PHÍM BÀN PHÍM (KEYBOARD CALLBACKS)
// ============================================================================
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        // [CÁC PHÍM CHUYỂN ĐỔI THUẬT TOÁN ĐỒ HỌA MÁY TÍNH]
        case '1':
            currentRenderMode = MODE_WIREFRAME;
            std::printf("[DO HOA] Che do 1: Wireframe (Khung day, tat khu khuat & anh sang)\n");
            break;
        case '2':
            currentRenderMode = MODE_BACKFACE_CULL;
            std::printf("[DO HOA] Che do 2: Back-face Culling (Khu mat sau GL_CCW, ko Z-buffer)\n");
            break;
        case '3':
            currentRenderMode = MODE_DEPTH_FLAT;
            std::printf("[DO HOA] Che do 3: Depth Buffer + Flat Shading (To bong phang)\n");
            break;
        case '4':
            currentRenderMode = MODE_GOURAUD_FULL;
            std::printf("[DO HOA] Che do 4: Depth Buffer + Gouraud Shading + Chieu sang Phong\n");
            break;

        // Bật / Tắt Ngày và Đêm
        case 'n':
        case 'N':
            isNightMode = !isNightMode;
            std::printf("[ANH SANG] Chuyen che do: %s\n", isNightMode ? "Ban Dem (Bat den pha)" : "Ban Ngay (Mat troi)");
            break;

        // Đổi góc nhìn Camera
        case 'c':
        case 'C':
            currentCamera = (CameraView)((currentCamera + 1) % 3);
            std::printf("[CAMERA] Da doi goc nhin sang: %d\n", currentCamera);
            break;

        // Bật / Tắt Minimap Radar 2D
        case 'm':
        case 'M':
            showMinimap = !showMinimap;
            std::printf("[MINIMAP] Da %s Minimap 2D\n", showMinimap ? "BAT" : "TAT");
            break;

        // Chuyển làn xe (A / D)
        case 'a':
        case 'A':
            if (!isGameOver && currentLane > 0) {
                currentLane--;
                targetX = LANES[currentLane];
            }
            break;
        case 'd':
        case 'D':
            if (!isGameOver && currentLane < 2) {
                currentLane++;
                targetX = LANES[currentLane];
            }
            break;

        // Tăng / Giảm tốc độ (W / S)
        case 'w':
        case 'W':
            if (!isGameOver) {
                targetSpeed = std::min(targetSpeed + 10.0f, MAX_SPEED);
            }
            break;
        case 's':
        case 'S':
            if (!isGameOver) {
                targetSpeed = std::max(targetSpeed - 10.0f, MIN_SPEED);
            }
            break;

        // Khởi động lại trò chơi
        case 'r':
        case 'R':
            resetGame();
            std::printf("[GAMEPLAY] Da khoi dong lai tro choi!\n");
            break;

        // Thoát game
        case 27:
            std::printf("Cam on ban da trai nghiem 3D Endless Car Dodger!\n");
            std::exit(0);
            break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if (isGameOver) return;

    switch (key) {
        case GLUT_KEY_LEFT:
            if (currentLane > 0) {
                currentLane--;
                targetX = LANES[currentLane];
            }
            break;
        case GLUT_KEY_RIGHT:
            if (currentLane < 2) {
                currentLane++;
                targetX = LANES[currentLane];
            }
            break;
        case GLUT_KEY_UP:
            targetSpeed = std::min(targetSpeed + 10.0f, MAX_SPEED);
            break;
        case GLUT_KEY_DOWN:
            targetSpeed = std::max(targetSpeed - 10.0f, MIN_SPEED);
            break;
    }
    glutPostRedisplay();
}

// ============================================================================
// 10. THAY ĐỔI KÍCH THƯỚC CỬA SỔ (RESHAPE CALLBACK)
// ============================================================================
void reshape(int width, int height) {
    if (height <= 0) height = 1;
    float aspect = (float)width / (float)height;

    glViewport(0, 0, width, height);

    // [THUẬT TOÁN ĐỒ HỌA]: Chiếu phối cảnh 3D (Perspective Projection)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspect, 0.5, 300.0);

    glMatrixMode(GL_MODELVIEW);
}

// ============================================================================
// 11. HÀM MAIN - ĐIỂM BẮT ĐẦU CHƯƠNG TRÌNH
// ============================================================================
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3D Endless Car Dodger (Modular Architecture) - Do Hoa May Tinh");

    std::printf("==================================================================\n");
    std::printf("  MINI-GAME 3D ENDLESS CAR DODGER - KIEN TRUC DA FILE (MODULAR)   \n");
    std::printf("==================================================================\n");
    std::printf("Phim tat demo cho giang vien:\n");
    std::printf("  - Phim [ 1 ]: Wireframe (Khung day, tat khu khuat & anh sang)\n");
    std::printf("  - Phim [ 2 ]: Back-face Culling (Khu mat sau GL_CCW, ko Z-buffer)\n");
    std::printf("  - Phim [ 3 ]: Depth Buffer + Flat Shading (To bong phang)\n");
    std::printf("  - Phim [ 4 ]: Depth Buffer + Gouraud Shading + Phong Lighting\n");
    std::printf("  - Phim [ N ]: Bat / Tat chu ky Ngay / Dem (Den pha Spotlight)\n");
    std::printf("  - Phim [ C ]: Chuyen doi 3 goc nhin Camera (Sau xe, Toan canh, Mui xe)\n");
    std::printf("  - Phim [ M ]: Bat / Tat Radar Minimap 2D (Bresenham + Midpoint + Clip)\n");
    std::printf("  - Phim [A / D] hoac [Mui ten Trai / Phai]: Chuyen 3 lan xe\n");
    std::printf("  - Phim [W / S] hoac [Mui ten Len / Xuong]: Tang / Giam van toc\n");
    std::printf("  - Phim [ R ]: Choi lai khi Game Over\n");
    std::printf("  - Phim [ESC]: Thoat tro choi\n");
    std::printf("==================================================================\n");

    std::srand((unsigned int)std::time(NULL));

    initObstacles();
    initScenery();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
