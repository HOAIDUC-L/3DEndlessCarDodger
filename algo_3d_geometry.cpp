/**
 * ============================================================================
 * MODULE 2: MÔ HÌNH HÓA ĐỐI TƯỢNG 3D & VECTOR PHÁP TUYẾN (ALGO_3D_GEOMETRY.CPP)
 * Dự án: 3D Endless Car Dodger (C++ & OpenGL / FreeGLUT)
 * ============================================================================
 */

#include "algo_3d_geometry.h"
#include "algo_shading_culling.h"
#include <GL/gl.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// 1. CÁC PHÉP TOÁN VECTOR & TÍNH TOÁN VECTOR PHÁP TUYẾN
// ============================================================================
Vector3 crossProduct(const Vector3& a, const Vector3& b) {
    return Vector3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

Vector3 normalize(const Vector3& v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len > 0.00001f) {
        return Vector3(v.x / len, v.y / len, v.z / len);
    }
    return Vector3(0.0f, 1.0f, 0.0f);
}

/**
 * [THUẬT TOÁN ĐỒ HỌA]: Tính Vector Pháp Tuyến bề mặt
 * Giải thích cho báo cáo:
 * - Để xác định hướng phản xạ của ánh sáng theo định luật khúc xạ/phản xạ Lambert,
 *   ta cần vector pháp tuyến N vuông góc với bề mặt đa giác.
 * - Cho 3 đỉnh V0, V1, V2 theo thứ tự ngược chiều kim đồng hồ (CCW):
 *     Vector cạnh 1: E1 = V1 - V0
 *     Vector cạnh 2: E2 = V2 - V0
 *     Vector pháp tuyến: N = Normalize(E1 x E2)
 */
Vector3 calcNormal(const Vector3& v0, const Vector3& v1, const Vector3& v2) {
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;
    return normalize(crossProduct(edge1, edge2));
}

// ============================================================================
// 2. CÁC HÀM DỰNG HÌNH KHỐI 3D NGUYÊN THỦY (PRIMITIVES)
// [LƯU Ý]: Mọi bề mặt đều tuân thủ quy ước đỉnh ngược chiều kim đồng hồ (GL_CCW)
// ============================================================================

/**
 * [HÌNH HỘP CHỮ NHẬT / LẬP PHƯƠNG]
 * Dựng từ 6 mặt tứ giác (Quads), đáy đặt tại Y = 0, tâm X và Z tại gốc tọa độ.
 */
void drawCuboid(float width, float height, float length) {
    float w2 = width * 0.5f;
    float l2 = length * 0.5f;

    glBegin(GL_QUADS);
    // 1. Mặt Trước (+Z): Vector pháp tuyến (0, 0, 1)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w2, 0.0f, l2);
    glVertex3f( w2, 0.0f, l2);
    glVertex3f( w2, height, l2);
    glVertex3f(-w2, height, l2);

    // 2. Mặt Sau (-Z): Vector pháp tuyến (0, 0, -1)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f( w2, 0.0f, -l2);
    glVertex3f(-w2, 0.0f, -l2);
    glVertex3f(-w2, height, -l2);
    glVertex3f( w2, height, -l2);

    // 3. Mặt Trên (+Y): Vector pháp tuyến (0, 1, 0)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-w2, height,  l2);
    glVertex3f( w2, height,  l2);
    glVertex3f( w2, height, -l2);
    glVertex3f(-w2, height, -l2);

    // 4. Mặt Dưới (-Y): Vector pháp tuyến (0, -1, 0)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-w2, 0.0f, -l2);
    glVertex3f( w2, 0.0f, -l2);
    glVertex3f( w2, 0.0f,  l2);
    glVertex3f(-w2, 0.0f,  l2);

    // 5. Mặt Phải (+X): Vector pháp tuyến (1, 0, 0)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w2, 0.0f,  l2);
    glVertex3f(w2, 0.0f, -l2);
    glVertex3f(w2, height, -l2);
    glVertex3f(w2, height,  l2);

    // 6. Mặt Trái (-X): Vector pháp tuyến (-1, 0, 0)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w2, 0.0f, -l2);
    glVertex3f(-w2, 0.0f,  l2);
    glVertex3f(-w2, height,  l2);
    glVertex3f(-w2, height, -l2);
    glEnd();
}

/**
 * [HÌNH CHÓP TỨ GIÁC S.ABCD (PYRAMID)]
 * Đáy ABCD đặt trên mặt phẳng Y = 0, đỉnh S tại (0, height, 0).
 */
void drawPyramid(float baseW, float baseL, float height) {
    float w2 = baseW * 0.5f;
    float l2 = baseL * 0.5f;
    Vector3 apex(0.0f, height, 0.0f);

    Vector3 vA(-w2, 0.0f,  l2); // Trước - Trái
    Vector3 vB( w2, 0.0f,  l2); // Trước - Phải
    Vector3 vC( w2, 0.0f, -l2); // Sau - Phải
    Vector3 vD(-w2, 0.0f, -l2); // Sau - Trái

    glBegin(GL_TRIANGLES);
    // Mặt trước: vA -> vB -> Apex
    Vector3 nFront = calcNormal(vA, vB, apex);
    glNormal3f(nFront.x, nFront.y, nFront.z);
    glVertex3f(vA.x, vA.y, vA.z);
    glVertex3f(vB.x, vB.y, vB.z);
    glVertex3f(apex.x, apex.y, apex.z);

    // Mặt phải: vB -> vC -> Apex
    Vector3 nRight = calcNormal(vB, vC, apex);
    glNormal3f(nRight.x, nRight.y, nRight.z);
    glVertex3f(vB.x, vB.y, vB.z);
    glVertex3f(vC.x, vC.y, vC.z);
    glVertex3f(apex.x, apex.y, apex.z);

    // Mặt sau: vC -> vD -> Apex
    Vector3 nBack = calcNormal(vC, vD, apex);
    glNormal3f(nBack.x, nBack.y, nBack.z);
    glVertex3f(vC.x, vC.y, vC.z);
    glVertex3f(vD.x, vD.y, vD.z);
    glVertex3f(apex.x, apex.y, apex.z);

    // Mặt trái: vD -> vA -> Apex
    Vector3 nLeft = calcNormal(vD, vA, apex);
    glNormal3f(nLeft.x, nLeft.y, nLeft.z);
    glVertex3f(vD.x, vD.y, vD.z);
    glVertex3f(vA.x, vA.y, vA.z);
    glVertex3f(apex.x, apex.y, apex.z);
    glEnd();

    // Mặt đáy: Vector pháp tuyến (0, -1, 0)
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(vA.x, vA.y, vA.z);
    glVertex3f(vD.x, vD.y, vD.z);
    glVertex3f(vC.x, vC.y, vC.z);
    glVertex3f(vB.x, vB.y, vB.z);
    glEnd();
}

/**
 * [HÌNH CHÓP CỤT TỨ GIÁC (TRUNCATED FRUSTUM)]
 */
void drawFrustum(float bW, float bL, float tW, float tL, float height) {
    float bw2 = bW * 0.5f, bl2 = bL * 0.5f;
    float tw2 = tW * 0.5f, tl2 = tL * 0.5f;

    Vector3 b1(-bw2, 0.0f,  bl2), b2( bw2, 0.0f,  bl2), b3( bw2, 0.0f, -bl2), b4(-bw2, 0.0f, -bl2);
    Vector3 t1(-tw2, height, tl2), t2( tw2, height, tl2), t3( tw2, height, -tl2), t4(-tw2, height, -tl2);

    glBegin(GL_QUADS);
    // Mặt trước
    Vector3 nF = calcNormal(b1, b2, t2);
    glNormal3f(nF.x, nF.y, nF.z);
    glVertex3f(b1.x, b1.y, b1.z);
    glVertex3f(b2.x, b2.y, b2.z);
    glVertex3f(t2.x, t2.y, t2.z);
    glVertex3f(t1.x, t1.y, t1.z);

    // Mặt phải
    Vector3 nR = calcNormal(b2, b3, t3);
    glNormal3f(nR.x, nR.y, nR.z);
    glVertex3f(b2.x, b2.y, b2.z);
    glVertex3f(b3.x, b3.y, b3.z);
    glVertex3f(t3.x, t3.y, t3.z);
    glVertex3f(t2.x, t2.y, t2.z);

    // Mặt sau
    Vector3 nB = calcNormal(b3, b4, t4);
    glNormal3f(nB.x, nB.y, nB.z);
    glVertex3f(b3.x, b3.y, b3.z);
    glVertex3f(b4.x, b4.y, b4.z);
    glVertex3f(t4.x, t4.y, t4.z);
    glVertex3f(t3.x, t3.y, t3.z);

    // Mặt trái
    Vector3 nL = calcNormal(b4, b1, t1);
    glNormal3f(nL.x, nL.y, nL.z);
    glVertex3f(b4.x, b4.y, b4.z);
    glVertex3f(b1.x, b1.y, b1.z);
    glVertex3f(t1.x, t1.y, t1.z);
    glVertex3f(t4.x, t4.y, t4.z);

    // Mặt trên (+Y)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(t1.x, t1.y, t1.z);
    glVertex3f(t2.x, t2.y, t2.z);
    glVertex3f(t3.x, t3.y, t3.z);
    glVertex3f(t4.x, t4.y, t4.z);

    // Mặt dưới (-Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(b1.x, b1.y, b1.z);
    glVertex3f(b4.x, b4.y, b4.z);
    glVertex3f(b3.x, b3.y, b3.z);
    glVertex3f(b2.x, b2.y, b2.z);
    glEnd();
}

/**
 * [HÌNH TRỤ TRÒN ĐA GIÁC (CYLINDER)]
 */
void drawCylinder(float radius, float height, int slices) {
    float angleStep = 2.0f * (float)M_PI / (float)slices;

    // Thân trụ: Dựng bằng GL_QUAD_STRIP
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i) {
        float angle = i * angleStep;
        float nx = std::cos(angle);
        float nz = std::sin(angle);
        float x = radius * nx;
        float z = radius * nz;

        glNormal3f(nx, 0.0f, nz);
        glVertex3f(x, height, z);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();

    // Nắp trên (+Y)
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, height, 0.0f);
    for (int i = 0; i <= slices; ++i) {
        float angle = i * angleStep;
        glVertex3f(radius * std::cos(angle), height, radius * std::sin(angle));
    }
    glEnd();

    // Nắp dưới (-Y)
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = slices; i >= 0; --i) {
        float angle = i * angleStep;
        glVertex3f(radius * std::cos(angle), 0.0f, radius * std::sin(angle));
    }
    glEnd();
}

/**
 * [HÌNH NÓN (CONE)]
 */
void drawCone(float radius, float height, int slices) {
    float angleStep = 2.0f * (float)M_PI / (float)slices;
    float slopeLen = std::sqrt(radius * radius + height * height);
    float ny = radius / slopeLen;
    float nFac = height / slopeLen;

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < slices; ++i) {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;

        float x1 = radius * std::cos(a1);
        float z1 = radius * std::sin(a1);
        float x2 = radius * std::cos(a2);
        float z2 = radius * std::sin(a2);

        glNormal3f(std::cos(a1) * nFac, ny, std::sin(a1) * nFac);
        glVertex3f(x1, 0.0f, z1);

        glNormal3f(std::cos(a2) * nFac, ny, std::sin(a2) * nFac);
        glVertex3f(x2, 0.0f, z2);

        glNormal3f(std::cos((a1 + a2) * 0.5f) * nFac, ny, std::sin((a1 + a2) * 0.5f) * nFac);
        glVertex3f(0.0f, height, 0.0f);
    }
    glEnd();

    // Đáy nón (-Y)
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = slices; i >= 0; --i) {
        float a = i * angleStep;
        glVertex3f(radius * std::cos(a), 0.0f, radius * std::sin(a));
    }
    glEnd();
}

// ============================================================================
// 3. CÁC HÀM LẮP GHÉP ĐỐI TƯỢNG PHỨC HỢP VỚI PHÉP BIẾN ĐỔI HÌNH HỌC
// ============================================================================

/**
 * [BÁNH XE Ô TÔ]
 * Tự động lăn tròn theo góc quay bánh xe rotationAngle
 */
void drawWheel(float radius, float width, float rotationAngle) {
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(-width * 0.5f, 0.0f, 0.0f);

    // [BIẾN ĐỔI HÌNH HỌC]: Phép quay quanh trục của bánh xe
    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);

    // 1. Lốp cao su đen
    applyMaterial(0.12f, 0.12f, 0.12f, 16.0f, 0.2f);
    drawCylinder(radius, width, 16);

    // 2. Mâm kim loại 2 bên
    applyMaterial(0.85f, 0.85f, 0.90f, 64.0f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, width + 0.005f, 0.0f);
    drawCylinder(radius * 0.65f, 0.01f, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -0.015f, 0.0f);
    drawCylinder(radius * 0.65f, 0.01f, 12);
    glPopMatrix();

    // 3. Nan hoa nan quạt
    applyMaterial(0.3f, 0.3f, 0.35f, 32.0f);
    glPushMatrix();
    glTranslatef(0.0f, width + 0.01f, 0.0f);
    drawCuboid(radius * 1.1f, 0.01f, radius * 0.2f);
    drawCuboid(radius * 0.2f, 0.01f, radius * 1.1f);
    glPopMatrix();

    glPopMatrix();
}

/**
 * [XE Ô TÔ CỦA NGƯỜI CHƠI]
 */
void drawCar(float x, float z, float rollAngle, float wheelAngle, bool isNight) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    // [BIẾN ĐỔI HÌNH HỌC]: Nghiêng thân xe quanh trục Z khi rẽ chuyển làn
    glRotatef(-rollAngle, 0.0f, 0.0f, 1.0f);

    // 1. Thân gầm xe thể thao đỏ
    applyMaterial(0.92f, 0.12f, 0.15f, 64.0f, 0.7f);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.0f);
    drawCuboid(1.8f, 0.45f, 4.0f);
    glPopMatrix();

    // 2. Mũi xe thấp
    applyMaterial(0.85f, 0.10f, 0.12f, 64.0f, 0.7f);
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, -1.8f);
    drawCuboid(1.72f, 0.35f, 0.9f);
    glPopMatrix();

    // 3. Cabin mui xe + Kính
    applyMaterial(0.18f, 0.22f, 0.28f, 96.0f, 0.9f);
    glPushMatrix();
    glTranslatef(0.0f, 0.65f, 0.2f);
    drawCuboid(1.4f, 0.5f, 2.1f);
    glPopMatrix();

    // Kính chắn gió vát nghiêng
    applyMaterial(0.25f, 0.45f, 0.65f, 128.0f, 0.95f);
    glPushMatrix();
    glTranslatef(0.0f, 0.65f, -0.9f);
    glRotatef(25.0f, 1.0f, 0.0f, 0.0f);
    drawCuboid(1.36f, 0.45f, 0.1f);
    glPopMatrix();

    // 4. Cánh gió thể thao phía sau (Spoiler)
    applyMaterial(0.1f, 0.1f, 0.12f, 32.0f);
    glPushMatrix(); glTranslatef(-0.6f, 0.65f, 1.7f); drawCuboid(0.1f, 0.35f, 0.15f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.6f, 0.65f, 1.7f); drawCuboid(0.1f, 0.35f, 0.15f); glPopMatrix();
    applyMaterial(0.92f, 0.12f, 0.15f, 64.0f, 0.6f);
    glPushMatrix(); glTranslatef(0.0f, 0.98f, 1.7f); drawCuboid(1.9f, 0.08f, 0.45f); glPopMatrix();

    // 5. Đèn pha phía trước
    if (isNight) {
        applyEmissiveMaterial(1.0f, 0.95f, 0.7f, 1.0f);
    } else {
        applyMaterial(0.95f, 0.95f, 0.8f, 64.0f, 0.8f);
    }
    glPushMatrix(); glTranslatef(-0.65f, 0.32f, -2.01f); drawCuboid(0.35f, 0.18f, 0.1f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.65f, 0.32f, -2.01f); drawCuboid(0.35f, 0.18f, 0.1f); glPopMatrix();

    // 6. Đèn hậu đỏ phía sau
    applyEmissiveMaterial(0.95f, 0.05f, 0.05f, isNight ? 0.9f : 0.3f);
    glPushMatrix(); glTranslatef(-0.65f, 0.42f, 2.01f); drawCuboid(0.35f, 0.15f, 0.1f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.65f, 0.42f, 2.01f); drawCuboid(0.35f, 0.15f, 0.1f); glPopMatrix();

    // 7. Bốn bánh xe quay quanh trục
    float wR = 0.36f, wW = 0.30f, wY = 0.36f;
    glPushMatrix(); glTranslatef(-0.95f, wY, -1.25f); drawWheel(wR, wW, wheelAngle); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.95f, wY, -1.25f); drawWheel(wR, wW, wheelAngle); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.95f, wY,  1.25f); drawWheel(wR, wW, wheelAngle); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.95f, wY,  1.25f); drawWheel(wR, wW, wheelAngle); glPopMatrix();

    glPopMatrix();
}

/**
 * [XE CẢN ĐƯỜNG ĐỐI THỦ]
 */
void drawTrafficCar(float x, float y, float z, float r, float g, float b, float wheelAngle) {
    glPushMatrix();
    glTranslatef(x, y, z);

    // Thân xe
    applyMaterial(r, g, b, 48.0f, 0.5f);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.0f);
    drawCuboid(1.75f, 0.45f, 3.8f);
    glPopMatrix();

    // Cabin
    applyMaterial(0.2f, 0.25f, 0.3f, 64.0f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, 0.65f, 0.0f);
    drawCuboid(1.4f, 0.50f, 2.2f);
    glPopMatrix();

    // Đèn hậu đỏ
    applyEmissiveMaterial(0.9f, 0.1f, 0.1f, 0.8f);
    glPushMatrix(); glTranslatef(-0.6f, 0.35f, 1.91f); drawCuboid(0.3f, 0.14f, 0.08f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.6f, 0.35f, 1.91f); drawCuboid(0.3f, 0.14f, 0.08f); glPopMatrix();

    // Bánh xe
    float wR = 0.34f, wW = 0.28f, wY = 0.34f;
    glPushMatrix(); glTranslatef(-0.92f, wY, -1.1f); drawWheel(wR, wW, wheelAngle); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.92f, wY, -1.1f); drawWheel(wR, wW, wheelAngle); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.92f, wY,  1.1f); drawWheel(wR, wW, wheelAngle); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.92f, wY,  1.1f); drawWheel(wR, wW, wheelAngle); glPopMatrix();

    glPopMatrix();
}

/**
 * [CHƯỚNG NGẠI VẬT: THÙNG GỖ]
 */
void drawObstacleCrate(float x, float y, float z, float size) {
    glPushMatrix();
    glTranslatef(x, y, z);
    applyMaterial(0.65f, 0.42f, 0.22f, 16.0f, 0.1f);
    drawCuboid(size, size, size);

    applyMaterial(0.45f, 0.26f, 0.12f, 24.0f, 0.15f);
    glPushMatrix();
    glTranslatef(0.0f, size * 0.5f, 0.0f);
    drawCuboid(size * 1.02f, size * 0.15f, size * 1.02f);
    glPopMatrix();
    glPopMatrix();
}

/**
 * [CHƯỚNG NGẠI VẬT: RÀO CHẮN CHÓP CỤT PHẢN QUANG]
 */
void drawObstacleBarrier(float x, float y, float z, float w, float h, float l) {
    glPushMatrix();
    glTranslatef(x, y, z);
    // Thân chóp cụt màu cam
    applyMaterial(0.95f, 0.45f, 0.05f, 48.0f, 0.4f);
    drawFrustum(w, l, w * 0.4f, l * 0.4f, h);

    // Vạch phản quang trắng
    applyMaterial(0.95f, 0.95f, 0.95f, 64.0f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, h * 0.4f, 0.0f);
    drawFrustum(w * 0.75f, l * 0.75f, w * 0.55f, l * 0.55f, h * 0.25f);
    glPopMatrix();
    glPopMatrix();
}

/**
 * [NGÔI NHÀ DÂN CƯ HAI BÊN ĐƯỜNG]
 */
void drawHouse(float x, float y, float z, float scale, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    float houseW = 5.0f;
    float houseH = 4.2f;
    float houseL = 6.0f;

    // 1. Thân nhà
    applyMaterial(r, g, b, 24.0f, 0.2f);
    drawCuboid(houseW, houseH, houseL);

    // 2. Cửa ra vào
    applyMaterial(0.35f, 0.20f, 0.10f, 16.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, houseL * 0.5f + 0.02f);
    drawCuboid(1.2f, 2.2f, 0.05f);
    glPopMatrix();

    // 3. Cửa sổ
    applyMaterial(0.6f, 0.8f, 0.95f, 80.0f, 0.8f);
    glPushMatrix(); glTranslatef( 1.5f, 2.2f, houseL * 0.5f + 0.02f); drawCuboid(1.0f, 1.0f, 0.05f); glPopMatrix();
    glPushMatrix(); glTranslatef(-1.5f, 2.2f, houseL * 0.5f + 0.02f); drawCuboid(1.0f, 1.0f, 0.05f); glPopMatrix();

    // 4. Mái ngói hình chóp S.ABCD
    applyMaterial(0.75f, 0.22f, 0.15f, 32.0f, 0.3f);
    glPushMatrix();
    glTranslatef(0.0f, houseH, 0.0f);
    drawPyramid(houseW + 0.8f, houseL + 0.8f, 2.8f);
    glPopMatrix();

    glPopMatrix();
}

/**
 * [CÂY XANH HAI BÊN ĐƯỜNG]
 */
void drawTree(float x, float y, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    // Thân cây
    applyMaterial(0.42f, 0.26f, 0.14f, 12.0f, 0.1f);
    drawCylinder(0.35f, 2.5f, 12);

    // Tán lá tầng 1
    applyMaterial(0.12f, 0.55f, 0.18f, 24.0f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 2.0f, 0.0f);
    drawCone(1.8f, 3.0f, 14);
    glPopMatrix();

    // Tán lá tầng 2
    applyMaterial(0.18f, 0.68f, 0.22f, 24.0f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 3.8f, 0.0f);
    drawCone(1.4f, 2.6f, 14);
    glPopMatrix();

    glPopMatrix();
}

/**
 * [CỘT ĐÈN ĐƯỜNG CAO ÁP]
 */
void drawLamppost(float x, float y, float z, bool onLeftSide, bool isNight) {
    glPushMatrix();
    glTranslatef(x, y, z);

    // Cột đứng
    applyMaterial(0.6f, 0.62f, 0.65f, 64.0f, 0.5f);
    drawCylinder(0.18f, 7.0f, 10);

    // Tay đèn
    float armLen = 2.4f;
    float dir = onLeftSide ? 1.0f : -1.0f;
    glPushMatrix();
    glTranslatef(dir * armLen * 0.5f, 6.8f, 0.0f);
    drawCuboid(armLen, 0.16f, 0.16f);
    glPopMatrix();

    // Chao đèn
    glPushMatrix();
    glTranslatef(dir * armLen, 6.6f, 0.0f);
    applyMaterial(0.4f, 0.4f, 0.4f, 32.0f);
    drawCuboid(0.5f, 0.2f, 0.3f);

    // Bóng đèn phát sáng ban đêm
    if (isNight) {
        applyEmissiveMaterial(1.0f, 0.95f, 0.6f, 1.0f);
    } else {
        applyMaterial(0.9f, 0.9f, 0.85f, 32.0f);
    }
    glTranslatef(0.0f, -0.1f, 0.0f);
    drawCuboid(0.35f, 0.08f, 0.22f);
    glPopMatrix();

    glPopMatrix();
}

/**
 * [MẶT ĐƯỜNG VÔ TẬN VÀ PHONG CẢNH THIÊN NHIÊN]
 */
void drawRoadAndEnvironment(float totalDistance, float roadWidth, float roadLength) {
    // 1. Dải cỏ xanh rộng lớn
    applyMaterial(0.22f, 0.65f, 0.25f, 8.0f, 0.05f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    // Cỏ bên trái
    glVertex3f(-70.0f, -0.05f,  30.0f);
    glVertex3f(-roadWidth * 0.5f - 1.2f, -0.05f,  30.0f);
    glVertex3f(-roadWidth * 0.5f - 1.2f, -0.05f, -roadLength);
    glVertex3f(-70.0f, -0.05f, -roadLength);

    // Cỏ bên phải
    glVertex3f( roadWidth * 0.5f + 1.2f, -0.05f,  30.0f);
    glVertex3f( 70.0f, -0.05f,  30.0f);
    glVertex3f( 70.0f, -0.05f, -roadLength);
    glVertex3f( roadWidth * 0.5f + 1.2f, -0.05f, -roadLength);
    glEnd();

    // 2. Vỉa hè / Lề đường lát đá xám
    applyMaterial(0.72f, 0.72f, 0.74f, 16.0f);
    glPushMatrix();
    glTranslatef(-roadWidth * 0.5f - 0.6f, 0.05f, -roadLength * 0.5f + 15.0f);
    drawCuboid(1.2f, 0.1f, roadLength + 40.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef( roadWidth * 0.5f + 0.6f, 0.05f, -roadLength * 0.5f + 15.0f);
    drawCuboid(1.2f, 0.1f, roadLength + 40.0f);
    glPopMatrix();

    // 3. Mặt đường nhựa Asphalt màu xám đen
    applyMaterial(0.18f, 0.18f, 0.20f, 16.0f, 0.1f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-roadWidth * 0.5f, 0.0f,  30.0f);
    glVertex3f( roadWidth * 0.5f, 0.0f,  30.0f);
    glVertex3f( roadWidth * 0.5f, 0.0f, -roadLength);
    glVertex3f(-roadWidth * 0.5f, 0.0f, -roadLength);
    glEnd();

    // 4. [CHUYỂN ĐỘNG VÔ TẬN]: Vạch kẻ đường phân làn đứt đoạn
    applyMaterial(0.95f, 0.95f, 0.95f, 64.0f, 0.6f);
    float dashLength = 3.5f;
    float gapLength = 3.5f;
    float cycle = dashLength + gapLength;
    float offsetZ = std::fmod(totalDistance, cycle);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (float z = -roadLength; z <= 30.0f; z += cycle) {
        float curZ = z + offsetZ;
        // Phân làn giữa làn 0 và 1 (X = -1.75)
        glVertex3f(-1.85f, 0.02f, curZ);
        glVertex3f(-1.65f, 0.02f, curZ);
        glVertex3f(-1.65f, 0.02f, curZ - dashLength);
        glVertex3f(-1.85f, 0.02f, curZ - dashLength);

        // Phân làn giữa làn 1 và 2 (X = +1.75)
        glVertex3f( 1.65f, 0.02f, curZ);
        glVertex3f( 1.85f, 0.02f, curZ);
        glVertex3f( 1.85f, 0.02f, curZ - dashLength);
        glVertex3f( 1.65f, 0.02f, curZ - dashLength);
    }

    // Vạch vàng viền mép đường 2 bên
    applyMaterial(0.95f, 0.80f, 0.10f, 32.0f);
    glVertex3f(-roadWidth * 0.5f + 0.15f, 0.02f,  30.0f);
    glVertex3f(-roadWidth * 0.5f + 0.35f, 0.02f,  30.0f);
    glVertex3f(-roadWidth * 0.5f + 0.35f, 0.02f, -roadLength);
    glVertex3f(-roadWidth * 0.5f + 0.15f, 0.02f, -roadLength);

    glVertex3f( roadWidth * 0.5f - 0.35f, 0.02f,  30.0f);
    glVertex3f( roadWidth * 0.5f - 0.15f, 0.02f,  30.0f);
    glVertex3f( roadWidth * 0.5f - 0.15f, 0.02f, -roadLength);
    glVertex3f( roadWidth * 0.5f - 0.35f, 0.02f, -roadLength);
    glEnd();
}
