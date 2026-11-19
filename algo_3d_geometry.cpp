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
// [QUY ƯỚC ĐỈNH NGƯỢC CHIỀU KIM ĐỒNG HỒ - GL_CCW]
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
// 3. ĐỔ BÓNG GẦM XE (PLANAR SHADOW) & BÁNH XE THỂ THAO CAO CẤP
// ============================================================================
void drawVehicleShadow(float x, float z, float width, float length) {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);

    glColor4f(0.01f, 0.01f, 0.01f, 0.44f);

    float rx = width * 0.54f;
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

/**
 * [BÁNH XE THỂ THAO CAO CẤP - ĐỐI XỨNG HOÀN HẢO 2 BÊN]
 * Khắc phục triệt để lỗi lệch tâm:
 *  - isLeft = true: Quay +90 độ quanh Z để trục +Y hướng sang -X (hướng ra ngoài bên trái xe).
 *  - isLeft = false: Quay -90 độ quanh Z để trục +Y hướng sang +X (hướng ra ngoài bên phải xe).
 *  - Căn giữa hình trụ bánh xe đối xứng qua tâm mount (-width/2 dọc trục Y).
 *  - Cả 2 bên xe hoàn toàn cân xứng đối xứng gương, mặt mâm đúc hướng ra ngoài.
 */
void drawSportsWheel(float radius, float width, float rotationAngle, bool isLeft) {
    glPushMatrix();

    // 1. Định hướng trục bánh xe theo trục X của thế giới
    if (isLeft) {
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    } else {
        glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
    }

    // 2. Căn giữa hình trụ bánh xe đối xứng qua tâm:
    // Cylinder vẽ từ Y = 0 đến Y = width, nên ta lùi lại -width/2 để bánh xe
    // nằm cân đối từ -width/2 (mặt trong) đến +width/2 (mặt ngoài)
    glTranslatef(0.0f, -width * 0.5f, 0.0f);

    // 3. Đĩa phanh kim loại & Cùm phanh đĩa (Brake Caliper) màu đỏ
    // Nằm ở phía trong mâm xe, không quay theo bánh xe
    applyMaterial(0.72f, 0.74f, 0.78f, 64.0f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 0.02f, 0.0f);
    drawCylinder(radius * 0.70f, 0.02f, 14);
    glPopMatrix();

    applyMaterial(0.95f, 0.08f, 0.08f, 32.0f);
    glPushMatrix();
    glTranslatef(radius * 0.52f, 0.01f, 0.0f);
    drawCuboid(radius * 0.35f, 0.05f, radius * 0.35f);
    glPopMatrix();

    // 4. [BIẾN ĐỔI HÌNH HỌC]: Bánh xe và mâm xoay quanh trục theo tốc độ lăn
    float rollRot = isLeft ? -rotationAngle : rotationAngle;
    glRotatef(rollRot, 0.0f, 1.0f, 0.0f);

    // 5. Lốp cao su thể thao đen (trải đều từ Y = 0 đến Y = width)
    applyMaterial(0.10f, 0.10f, 0.12f, 16.0f, 0.15f);
    drawCylinder(radius, width, 18);

    // 6. Mâm đúc hợp kim sáng bóng ở mặt ngoài xe (Y = width)
    applyMaterial(0.88f, 0.90f, 0.94f, 96.0f, 0.85f);
    glPushMatrix();
    glTranslatef(0.0f, width + 0.005f, 0.0f);
    drawCylinder(radius * 0.72f, 0.01f, 14);
    glPopMatrix();

    // Mâm trong (Y = -0.01f)
    glPushMatrix();
    glTranslatef(0.0f, -0.01f, 0.0f);
    drawCylinder(radius * 0.72f, 0.01f, 14);
    glPopMatrix();

    // 7. Năm chấu thể thao hình ngôi sao ở mặt ngoài xe
    applyMaterial(0.25f, 0.28f, 0.32f, 48.0f);
    for (int s = 0; s < 5; ++s) {
        glPushMatrix();
        glRotatef(s * 72.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, width + 0.01f, 0.0f);
        drawCuboid(radius * 1.15f, 0.01f, radius * 0.14f);
        glPopMatrix();
    }

    glPopMatrix();
}

// ============================================================================
// 4. SIÊU XE THỂ THAO CỦA NGƯỜI CHƠI (SUPERCAR)
// ============================================================================
void drawSuperCar(float x, float z, float rollAngle, float wheelAngle, bool isNight) {
    // Đổ bóng gầm xe thể thao
    drawVehicleShadow(x, z, 2.3f, 4.6f);

    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(-rollAngle, 0.0f, 0.0f, 1.0f);

    // 1. Gầm và Thân xe chính: Màu đỏ thể thao Italian Racing Red rực rỡ
    applyMaterial(0.95f, 0.08f, 0.10f, 80.0f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.0f);
    drawCuboid(1.85f, 0.40f, 4.1f);
    glPopMatrix();

    // 2. Cản trước khí động học với Lip carbon đen bóng
    applyMaterial(0.06f, 0.06f, 0.08f, 32.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.12f, -2.15f);
    drawCuboid(1.95f, 0.12f, 0.45f);
    glPopMatrix();

    // Mũi xe vuốt thấp thể thao
    applyMaterial(0.90f, 0.06f, 0.08f, 80.0f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, -1.85f);
    drawCuboid(1.72f, 0.26f, 0.85f);
    glPopMatrix();

    // Hốc gió làm mát trên nắp capo
    applyMaterial(0.12f, 0.12f, 0.14f, 48.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.38f, -1.45f);
    drawCuboid(0.85f, 0.04f, 0.55f);
    glPopMatrix();

    // 3. Cabin & Kính chắn gió buồng lái Panoramic bán trong suốt
    // Nẹp trụ A và trụ C màu đen bóng
    applyMaterial(0.12f, 0.12f, 0.15f, 96.0f, 0.9f);
    glPushMatrix();
    glTranslatef(0.0f, 0.62f, 0.15f);
    drawCuboid(1.38f, 0.48f, 2.1f);
    glPopMatrix();

    // Kính chắn gió trước vát nghiêng trong suốt (GL_BLEND)
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.20f, 0.50f, 0.85f, 0.52f);
    GLfloat glassMat[4] = { 0.20f, 0.50f, 0.85f, 0.52f };
    GLfloat glassSpec[4] = { 0.95f, 0.95f, 0.95f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, glassMat);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glassSpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128.0f);

    glPushMatrix();
    glTranslatef(0.0f, 0.62f, -0.95f);
    glRotatef(28.0f, 1.0f, 0.0f, 0.0f);
    drawCuboid(1.34f, 0.46f, 0.08f);
    glPopMatrix();

    // Kính chắn gió sau vuốt thoải
    glPushMatrix();
    glTranslatef(0.0f, 0.62f, 1.25f);
    glRotatef(-28.0f, 1.0f, 0.0f, 0.0f);
    drawCuboid(1.34f, 0.46f, 0.08f);
    glPopMatrix();
    glPopAttrib();

    // 4. Gương chiếu hậu 2 bên (Side Mirrors)
    applyMaterial(0.88f, 0.05f, 0.08f, 64.0f);
    glPushMatrix(); glTranslatef(-1.02f, 0.62f, -0.65f); drawCuboid(0.26f, 0.12f, 0.16f); glPopMatrix();
    glPushMatrix(); glTranslatef( 1.02f, 0.62f, -0.65f); drawCuboid(0.26f, 0.12f, 0.16f); glPopMatrix();
    // Mặt kính gương phản quang
    applyMaterial(0.95f, 0.95f, 0.98f, 128.0f, 1.0f);
    glPushMatrix(); glTranslatef(-1.02f, 0.62f, -0.56f); drawCuboid(0.24f, 0.10f, 0.02f); glPopMatrix();
    glPushMatrix(); glTranslatef( 1.02f, 0.62f, -0.56f); drawCuboid(0.24f, 0.10f, 0.02f); glPopMatrix();

    // 5. Cánh gió thể thao GT kép phía sau (Dual Wing Spoiler)
    applyMaterial(0.08f, 0.08f, 0.10f, 48.0f);
    glPushMatrix(); glTranslatef(-0.65f, 0.62f, 1.85f); drawCuboid(0.08f, 0.42f, 0.12f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.65f, 0.62f, 1.85f); drawCuboid(0.08f, 0.42f, 0.12f); glPopMatrix();
    applyMaterial(0.95f, 0.08f, 0.10f, 80.0f, 0.8f);
    glPushMatrix(); glTranslatef(0.0f, 1.04f, 1.85f); drawCuboid(2.05f, 0.08f, 0.45f); glPopMatrix();
    // Cánh lướt gió phụ bên dưới
    applyMaterial(0.08f, 0.08f, 0.10f, 32.0f);
    glPushMatrix(); glTranslatef(0.0f, 0.85f, 1.85f); drawCuboid(1.65f, 0.05f, 0.30f); glPopMatrix();

    // 6. Cụm 4 ống xả thể thao mạ Chrome phía sau (Quad Exhausts)
    applyMaterial(0.85f, 0.88f, 0.92f, 96.0f, 0.9f);
    float exY = 0.20f, exZ = 2.06f;
    glPushMatrix(); glTranslatef(-0.62f, exY, exZ); drawCylinder(0.07f, 0.18f, 10); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.46f, exY, exZ); drawCylinder(0.07f, 0.18f, 10); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.46f, exY, exZ); drawCylinder(0.07f, 0.18f, 10); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.62f, exY, exZ); drawCylinder(0.07f, 0.18f, 10); glPopMatrix();

    // 7. Cụm đèn pha trước công nghệ LED Daylight
    if (isNight) {
        applyEmissiveMaterial(1.0f, 0.98f, 0.75f, 1.0f);
    } else {
        applyMaterial(0.95f, 0.95f, 0.85f, 64.0f, 0.8f);
    }
    glPushMatrix(); glTranslatef(-0.68f, 0.32f, -2.06f); drawCuboid(0.38f, 0.14f, 0.10f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.68f, 0.32f, -2.06f); drawCuboid(0.38f, 0.14f, 0.10f); glPopMatrix();

    // Dải đèn hậu LED Neon đỏ rực suốt chiều ngang đuôi xe
    applyEmissiveMaterial(0.98f, 0.05f, 0.05f, isNight ? 1.0f : 0.45f);
    glPushMatrix();
    glTranslatef(0.0f, 0.42f, 2.06f);
    drawCuboid(1.76f, 0.12f, 0.08f);
    glPopMatrix();

    // 8. Bốn bánh xe thể thao hợp kim cao cấp
    float wR = 0.36f, wW = 0.28f, wY = 0.36f;
    glPushMatrix(); glTranslatef(-0.95f, wY, -1.25f); drawSportsWheel(wR, wW, wheelAngle, true); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.95f, wY, -1.25f); drawSportsWheel(wR, wW, wheelAngle, false); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.95f, wY,  1.25f); drawSportsWheel(wR, wW, wheelAngle, true); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.95f, wY,  1.25f); drawSportsWheel(wR, wW, wheelAngle, false); glPopMatrix();

    glPopMatrix();
}

// ============================================================================
// 5. CÁC PHƯƠNG TIỆN GIAO THÔNG CẢN ĐƯỜNG (TRAFFIC VEHICLES)
// ============================================================================

/**
 * [PHƯƠNG TIỆN 1]: XE TẢI CONTAINER CỠ LỚN (SEMI-TRAILER TRUCK)
 * Kích thước đồ sộ: Rộng 2.3m x Cao 2.9m x Dài 8.2m
 */
void drawSemiTruck(float x, float y, float z, float r, float g, float b, float wheelAngle) {
    drawVehicleShadow(x, z, 2.6f, 8.6f);

    glPushMatrix();
    glTranslatef(x, y, z);

    // 1. Thùng Container phía sau (Màu riêng biệt hoặc xám bạc)
    applyMaterial(r, g, b, 24.0f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.45f, 0.8f);
    drawCuboid(2.25f, 2.35f, 5.6f);
    glPopMatrix();

    // Gân dập nổi của thùng Container
    applyMaterial(r * 0.85f, g * 0.85f, b * 0.85f, 32.0f);
    for (float cz = -1.6f; cz <= 3.2f; cz += 0.8f) {
        glPushMatrix();
        glTranslatef(0.0f, 0.45f, cz);
        drawCuboid(2.28f, 2.36f, 0.12f);
        glPopMatrix();
    }

    // Cửa sau Container với thanh khóa mạ kẽm
    applyMaterial(0.2f, 0.2f, 0.2f, 16.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.45f, 3.61f);
    drawCuboid(2.15f, 2.25f, 0.05f);
    glPopMatrix();

    // 2. Đầu kéo xe tải phía trước (Tractor Cab)
    applyMaterial(0.15f, 0.35f, 0.75f, 64.0f, 0.6f); // Xanh dương hoặc đỏ
    glPushMatrix();
    glTranslatef(0.0f, 0.40f, -2.8f);
    drawCuboid(2.2f, 2.0f, 2.4f);
    glPopMatrix();

    // Lưới tản nhiệt mạ Chrome khổng lồ ở đầu xe
    applyMaterial(0.85f, 0.88f, 0.92f, 96.0f, 0.9f);
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, -4.01f);
    drawCuboid(1.7f, 1.1f, 0.12f);
    glPopMatrix();

    // Kính chắn gió đầu kéo
    applyMaterial(0.18f, 0.35f, 0.55f, 96.0f, 0.9f);
    glPushMatrix();
    glTranslatef(0.0f, 1.45f, -3.4f);
    drawCuboid(1.95f, 0.75f, 0.85f);
    glPopMatrix();

    // 2 Ống xả đứng mạ Chrome cao vút 2 bên đầu kéo
    applyMaterial(0.85f, 0.88f, 0.92f, 96.0f, 0.9f);
    glPushMatrix(); glTranslatef(-1.15f, 1.2f, -1.8f); drawCylinder(0.08f, 1.9f, 10); glPopMatrix();
    glPushMatrix(); glTranslatef( 1.15f, 1.2f, -1.8f); drawCylinder(0.08f, 1.9f, 10); glPopMatrix();

    // Đèn hậu đỏ cảnh báo xe tải
    applyEmissiveMaterial(0.95f, 0.1f, 0.1f, 0.9f);
    glPushMatrix(); glTranslatef(-0.85f, 0.6f, 3.62f); drawCuboid(0.35f, 0.25f, 0.08f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.85f, 0.6f, 3.62f); drawCuboid(0.35f, 0.25f, 0.08f); glPopMatrix();

    // 3. Hệ thống 8 bánh xe tải hạng nặng
    float twR = 0.42f, twW = 0.35f, twY = 0.42f;
    // Bánh đầu kéo
    glPushMatrix(); glTranslatef(-1.15f, twY, -3.2f); drawSportsWheel(twR, twW, wheelAngle, true); glPopMatrix();
    glPushMatrix(); glTranslatef( 1.15f, twY, -3.2f); drawSportsWheel(twR, twW, wheelAngle, false); glPopMatrix();
    // Bánh đôi trục sau đầu kéo
    glPushMatrix(); glTranslatef(-1.15f, twY, -1.8f); drawSportsWheel(twR, twW, wheelAngle, true); glPopMatrix();
    glPushMatrix(); glTranslatef( 1.15f, twY, -1.8f); drawSportsWheel(twR, twW, wheelAngle, false); glPopMatrix();
    // Bánh đôi trục sau Container (2 cầu đôi)
    glPushMatrix(); glTranslatef(-1.15f, twY,  2.2f); drawSportsWheel(twR, twW, wheelAngle, true); glPopMatrix();
    glPushMatrix(); glTranslatef( 1.15f, twY,  2.2f); drawSportsWheel(twR, twW, wheelAngle, false); glPopMatrix();
    glPushMatrix(); glTranslatef(-1.15f, twY,  3.2f); drawSportsWheel(twR, twW, wheelAngle, true); glPopMatrix();
    glPushMatrix(); glTranslatef( 1.15f, twY,  3.2f); drawSportsWheel(twR, twW, wheelAngle, false); glPopMatrix();

    glPopMatrix();
}

/**
 * [PHƯƠNG TIỆN 2]: XE BUÝT THÀNH PHỐ (CITY TRANSIT BUS)
 * Kích thước: Rộng 2.2m x Cao 2.5m x Dài 6.8m
 */
void drawCityBus(float x, float y, float z, float r, float g, float b, float wheelAngle) {
    drawVehicleShadow(x, z, 2.5f, 7.2f);

    glPushMatrix();
    glTranslatef(x, y, z);

    // 1. Thân xe buýt chính
    applyMaterial(r, g, b, 48.0f, 0.5f);
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, 0.0f);
    drawCuboid(2.15f, 2.05f, 6.6f);
    glPopMatrix();

    // 2. Dải cửa sổ kính hành khách chạy suốt 2 bên sườn xe
    applyMaterial(0.18f, 0.32f, 0.45f, 96.0f, 0.9f);
    glPushMatrix();
    glTranslatef(0.0f, 1.25f, 0.0f);
    drawCuboid(2.18f, 0.85f, 5.8f);
    glPopMatrix();

    // Kính chắn gió rộng phía trước
    glPushMatrix();
    glTranslatef(0.0f, 1.15f, -3.31f);
    drawCuboid(1.95f, 1.05f, 0.08f);
    glPopMatrix();

    // Bảng hiệu số tuyến xe buýt phát sáng phía trên kính lái
    applyEmissiveMaterial(0.1f, 0.9f, 0.2f, 0.85f);
    glPushMatrix();
    glTranslatef(0.0f, 2.05f, -3.31f);
    drawCuboid(1.4f, 0.25f, 0.08f);
    glPopMatrix();

    // Cửa lên xuống hành khách bên phải
    applyMaterial(0.15f, 0.15f, 0.18f, 24.0f);
    glPushMatrix();
    glTranslatef(1.09f, 0.35f, -1.8f);
    drawCuboid(0.05f, 1.75f, 0.95f);
    glPopMatrix();

    // Cụm đèn hậu xe buýt
    applyEmissiveMaterial(0.95f, 0.08f, 0.08f, 0.9f);
    glPushMatrix(); glTranslatef(-0.75f, 0.75f, 3.31f); drawCuboid(0.3f, 0.55f, 0.08f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.75f, 0.75f, 3.31f); drawCuboid(0.3f, 0.55f, 0.08f); glPopMatrix();

    // 3. Bốn bánh xe buýt
    float bwR = 0.38f, bwW = 0.32f, bwY = 0.38f;
    glPushMatrix(); glTranslatef(-1.10f, bwY, -2.1f); drawSportsWheel(bwR, bwW, wheelAngle, true); glPopMatrix();
    glPushMatrix(); glTranslatef( 1.10f, bwY, -2.1f); drawSportsWheel(bwR, bwW, wheelAngle, false); glPopMatrix();
    glPushMatrix(); glTranslatef(-1.10f, bwY,  2.0f); drawSportsWheel(bwR, bwW, wheelAngle, true); glPopMatrix();
    glPushMatrix(); glTranslatef( 1.10f, bwY,  2.0f); drawSportsWheel(bwR, bwW, wheelAngle, false); glPopMatrix();

    glPopMatrix();
}

/**
 * [PHƯƠNG TIỆN 3]: XE BÁN TẢI ĐỊA HÌNH (OFF-ROAD PICKUP TRUCK / SUV)
 * Kích thước: Rộng 1.9m x Cao 1.6m x Dài 4.4m
 */
void drawPickupTruck(float x, float y, float z, float r, float g, float b, float wheelAngle) {
    drawVehicleShadow(x, z, 2.2f, 4.8f);

    glPushMatrix();
    glTranslatef(x, y, z);

    // 1. Thùng xe bán tải phía sau
    applyMaterial(r, g, b, 48.0f, 0.5f);
    glPushMatrix();
    glTranslatef(0.0f, 0.40f, 0.85f);
    drawCuboid(1.85f, 0.70f, 2.1f);
    glPopMatrix();

    // Khung chống lật thể thao (Roll Bar) trên thùng xe
    applyMaterial(0.12f, 0.12f, 0.14f, 32.0f);
    glPushMatrix(); glTranslatef(-0.75f, 1.10f, 0.05f); drawCuboid(0.1f, 0.55f, 0.1f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.75f, 1.10f, 0.05f); drawCuboid(0.1f, 0.55f, 0.1f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.0f,  1.60f, 0.05f); drawCuboid(1.6f, 0.08f, 0.1f); glPopMatrix();

    // 2. Cabin xe bán tải (Gầm cao)
    applyMaterial(r, g, b, 48.0f, 0.5f);
    glPushMatrix();
    glTranslatef(0.0f, 0.40f, -0.65f);
    drawCuboid(1.85f, 1.05f, 1.8f);
    glPopMatrix();

    // Kính chắn gió cabin
    applyMaterial(0.20f, 0.35f, 0.55f, 96.0f, 0.85f);
    glPushMatrix();
    glTranslatef(0.0f, 0.95f, -1.56f);
    glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
    drawCuboid(1.65f, 0.55f, 0.1f);
    glPopMatrix();

    // Nắp capo mũi xe
    applyMaterial(r, g, b, 48.0f, 0.5f);
    glPushMatrix();
    glTranslatef(0.0f, 0.40f, -1.85f);
    drawCuboid(1.80f, 0.55f, 1.0f);
    glPopMatrix();

    // Cản va bảo vệ trước (Bull Bar) địa hình
    applyMaterial(0.15f, 0.15f, 0.18f, 48.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.35f, -2.40f);
    drawCuboid(1.90f, 0.45f, 0.20f);
    glPopMatrix();

    // Đèn hậu đỏ phía sau thùng
    applyEmissiveMaterial(0.95f, 0.05f, 0.05f, 0.9f);
    glPushMatrix(); glTranslatef(-0.75f, 0.65f, 1.91f); drawCuboid(0.25f, 0.35f, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.75f, 0.65f, 1.91f); drawCuboid(0.25f, 0.35f, 0.06f); glPopMatrix();

    // 3. Bánh xe địa hình gầm cao (Lốp to khỏe)
    float pwR = 0.40f, pwW = 0.32f, pwY = 0.40f;
    glPushMatrix(); glTranslatef(-0.95f, pwY, -1.35f); drawSportsWheel(pwR, pwW, wheelAngle, true); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.95f, pwY, -1.35f); drawSportsWheel(pwR, pwW, wheelAngle, false); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.95f, pwY,  1.15f); drawSportsWheel(pwR, pwW, wheelAngle, true); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.95f, pwY,  1.15f); drawSportsWheel(pwR, pwW, wheelAngle, false); glPopMatrix();

    glPopMatrix();
}

/**
 * [PHƯƠNG TIỆN 4]: XE SEDAN / TAXI ĐÔ THỊ (CITY SEDAN / TAXI)
 * Kích thước: Rộng 1.8m x Cao 1.3m x Dài 4.0m
 */
void drawSedanCar(float x, float y, float z, float r, float g, float b, float wheelAngle) {
    drawVehicleShadow(x, z, 2.1f, 4.4f);

    glPushMatrix();
    glTranslatef(x, y, z);

    // Thân xe
    applyMaterial(r, g, b, 64.0f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.0f);
    drawCuboid(1.78f, 0.45f, 3.9f);
    glPopMatrix();

    // Mui cabin
    applyMaterial(r * 0.95f, g * 0.95f, b * 0.95f, 64.0f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 0.65f, 0.1f);
    drawCuboid(1.42f, 0.50f, 2.1f);
    glPopMatrix();

    // Kính chắn gió trước và sau
    applyMaterial(0.18f, 0.35f, 0.55f, 96.0f, 0.9f);
    glPushMatrix();
    glTranslatef(0.0f, 0.65f, -0.96f);
    glRotatef(25.0f, 1.0f, 0.0f, 0.0f);
    drawCuboid(1.36f, 0.45f, 0.1f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.65f, 1.16f);
    glRotatef(-25.0f, 1.0f, 0.0f, 0.0f);
    drawCuboid(1.36f, 0.45f, 0.1f);
    glPopMatrix();

    // Biển hiệu Taxi trên nóc xe (nếu xe màu vàng tươi)
    if (r > 0.8f && g > 0.7f && b < 0.3f) {
        applyEmissiveMaterial(1.0f, 0.9f, 0.1f, 0.9f);
        glPushMatrix();
        glTranslatef(0.0f, 1.18f, 0.1f);
        drawCuboid(0.6f, 0.15f, 0.25f);
        glPopMatrix();
    }

    // Đèn hậu đỏ
    applyEmissiveMaterial(0.95f, 0.08f, 0.08f, 0.85f);
    glPushMatrix(); glTranslatef(-0.62f, 0.38f, 1.96f); drawCuboid(0.32f, 0.15f, 0.08f); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.62f, 0.38f, 1.96f); drawCuboid(0.32f, 0.15f, 0.08f); glPopMatrix();

    // Bánh xe
    float swR = 0.35f, swW = 0.26f, swY = 0.35f;
    glPushMatrix(); glTranslatef(-0.90f, swY, -1.15f); drawSportsWheel(swR, swW, wheelAngle, true); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.90f, swY, -1.15f); drawSportsWheel(swR, swW, wheelAngle, false); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.90f, swY,  1.15f); drawSportsWheel(swR, swW, wheelAngle, true); glPopMatrix();
    glPushMatrix(); glTranslatef( 0.90f, swY,  1.15f); drawSportsWheel(swR, swW, wheelAngle, false); glPopMatrix();

    glPopMatrix();
}

// ============================================================================
// 6. CẢNH QUAN HAI BÊN ĐƯỜNG & MẶT ĐƯỜNG CHIA LƯỚI
// ============================================================================
void drawHouse(float x, float y, float z, float scale, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    float houseW = 5.0f, houseH = 4.2f, houseL = 6.0f;
    applyMaterial(r, g, b, 24.0f, 0.2f);
    drawCuboid(houseW, houseH, houseL);

    applyMaterial(0.35f, 0.20f, 0.10f, 16.0f);
    glPushMatrix(); glTranslatef(0.0f, 0.0f, houseL * 0.5f + 0.02f); drawCuboid(1.2f, 2.2f, 0.05f); glPopMatrix();

    applyMaterial(0.6f, 0.8f, 0.95f, 80.0f, 0.8f);
    glPushMatrix(); glTranslatef( 1.5f, 2.2f, houseL * 0.5f + 0.02f); drawCuboid(1.0f, 1.0f, 0.05f); glPopMatrix();
    glPushMatrix(); glTranslatef(-1.5f, 2.2f, houseL * 0.5f + 0.02f); drawCuboid(1.0f, 1.0f, 0.05f); glPopMatrix();

    applyMaterial(0.75f, 0.22f, 0.15f, 32.0f, 0.3f);
    glPushMatrix(); glTranslatef(0.0f, houseH, 0.0f); drawPyramid(houseW + 0.8f, houseL + 0.8f, 2.8f); glPopMatrix();

    glPopMatrix();
}

void drawTree(float x, float y, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    applyMaterial(0.42f, 0.26f, 0.14f, 12.0f, 0.1f);
    drawCylinder(0.35f, 2.5f, 12);

    applyMaterial(0.12f, 0.55f, 0.18f, 24.0f, 0.2f);
    glPushMatrix(); glTranslatef(0.0f, 2.0f, 0.0f); drawCone(1.8f, 3.0f, 14); glPopMatrix();

    applyMaterial(0.18f, 0.68f, 0.22f, 24.0f, 0.2f);
    glPushMatrix(); glTranslatef(0.0f, 3.8f, 0.0f); drawCone(1.4f, 2.6f, 14); glPopMatrix();

    glPopMatrix();
}

void drawLamppost(float x, float y, float z, bool onLeftSide, bool isNight) {
    glPushMatrix();
    glTranslatef(x, y, z);

    applyMaterial(0.6f, 0.62f, 0.65f, 64.0f, 0.5f);
    drawCylinder(0.18f, 7.0f, 10);

    float armLen = 2.4f;
    float dir = onLeftSide ? 1.0f : -1.0f;
    glPushMatrix(); glTranslatef(dir * armLen * 0.5f, 6.8f, 0.0f); drawCuboid(armLen, 0.16f, 0.16f); glPopMatrix();

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

void drawMountains(float roadLength) {
    float startZ = -roadLength - 30.0f;
    float mountainData[][5] = {
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

        applyMaterial(0.90f, 0.92f, 0.98f, 32.0f, 0.2f);
        glTranslatef(0.0f, mountainData[i][4] * 0.72f, 0.0f);
        drawPyramid(mountainData[i][3] * 0.32f, mountainData[i][3] * 0.32f, mountainData[i][4] * 0.28f);
        glPopMatrix();
    }
}

void drawRoadAndEnvironment(float totalDistance, float roadWidth, float roadLength) {
    // 1. Dãy núi xa chân trời
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

    int nx = 12;
    float dx = roadWidth / (float)nx;
    float dz = 1.8f;

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

    // 5. Vạch kẻ đường phân làn đứt đoạn
    applyMaterial(0.95f, 0.95f, 0.95f, 64.0f, 0.6f);
    float dashLength = 3.5f;
    float gapLength = 3.5f;
    float cycle = dashLength + gapLength;
    float offsetZ = std::fmod(totalDistance, cycle);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    for (float z = -roadLength; z <= 30.0f; z += cycle) {
        float curZ = z + offsetZ;
        glVertex3f(-1.85f, 0.02f, curZ);
        glVertex3f(-1.65f, 0.02f, curZ);
        glVertex3f(-1.65f, 0.02f, curZ - dashLength);
        glVertex3f(-1.85f, 0.02f, curZ - dashLength);

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

    // 6. Hộ lan kim loại bảo vệ 2 bên mép đường
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
