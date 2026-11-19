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
#include <algorithm>

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

Vector3 calcNormal(const Vector3& v0, const Vector3& v1, const Vector3& v2) {
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;
    return normalize(crossProduct(edge1, edge2));
}

// ============================================================================
// 2. CÁC HÀM DỰNG HÌNH KHỐI 3D NGUYÊN THỦY (PRIMITIVES)
// [LƯU Ý]: Mọi bề mặt đều tuân thủ quy ước đỉnh ngược chiều kim đồng hồ (GL_CCW)
// ============================================================================
void drawCuboid(float width, float height, float length) {
    float w2 = width * 0.5f;
    float l2 = length * 0.5f;

    glBegin(GL_QUADS);
    // 1. Mặt Trước (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w2, 0.0f, l2);
    glVertex3f( w2, 0.0f, l2);
    glVertex3f( w2, height, l2);
    glVertex3f(-w2, height, l2);

    // 2. Mặt Sau (-Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f( w2, 0.0f, -l2);
    glVertex3f(-w2, 0.0f, -l2);
    glVertex3f(-w2, height, -l2);
    glVertex3f( w2, height, -l2);

    // 3. Mặt Trên (+Y)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-w2, height,  l2);
    glVertex3f( w2, height,  l2);
    glVertex3f( w2, height, -l2);
    glVertex3f(-w2, height, -l2);

    // 4. Mặt Dưới (-Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-w2, 0.0f, -l2);
    glVertex3f( w2, 0.0f, -l2);
    glVertex3f( w2, 0.0f,  l2);
    glVertex3f(-w2, 0.0f,  l2);

    // 5. Mặt Phải (+X)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(w2, 0.0f,  l2);
    glVertex3f(w2, 0.0f, -l2);
    glVertex3f(w2, height, -l2);
    glVertex3f(w2, height,  l2);

    // 6. Mặt Trái (-X)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-w2, 0.0f, -l2);
    glVertex3f(-w2, 0.0f,  l2);
    glVertex3f(-w2, height,  l2);
    glVertex3f(-w2, height, -l2);
    glEnd();
}

void drawPyramid(float baseW, float baseL, float height) {
    float w2 = baseW * 0.5f;
    float l2 = baseL * 0.5f;
    Vector3 apex(0.0f, height, 0.0f);

    Vector3 vA(-w2, 0.0f,  l2);
    Vector3 vB( w2, 0.0f,  l2);
    Vector3 vC( w2, 0.0f, -l2);
    Vector3 vD(-w2, 0.0f, -l2);

    glBegin(GL_TRIANGLES);
    Vector3 nFront = calcNormal(vA, vB, apex);
    glNormal3f(nFront.x, nFront.y, nFront.z);
    glVertex3f(vA.x, vA.y, vA.z); glVertex3f(vB.x, vB.y, vB.z); glVertex3f(apex.x, apex.y, apex.z);

    Vector3 nRight = calcNormal(vB, vC, apex);
    glNormal3f(nRight.x, nRight.y, nRight.z);
    glVertex3f(vB.x, vB.y, vB.z); glVertex3f(vC.x, vC.y, vC.z); glVertex3f(apex.x, apex.y, apex.z);

    Vector3 nBack = calcNormal(vC, vD, apex);
    glNormal3f(nBack.x, nBack.y, nBack.z);
    glVertex3f(vC.x, vC.y, vC.z); glVertex3f(vD.x, vD.y, vD.z); glVertex3f(apex.x, apex.y, apex.z);

    Vector3 nLeft = calcNormal(vD, vA, apex);
    glNormal3f(nLeft.x, nLeft.y, nLeft.z);
    glVertex3f(vD.x, vD.y, vD.z); glVertex3f(vA.x, vA.y, vA.z); glVertex3f(apex.x, apex.y, apex.z);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(vA.x, vA.y, vA.z); glVertex3f(vD.x, vD.y, vD.z);
    glVertex3f(vC.x, vC.y, vC.z); glVertex3f(vB.x, vB.y, vB.z);
    glEnd();
}

void drawFrustum(float bW, float bL, float tW, float tL, float height) {
    float bw2 = bW * 0.5f, bl2 = bL * 0.5f;
    float tw2 = tW * 0.5f, tl2 = tL * 0.5f;

    Vector3 b1(-bw2, 0.0f,  bl2), b2( bw2, 0.0f,  bl2), b3( bw2, 0.0f, -bl2), b4(-bw2, 0.0f, -bl2);
    Vector3 t1(-tw2, height, tl2), t2( tw2, height, tl2), t3( tw2, height, -tl2), t4(-tw2, height, -tl2);

    glBegin(GL_QUADS);
    Vector3 nF = calcNormal(b1, b2, t2);
    glNormal3f(nF.x, nF.y, nF.z);
    glVertex3f(b1.x, b1.y, b1.z); glVertex3f(b2.x, b2.y, b2.z); glVertex3f(t2.x, t2.y, t2.z); glVertex3f(t1.x, t1.y, t1.z);

    Vector3 nR = calcNormal(b2, b3, t3);
    glNormal3f(nR.x, nR.y, nR.z);
    glVertex3f(b2.x, b2.y, b2.z); glVertex3f(b3.x, b3.y, b3.z); glVertex3f(t3.x, t3.y, t3.z); glVertex3f(t2.x, t2.y, t2.z);

    Vector3 nB = calcNormal(b3, b4, t4);
    glNormal3f(nB.x, nB.y, nB.z);
    glVertex3f(b3.x, b3.y, b3.z); glVertex3f(b4.x, b4.y, b4.z); glVertex3f(t4.x, t4.y, t4.z); glVertex3f(t3.x, t3.y, t3.z);

    Vector3 nL = calcNormal(b4, b1, t1);
    glNormal3f(nL.x, nL.y, nL.z);
    glVertex3f(b4.x, b4.y, b4.z); glVertex3f(b1.x, b1.y, b1.z); glVertex3f(t1.x, t1.y, t1.z); glVertex3f(t4.x, t4.y, t4.z);

    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(t1.x, t1.y, t1.z); glVertex3f(t2.x, t2.y, t2.z); glVertex3f(t3.x, t3.y, t3.z); glVertex3f(t4.x, t4.y, t4.z);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(b1.x, b1.y, b1.z); glVertex3f(b4.x, b4.y, b4.z); glVertex3f(b3.x, b3.y, b3.z); glVertex3f(b2.x, b2.y, b2.z);
    glEnd();
}

void drawCylinder(float radius, float height, int slices) {
    float angleStep = 2.0f * (float)M_PI / (float)slices;

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

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, height, 0.0f);
    for (int i = 0; i <= slices; ++i) {
        float angle = i * angleStep;
        glVertex3f(radius * std::cos(angle), height, radius * std::sin(angle));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = slices; i >= 0; --i) {
        float angle = i * angleStep;
        glVertex3f(radius * std::cos(angle), 0.0f, radius * std::sin(angle));
    }
    glEnd();
}

void drawCone(float radius, float height, int slices) {
    float angleStep = 2.0f * (float)M_PI / (float)slices;
    float slopeLen = std::sqrt(radius * radius + height * height);
    float ny = radius / slopeLen;
    float nFac = height / slopeLen;

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < slices; ++i) {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;

        float x1 = radius * std::cos(a1); float z1 = radius * std::sin(a1);
        float x2 = radius * std::cos(a2); float z2 = radius * std::sin(a2);

        glNormal3f(std::cos(a1) * nFac, ny, std::sin(a1) * nFac);
        glVertex3f(x1, 0.0f, z1);

        glNormal3f(std::cos(a2) * nFac, ny, std::sin(a2) * nFac);
        glVertex3f(x2, 0.0f, z2);

        glNormal3f(std::cos((a1 + a2) * 0.5f) * nFac, ny, std::sin((a1 + a2) * 0.5f) * nFac);
        glVertex3f(0.0f, height, 0.0f);
    }
    glEnd();

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
// 3. CÁC HÀM LẮP GHÉP ĐỐI TƯỢNG PHỨC HỢP & NÂNG CẤP THẨM MỸ
// ============================================================================

/**
 * [THUẬT TOÁN ĐỔ BÓNG PHẲNG GẦM XE (PLANAR SHADOW)]
 * Sử dụng GL_BLEND vẽ một vệt bóng đen bán trong suốt nằm ngay sát mặt đường (Y = 0.015m)
 * giúp chiếc xe như bám chặt vào mặt đường và tăng chiều sâu không gian.
 */
void drawVehicleShadow(float x, float z, float width, float length) {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);

    glColor4f(0.01f, 0.01f, 0.01f, 0.42f);

    float rx = width * 0.55f;
    float rz = length * 0.52f;
    int segments = 16;

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(x, 0.015f, z);
    for (int i = 0; i <= segments; ++i) {
        float ang = i * 2.0f * (float)M_PI / (float)segments;
        glVertex3f(x + rx * std::cos(ang), 0.015f, z + rz * std::sin(ang));
    }
    glEnd();

    glPopAttrib();
}

void drawWheel(float radius, float width, float rotationAngle) {
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(-width * 0.5f, 0.0f, 0.0f);

    glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);

    // Lốp cao su đen
    applyMaterial(0.12f, 0.12f, 0.12f, 16.0f, 0.2f);
    drawCylinder(radius, width, 16);

    // Mâm kim loại 2 bên
    applyMaterial(0.85f, 0.85f, 0.90f, 64.0f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, width + 0.005f, 0.0f);
    drawCylinder(radius * 0.65f, 0.01f, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -0.015f, 0.0f);
    drawCylinder(radius * 0.65f, 0.01f, 12);
    glPopMatrix();

    // Nan hoa nan quạt
    applyMaterial(0.3f, 0.3f, 0.35f, 32.0f);
    glPushMatrix();
    glTranslatef(0.0f, width + 0.01f, 0.0f);
    drawCuboid(radius * 1.1f, 0.01f, radius * 0.2f);
    drawCuboid(radius * 0.2f, 0.01f, radius * 1.1f);
    glPopMatrix();

    glPopMatrix();
}

/**
 * [XE CỦA NGƯỜI CHƠI VỚI KÍNH CHẮN GIÓ BÁN TRONG SUỐT]
 */
void drawCar(float x, float z, float rollAngle, float wheelAngle, bool isNight) {
    // 1. Vẽ bóng đổ gầm xe trước
    drawVehicleShadow(x, z, 2.2f, 4.4f);

    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(-rollAngle, 0.0f, 0.0f, 1.0f);

    // Thân gầm xe thể thao đỏ
    applyMaterial(0.92f, 0.12f, 0.15f, 64.0f, 0.7f);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.0f);
    drawCuboid(1.8f, 0.45f, 4.0f);
    glPopMatrix();

    // Mũi xe thấp khí động học
    applyMaterial(0.85f, 0.10f, 0.12f, 64.0f, 0.7f);
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, -1.8f);
    drawCuboid(1.72f, 0.35f, 0.9f);
    glPopMatrix();

    // Khung mui cabin
    applyMaterial(0.18f, 0.20f, 0.24f, 64.0f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, 0.65f, 0.2f);
    drawCuboid(1.4f, 0.5f, 2.1f);
    glPopMatrix();

    // [NÂNG CẤP THẨM MỸ]: Kính chắn gió trước và kính hông bán trong suốt (Transparent Glass)
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.25f, 0.55f, 0.85f, 0.55f);
    GLfloat glassMat[4] = { 0.25f, 0.55f, 0.85f, 0.55f };
    GLfloat glassSpec[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glassMat);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glassSpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0f);

    glPushMatrix();
    glTranslatef(0.0f, 0.65f, -0.9f);
    glRotatef(25.0f, 1.0f, 0.0f, 0.0f);
    drawCuboid(1.36f, 0.45f, 0.1f);
    glPopMatrix();
    glPopAttrib();

    // Cánh gió thể thao phía sau (Spoiler)
    applyMaterial(0.1f, 0.1f, 0.12f, 32.0f);
    glPushMatrix(); glTranslatef(-0.6f, 0.65f, 1.7f); drawCuboid(0.1f, 0.35f, 0.15f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.6f, 0.65f, 1.7f); drawCuboid(0.1f, 0.35f, 0.15f); glPopMatrix();
    applyMaterial(0.92f, 0.12f, 0.15f, 64.0f, 0.6f);
    glPushMatrix(); glTranslatef(0.0f, 0.98f, 1.7f); drawCuboid(1.9f, 0.08f, 0.45f); glPopMatrix();

    // Đèn pha phía trước
    if (isNight) {
        applyEmissiveMaterial(1.0f, 0.95f, 0.7f, 1.0f);
    } else {
        applyMaterial(0.95f, 0.95f, 0.8f, 64.0f, 0.8f);
    }
    glPushMatrix(); glTranslatef(-0.65f, 0.32f, -2.01f); drawCuboid(0.35f, 0.18f, 0.1f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.65f, 0.32f, -2.01f); drawCuboid(0.35f, 0.18f, 0.1f); glPopMatrix();

    // Đèn hậu đỏ
    applyEmissiveMaterial(0.95f, 0.05f, 0.05f, isNight ? 0.9f : 0.3f);
    glPushMatrix(); glTranslatef(-0.65f, 0.42f, 2.01f); drawCuboid(0.35f, 0.15f, 0.1f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.65f, 0.42f, 2.01f); drawCuboid(0.35f, 0.15f, 0.1f); glPopMatrix();

    // Bốn bánh xe lăn
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
    // Đổ bóng dưới gầm xe đối thủ
    drawVehicleShadow(x, z, 2.1f, 4.2f);

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

void drawObstacleCrate(float x, float y, float z, float size) {
    drawVehicleShadow(x, z, size * 1.1f, size * 1.1f);
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

void drawObstacleBarrier(float x, float y, float z, float w, float h, float l) {
    drawVehicleShadow(x, z, w * 1.05f, l * 1.05f);
    glPushMatrix();
    glTranslatef(x, y, z);
    applyMaterial(0.95f, 0.45f, 0.05f, 48.0f, 0.4f);
    drawFrustum(w, l, w * 0.4f, l * 0.4f, h);

    applyMaterial(0.95f, 0.95f, 0.95f, 64.0f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, h * 0.4f, 0.0f);
    drawFrustum(w * 0.75f, l * 0.75f, w * 0.55f, l * 0.55f, h * 0.25f);
    glPopMatrix();
    glPopMatrix();
}

void drawHouse(float x, float y, float z, float scale, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    float houseW = 5.0f;
    float houseH = 4.2f;
    float houseL = 6.0f;

    applyMaterial(r, g, b, 24.0f, 0.2f);
    drawCuboid(houseW, houseH, houseL);

    applyMaterial(0.35f, 0.20f, 0.10f, 16.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, houseL * 0.5f + 0.02f);
    drawCuboid(1.2f, 2.2f, 0.05f);
    glPopMatrix();

    applyMaterial(0.6f, 0.8f, 0.95f, 80.0f, 0.8f);
    glPushMatrix(); glTranslatef( 1.5f, 2.2f, houseL * 0.5f + 0.02f); drawCuboid(1.0f, 1.0f, 0.05f); glPopMatrix();
    glPushMatrix(); glTranslatef(-1.5f, 2.2f, houseL * 0.5f + 0.02f); drawCuboid(1.0f, 1.0f, 0.05f); glPopMatrix();

    applyMaterial(0.75f, 0.22f, 0.15f, 32.0f, 0.3f);
    glPushMatrix();
    glTranslatef(0.0f, houseH, 0.0f);
    drawPyramid(houseW + 0.8f, houseL + 0.8f, 2.8f);
    glPopMatrix();

    glPopMatrix();
}

void drawTree(float x, float y, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    applyMaterial(0.42f, 0.26f, 0.14f, 12.0f, 0.1f);
    drawCylinder(0.35f, 2.5f, 12);

    applyMaterial(0.12f, 0.55f, 0.18f, 24.0f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 2.0f, 0.0f);
    drawCone(1.8f, 3.0f, 14);
    glPopMatrix();

    applyMaterial(0.18f, 0.68f, 0.22f, 24.0f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 3.8f, 0.0f);
    drawCone(1.4f, 2.6f, 14);
    glPopMatrix();

    glPopMatrix();
}

void drawLamppost(float x, float y, float z, bool onLeftSide, bool isNight) {
    glPushMatrix();
    glTranslatef(x, y, z);

    applyMaterial(0.6f, 0.62f, 0.65f, 64.0f, 0.5f);
    drawCylinder(0.18f, 7.0f, 10);

    float armLen = 2.4f;
    float dir = onLeftSide ? 1.0f : -1.0f;
    glPushMatrix();
    glTranslatef(dir * armLen * 0.5f, 6.8f, 0.0f);
    drawCuboid(armLen, 0.16f, 0.16f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(dir * armLen, 6.6f, 0.0f);
    applyMaterial(0.4f, 0.4f, 0.4f, 32.0f);
    drawCuboid(0.5f, 0.2f, 0.3f);

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
 * [NÂNG CẤP CẢNH QUAN: DÃY NÚI HÙNG VĨ Ở XA CHÂN TRỜI]
 * Kết hợp cùng sương mù GL_FOG tạo ra phông nền tự nhiên che khuất mặt cắt xa
 */
void drawMountains(float roadLength) {
    float startZ = -roadLength - 30.0f;
    // Dãy núi phía xa
    float mountainData[][5] = {
        // x, y, z, baseSize, height
        { -90.0f,  0.0f, startZ - 20.0f, 80.0f, 42.0f },
        { -45.0f,  0.0f, startZ - 40.0f, 95.0f, 52.0f },
        {   0.0f,  0.0f, startZ - 50.0f, 110.0f, 60.0f },
        {  50.0f,  0.0f, startZ - 35.0f, 90.0f, 48.0f },
        { 100.0f,  0.0f, startZ - 25.0f, 85.0f, 40.0f }
    };

    applyMaterial(0.28f, 0.35f, 0.45f, 16.0f, 0.05f);
    for (int i = 0; i < 5; ++i) {
        glPushMatrix();
        glTranslatef(mountainData[i][0], mountainData[i][1], mountainData[i][2]);
        drawPyramid(mountainData[i][3], mountainData[i][3], mountainData[i][4]);

        // Đỉnh núi phủ tuyết trắng
        applyMaterial(0.90f, 0.92f, 0.98f, 32.0f, 0.2f);
        glTranslatef(0.0f, mountainData[i][4] * 0.72f, 0.0f);
        drawPyramid(mountainData[i][3] * 0.32f, mountainData[i][3] * 0.32f, mountainData[i][4] * 0.28f);
        glPopMatrix();
    }
}

/**
 * [CHIA LƯỚI MẶT ĐƯỜNG (SUBDIVIDED GRID) & ĐÓN ĐÈN PHA SPOTLIGHT]
 * Giải thích nguyên lý kỹ thuật cho báo cáo:
 * - Trong mô hình chiếu sáng Gouraud (GL_SMOOTH), OpenGL chỉ tính toán màu sắc ánh sáng
 *   tại các đỉnh đa giác rồi nội suy vào trong mặt phẳng.
 * - Nếu mặt đường chỉ là 1 tứ giác khổng lồ từ z = +30 đến z = -160, các đỉnh nằm quá xa
 *   tâm chiếu của đèn pha Spotlight GL_LIGHT1, dẫn tới ánh đèn không thể hiển thị trên mặt đường.
 * - Bằng cách chia nhỏ mặt đường thành lưới mắt cáo (dx = 1.0m, dz = 1.8m), hàng nghìn đỉnh
 *   sẽ nhận chính xác cường độ sáng từ phương trình Spotlight, tạo ra vệt sáng nón loang tròn
 *   tự nhiên và mượt mà trên mặt đường nhựa vào ban đêm!
 */
void drawRoadAndEnvironment(float totalDistance, float roadWidth, float roadLength) {
    // 1. Dãy núi phía chân trời
    drawMountains(roadLength);

    // 2. Dải cỏ xanh rộng lớn
    applyMaterial(0.22f, 0.65f, 0.25f, 8.0f, 0.05f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-120.0f, -0.05f,  30.0f);
    glVertex3f(-roadWidth * 0.5f - 1.2f, -0.05f,  30.0f);
    glVertex3f(-roadWidth * 0.5f - 1.2f, -0.05f, -roadLength);
    glVertex3f(-120.0f, -0.05f, -roadLength);

    glVertex3f( roadWidth * 0.5f + 1.2f, -0.05f,  30.0f);
    glVertex3f( 120.0f, -0.05f,  30.0f);
    glVertex3f( 120.0f, -0.05f, -roadLength);
    glVertex3f( roadWidth * 0.5f + 1.2f, -0.05f, -roadLength);
    glEnd();

    // 3. Vỉa hè / Lề đường lát đá xám
    applyMaterial(0.72f, 0.72f, 0.74f, 16.0f);
    glPushMatrix();
    glTranslatef(-roadWidth * 0.5f - 0.6f, 0.05f, -roadLength * 0.5f + 15.0f);
    drawCuboid(1.2f, 0.1f, roadLength + 40.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef( roadWidth * 0.5f + 0.6f, 0.05f, -roadLength * 0.5f + 15.0f);
    drawCuboid(1.2f, 0.1f, roadLength + 40.0f);
    glPopMatrix();

    // 4. [CHIA LƯỚI ĐA GIÁC (SUBDIVIDED ROAD GRID)]: Mặt đường nhựa đón đèn pha Spotlight
    applyMaterial(0.18f, 0.18f, 0.20f, 24.0f, 0.15f);
    glNormal3f(0.0f, 1.0f, 0.0f);

    int nx = 12; // Chia chiều rộng thành 12 dải
    float dx = roadWidth / (float)nx;
    float dz = 1.8f; // Bước lưới dọc trục Z

    glBegin(GL_QUADS);
    for (float z = 30.0f; z > -roadLength; z -= dz) {
        float zNext = std::max(z - dz, -roadLength);
        for (int i = 0; i < nx; ++i) {
            float x1 = -roadWidth * 0.5f + i * dx;
            float x2 = x1 + dx;
            glVertex3f(x1, 0.0f, z);
            glVertex3f(x2, 0.0f, z);
            glVertex3f(x2, 0.0f, zNext);
            glVertex3f(x1, 0.0f, zNext);
        }
    }
    glEnd();

    // 5. Vạch kẻ đường phân làn đứt đoạn chuyển động vô tận
    applyMaterial(0.95f, 0.95f, 0.95f, 64.0f, 0.6f);
    float dashLength = 3.5f;
    float gapLength = 3.5f;
    float cycle = dashLength + gapLength;
    float offsetZ = std::fmod(totalDistance, cycle);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (float z = -roadLength; z <= 30.0f; z += cycle) {
        float curZ = z + offsetZ;
        // Phân làn giữa làn 0 và làn 1 (X = -1.75)
        glVertex3f(-1.85f, 0.02f, curZ);
        glVertex3f(-1.65f, 0.02f, curZ);
        glVertex3f(-1.65f, 0.02f, curZ - dashLength);
        glVertex3f(-1.85f, 0.02f, curZ - dashLength);

        // Phân làn giữa làn 1 và làn 2 (X = +1.75)
        glVertex3f( 1.65f, 0.02f, curZ);
        glVertex3f( 1.85f, 0.02f, curZ);
        glVertex3f( 1.85f, 0.02f, curZ - dashLength);
        glVertex3f( 1.65f, 0.02f, curZ - dashLength);
    }

    // Vạch viền vàng mép đường
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

    // 6. Hộ lan / Dải phân cách kim loại 2 bên mép đường
    applyMaterial(0.75f, 0.78f, 0.82f, 64.0f, 0.6f);
    glPushMatrix();
    glTranslatef(-roadWidth * 0.5f - 0.1f, 0.45f, -roadLength * 0.5f + 15.0f);
    drawCuboid(0.12f, 0.25f, roadLength + 30.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef( roadWidth * 0.5f + 0.1f, 0.45f, -roadLength * 0.5f + 15.0f);
    drawCuboid(0.12f, 0.25f, roadLength + 30.0f);
    glPopMatrix();
}
