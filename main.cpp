/**
 * ============================================================================
 * MODULE 4: VÒNG LẶP CHÍNH, CAMERA, VA CHẠM & HUD (MAIN.CPP)
 * Dự án: 3D Endless Car Dodger (C++ & OpenGL / FreeGLUT)
 * 
 * Nâng cấp thẩm mỹ:
 *  - Khử răng cưa MSAA (GLUT_MULTISAMPLE & glEnable(GL_MULTISAMPLE))
 *  - Sương mù chiều sâu (GL_FOG)
 *  - Hệ thống hạt khói ống xả (Exhaust Particles)
 *  - Cơ chế phím Toggle 1, 2, 3, 4 (Nhấn lần 1 kích hoạt, nhấn lần 2 quay lại)
 *  - Phím Cách (Spacebar) để chơi lại khi Game Over
 *  - Giao diện HUD 2D kính mờ sắc nét
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

// Nạp các Module thuật toán đồ họa
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

const float LANES[3] = { -3.5f, 0.0f, 3.5f };
const float ROAD_WIDTH = 12.0f;
const float ROAD_LENGTH = 160.0f;

const float MIN_SPEED = 18.0f;
const float MAX_SPEED = 65.0f;

enum CameraView {
    CAM_THIRD_PERSON = 0,
    CAM_TOP_DOWN = 1,
    CAM_FIRST_PERSON = 2
};

enum ObstacleType {
    OBS_CRATE = 0,
    OBS_BARRIER = 1,
    OBS_CAR = 2
};

enum SceneryType {
    SCENE_TREE = 0,
    SCENE_HOUSE = 1,
    SCENE_LAMPPOST = 2
};

// ============================================================================
// 2. CẤU TRÚC HẠT KHÓI ỐNG XẢ (PARTICLE SYSTEM) & HỘP BAO AABB
// ============================================================================
struct ExhaustParticle {
    float x, y, z;
    float vx, vy, vz;
    float life;
    float maxLife;
    float size;
    float alpha;
};

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
int previousRenderMode = MODE_GOURAUD_FULL; // Phục vụ cơ chế phím Toggle 1, 2, 3, 4
CameraView currentCamera = CAM_THIRD_PERSON;
bool isNightMode = false;
bool showMinimap = true;
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
std::vector<ExhaustParticle> exhaustParticles;

int lastTime = 0;

// ============================================================================
// 4. KHỞI TẠO MÔI TRƯỜNG VÀ VẬT THỂ
// ============================================================================
void initScenery() {
    sceneryItems.clear();
    for (float z = 20.0f; z >= -ROAD_LENGTH; z -= 14.0f) {
        SceneryItem treeL = { SCENE_TREE, -10.5f - (float)(rand() % 4), 0.0f, z, 0.9f + (rand() % 30) * 0.01f, 0.0f, 0.0f, 0.0f };
        SceneryItem treeR = { SCENE_TREE,  10.5f + (float)(rand() % 4), 0.0f, z - 7.0f, 0.9f + (rand() % 30) * 0.01f, 0.0f, 0.0f, 0.0f };
        sceneryItems.push_back(treeL);
        sceneryItems.push_back(treeR);

        if ((int)std::abs(z) % 28 == 0) {
            float r = 0.75f + (rand() % 20) * 0.01f;
            float g = 0.75f + (rand() % 20) * 0.01f;
            float b = 0.70f + (rand() % 20) * 0.01f;
            SceneryItem houseL = { SCENE_HOUSE, -18.0f, 0.0f, z, 1.0f, r, g, b };
            SceneryItem houseR = { SCENE_HOUSE,  18.0f, 0.0f, z - 14.0f, 1.0f, b, g, r };
            sceneryItems.push_back(houseL);
            sceneryItems.push_back(houseR);
        }

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
    exhaustParticles.clear();
    initObstacles();
    initScenery();
}

// ============================================================================
// 5. QUẢN LÝ HỆ THỐNG HẠT KHÓI ỐNG XẢ (EXHAUST SMOKE PARTICLES)
// ============================================================================
void spawnExhaustParticle(float posX, float posY, float posZ) {
    ExhaustParticle p;
    p.x = posX + (rand() % 20 - 10) * 0.01f;
    p.y = posY + (rand() % 10 - 5) * 0.01f;
    p.z = posZ;

    p.vx = (rand() % 40 - 20) * 0.01f;
    p.vy = 0.5f + (rand() % 30) * 0.02f; // Bay nhẹ lên trên
    p.vz = 4.0f + (rand() % 30) * 0.1f;  // Trôi giạt lùi lại phía sau

    p.life = 0.0f;
    p.maxLife = 0.55f + (rand() % 25) * 0.01f;
    p.size = 0.12f;
    p.alpha = 0.5f;

    exhaustParticles.push_back(p);
}

void updateParticles(float dt) {
    for (size_t i = 0; i < exhaustParticles.size();) {
        ExhaustParticle& p = exhaustParticles[i];
        p.life += dt;
        if (p.life >= p.maxLife) {
            exhaustParticles.erase(exhaustParticles.begin() + i);
        } else {
            p.x += p.vx * dt;
            p.y += p.vy * dt;
            p.z += p.vz * dt;

            float prog = p.life / p.maxLife;
            p.size = 0.12f + prog * 0.40f; // Hạt khói nở to dần
            p.alpha = (1.0f - prog) * 0.45f; // Mờ dần
            ++i;
        }
    }
}

void renderParticles() {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);

    for (size_t i = 0; i < exhaustParticles.size(); ++i) {
        const ExhaustParticle& p = exhaustParticles[i];
        glColor4f(0.85f, 0.85f, 0.88f, p.alpha);

        glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        // Vẽ hạt khói dạng hình khối bát giác mờ
        float r = p.size;
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0.0f, 0.0f, 0.0f);
        for (int a = 0; a <= 8; ++a) {
            float ang = a * 2.0f * (float)M_PI / 8.0f;
            glVertex3f(r * std::cos(ang), r * std::sin(ang), 0.0f);
        }
        glEnd();
        glPopMatrix();
    }

    glPopAttrib();
}

// ============================================================================
// 6. THIẾT LẬP CAMERA (CAMERA TRANSFORMATIONS)
// ============================================================================
void setupCamera() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (currentCamera == CAM_THIRD_PERSON) {
        gluLookAt(
            playerX * 0.65f, 4.2f, playerZ + 7.5f,
            playerX * 0.9f,  1.2f, playerZ - 12.0f,
            0.0f, 1.0f, 0.0f
        );
    } else if (currentCamera == CAM_TOP_DOWN) {
        gluLookAt(
            0.0f, 32.0f, playerZ + 2.0f,
            0.0f,  0.0f, playerZ - 22.0f,
            0.0f,  1.0f, 0.0f
        );
    } else { // CAM_FIRST_PERSON
        gluLookAt(
            playerX, 1.15f, playerZ - 0.6f,
            playerX, 1.0f,  playerZ - 30.0f,
            0.0f,    1.0f,  0.0f
        );
    }
}

// ============================================================================
// 7. GIAO DIỆN CHỮ 2D VÀ HUD (GLUORTHO2D)
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
    glDisable(GL_FOG);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
    glColor4f(0.04f, 0.06f, 0.10f, 0.75f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glVertex2f(15, winH - 15);
    glVertex2f(350, winH - 15);
    glVertex2f(350, winH - 175);
    glVertex2f(15, winH - 175);
    glEnd();

    // Viền khung sắc nét
    glColor4f(0.2f, 0.6f, 0.9f, 0.6f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(15, winH - 15);
    glVertex2f(350, winH - 15);
    glVertex2f(350, winH - 175);
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

    glColor3f(0.3f, 1.0f, 1.0f);
    const char* modeName = (currentRenderMode == MODE_WIREFRAME) ? "1. Wireframe (Khung day)" :
                           (currentRenderMode == MODE_BACKFACE_CULL) ? "2. Back-face Culling" :
                           (currentRenderMode == MODE_DEPTH_FLAT) ? "3. Z-Buffer + Flat" : "4. Gouraud + Phong (Chuan)";
    std::snprintf(buffer, sizeof(buffer), "Thuat toan:  %s", modeName);
    renderBitmapString(28, winH - 150, GLUT_BITMAP_HELVETICA_12, buffer);

    // 2. Minimap Radar 2D ở góc trên bên phải
    if (showMinimap) {
        int miniW = 180;
        int miniH = 220;
        int miniX = winW - miniW - 20;
        int miniY = winH - miniH - 20;

        std::vector<MinimapObstacle> miniObs;
        for (size_t i = 0; i < obstacles.size(); ++i) {
            MinimapObstacle mo;
            mo.x = obstacles[i].x;
            mo.z = obstacles[i].z;
            mo.type = (int)obstacles[i].type;
            mo.active = obstacles[i].active;
            miniObs.push_back(mo);
        }

        drawMinimap(miniX, miniY, miniW, miniH, playerX, playerZ, miniObs, ROAD_WIDTH, 80.0f, 15.0f);

        glColor3f(0.0f, 1.0f, 1.0f);
        renderBitmapString(miniX + 35, miniY + miniH + 6, GLUT_BITMAP_HELVETICA_12, "MINIMAP 2D (Phim M)");
    }

    // 3. Thanh hướng dẫn phím tắt ở cạnh đáy màn hình
    glColor4f(0.02f, 0.04f, 0.08f, 0.85f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(winW, 0);
    glVertex2f(winW, 42);
    glVertex2f(0, 42);
    glEnd();

    glColor3f(0.85f, 0.90f, 0.95f);
    renderBitmapString(20, 16, GLUT_BITMAP_HELVETICA_12,
        "[A/D, Mui ten]: Lan | [W/S]: Toc do | [1-4 Toggle]: Che do do hoa | [N]: Ngay/Dem | [C]: Cam | [M]: Minimap | [Space]: Choi lai");

    // 4. Màn hình Game Over khi va chạm
    if (isGameOver) {
        float boxW = 560, boxH = 230;
        float bx1 = (winW - boxW) * 0.5f;
        float by1 = (winH - boxH) * 0.5f;

        glColor4f(0.12f, 0.02f, 0.02f, 0.92f);
        glBegin(GL_QUADS);
        glVertex2f(bx1, by1);
        glVertex2f(bx1 + boxW, by1);
        glVertex2f(bx1 + boxW, by1 + boxH);
        glVertex2f(bx1, by1 + boxH);
        glEnd();

        // Viền đỏ rực cảnh báo
        glColor3f(1.0f, 0.2f, 0.2f);
        glLineWidth(3.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(bx1, by1);
        glVertex2f(bx1 + boxW, by1);
        glVertex2f(bx1 + boxW, by1 + boxH);
        glVertex2f(bx1, by1 + boxH);
        glEnd();

        glColor3f(1.0f, 0.2f, 0.2f);
        renderBitmapString(bx1 + 165, by1 + 175, GLUT_BITMAP_HELVETICA_18, "=== GAME OVER ===");

        glColor3f(1.0f, 1.0f, 1.0f);
        renderBitmapString(bx1 + 95, by1 + 135, GLUT_BITMAP_HELVETICA_18, "Xe cua ban da va cham chuong ngai vat!");

        std::snprintf(buffer, sizeof(buffer), "Tong diem thu duoc: %d diem", (int)score);
        glColor3f(0.3f, 1.0f, 0.3f);
        renderBitmapString(bx1 + 150, by1 + 95, GLUT_BITMAP_HELVETICA_18, buffer);

        // Hướng dẫn dùng phím Cách để chơi lại
        glColor3f(1.0f, 1.0f, 0.1f);
        renderBitmapString(bx1 + 90, by1 + 45, GLUT_BITMAP_HELVETICA_18, ">> Nhan [ SPACE / PHIM CACH ] de choi lai <<");
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}

// ============================================================================
// 8. HÀM HIỂN THỊ CHÍNH (DISPLAY CALLBACK)
// ============================================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Kích hoạt chế độ đồ họa từ Module 3 (Khử khuất & Tô bóng)
    applyRenderMode(currentRenderMode);

    // Thiết lập Camera, Nguồn sáng và Sương mù
    setupCamera();
    setupLighting(isNightMode, playerX, playerZ);
    setupFog(isNightMode);

    // 1. Vẽ mặt đường chia lưới Subdivided Grid và dãy núi xa
    drawRoadAndEnvironment(totalDistance, ROAD_WIDTH, ROAD_LENGTH);

    // 2. Vẽ cảnh quan hai bên đường
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

    // 3. Vẽ chướng ngại vật
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

    // 4. Vẽ xe của người chơi (có kính chắn gió bán trong suốt và bóng đổ)
    drawCar(playerX, playerZ, carRollAngle, wheelRotationAngle, isNightMode);

    // 5. Vẽ hiệu ứng hạt khói ống xả
    renderParticles();

    // 6. Vẽ giao diện HUD 2D và Minimap Radar
    drawHUD();

    glutSwapBuffers();
}

// ============================================================================
// 9. VÒNG LẶP CẬP NHẬT VẬT LÝ VÀ LOGIC (UPDATE / TIMER CALLBACK)
// ============================================================================
void update(int value) {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    if (lastTime == 0) lastTime = currentTime;
    float dt = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    if (dt > 0.05f) dt = 0.05f;

    if (!isGameOver) {
        // Nội suy di chuyển xe mượt sang làn đích
        float dx = targetX - playerX;
        playerX += dx * 12.0f * dt;
        carRollAngle = dx * 6.5f;

        // Nội suy tốc độ xe
        currentSpeed += (targetSpeed - currentSpeed) * 4.0f * dt;

        float movedDist = currentSpeed * dt;
        totalDistance += movedDist;
        score += movedDist * 1.5f;

        // Cập nhật góc quay bánh xe
        float wheelRadius = 0.36f;
        wheelRotationAngle += (movedDist / wheelRadius) * (180.0f / (float)M_PI);
        if (wheelRotationAngle > 36000.0f) wheelRotationAngle -= 36000.0f;

        // Sinh hạt khói ống xả từ đuôi xe
        spawnExhaustParticle(playerX - 0.55f, 0.22f, playerZ + 2.05f);
        spawnExhaustParticle(playerX + 0.55f, 0.22f, playerZ + 2.05f);

        // Hộp bao va chạm AABB của xe người chơi
        AABB playerBox;
        playerBox.minX = playerX - 0.85f;
        playerBox.maxX = playerX + 0.85f;
        playerBox.minY = 0.0f;
        playerBox.maxY = 1.35f;
        playerBox.minZ = playerZ - 1.9f;
        playerBox.maxZ = playerZ + 1.9f;

        // Cập nhật chướng ngại vật
        for (size_t i = 0; i < obstacles.size(); ++i) {
            Obstacle& obs = obstacles[i];
            float relativeSpeed = (obs.type == OBS_CAR) ? (currentSpeed - 8.0f) : currentSpeed;
            obs.z += relativeSpeed * dt;

            // Kiểm tra va chạm AABB
            if (playerBox.checkCollision(obs.getAABB())) {
                isGameOver = true;
            }

            // Tái sinh chướng ngại vật khi qua sau xe
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

        // Di chuyển cảnh quan hai bên đường
        for (size_t i = 0; i < sceneryItems.size(); ++i) {
            sceneryItems[i].z += currentSpeed * dt;
            if (sceneryItems[i].z > 25.0f) {
                sceneryItems[i].z -= (ROAD_LENGTH + 40.0f);
            }
        }
    }

    // Cập nhật vị trí và độ mờ của hạt khói
    updateParticles(dt);

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ============================================================================
// 10. XỬ LÝ PHÍM BÀN PHÍM (KEYBOARD CALLBACKS)
// ============================================================================
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        // [CƠ CHẾ PHÍM TOGGLE 1, 2, 3, 4]:
        // Nhấn lần 1 kích hoạt chế độ đó, nhấn lần 2 quay trở lại chế độ trước đó
        case '1':
        case '2':
        case '3':
        case '4': {
            int selectedMode = key - '0';
            if (currentRenderMode == selectedMode) {
                // Nhấn lần 2 -> Quay về chế độ mặc định hoặc trước đó
                currentRenderMode = (previousRenderMode != selectedMode) ? previousRenderMode : MODE_GOURAUD_FULL;
                std::printf("[TOGGLE] Da tat che do %d, tro ve che do: %d\n", selectedMode, currentRenderMode);
            } else {
                // Nhấn lần 1 -> Kích hoạt chế độ mới
                previousRenderMode = currentRenderMode;
                currentRenderMode = selectedMode;
                std::printf("[TOGGLE] Da kich hoat che do do hoa: %d\n", currentRenderMode);
            }
            break;
        }

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

        // [NÂNG CẤP]: Phím CÁCH (Spacebar) để chơi lại khi Game Over
        case ' ':
            if (isGameOver) {
                resetGame();
                std::printf("[GAMEPLAY] Nhan phim CACH (Spacebar): Khoi dong lai luot choi moi!\n");
            }
            break;

        // Vẫn giữ phím R để thuận tiện
        case 'r':
        case 'R':
            resetGame();
            std::printf("[GAMEPLAY] Khoi dong lai tro choi!\n");
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
// 11. THAY ĐỔI KÍCH THƯỚC CỬA SỔ (RESHAPE CALLBACK)
// ============================================================================
void reshape(int width, int height) {
    if (height <= 0) height = 1;
    float aspect = (float)width / (float)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspect, 0.5, 300.0);

    glMatrixMode(GL_MODELVIEW);
}

// ============================================================================
// 12. HÀM MAIN - ĐIỂM BẮT ĐẦU CHƯƠNG TRÌNH
// ============================================================================
int main(int argc, char** argv) {
    glutInit(&argc, argv);

    // [NÂNG CẤP KHỬ RĂNG CƯA]: Bật GLUT_MULTISAMPLE cho cạnh đa giác mịn màng
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3D Endless Car Dodger (Enhanced Visuals) - Do Hoa May Tinh");

    std::printf("==================================================================\n");
    std::printf("  3D ENDLESS CAR DODGER - NANG CAP THAM MY & DIEU KHIEN TOGGLE    \n");
    std::printf("==================================================================\n");
    std::printf("Phim tat demo cho giang vien:\n");
    std::printf("  - Phim [1, 2, 3, 4]: Che do Toggle (Nhan lan 1 bat, nhan lan 2 tro ve cu)\n");
    std::printf("      + [ 1 ]: Wireframe (Khung day, tat khu khuat & anh sang)\n");
    std::printf("      + [ 2 ]: Back-face Culling (Khu mat sau GL_CCW, ko Z-buffer)\n");
    std::printf("      + [ 3 ]: Depth Buffer + Flat Shading (To bong phang)\n");
    std::printf("      + [ 4 ]: Depth Buffer + Gouraud Shading + Phong Lighting\n");
    std::printf("  - Phim [ N ]: Bat / Tat chu ky Ngay / Dem (Den pha Spotlight)\n");
    std::printf("  - Phim [ C ]: Chuyen doi 3 goc nhin Camera (Sau xe, Toan canh, Mui xe)\n");
    std::printf("  - Phim [ M ]: Bat / Tat Radar Minimap 2D (Bresenham + Midpoint + Clip)\n");
    std::printf("  - Phim [A / D] hoac [Mui ten Trai / Phai]: Chuyen 3 lan xe\n");
    std::printf("  - Phim [W / S] hoac [Mui ten Len / Xuong]: Tang / Giam van toc\n");
    std::printf("  - Phim [ SPACE ] (Phim Cach) hoac [ R ]: Choi lai khi Game Over\n");
    std::printf("  - Phim [ESC]: Thoat tro choi\n");
    std::printf("==================================================================\n");

    std::srand((unsigned int)std::time(NULL));

    initObstacles();
    initScenery();

    // Cấu hình OpenGL
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_MULTISAMPLE); // Bật khử răng cưa đa mẫu (MSAA)
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
