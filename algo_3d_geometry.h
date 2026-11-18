/**
 * ============================================================================
 * MODULE 2: MÔ HÌNH HÓA ĐỐI TƯỢNG 3D & VECTOR PHÁP TUYẾN (ALGO_3D_GEOMETRY.H)
 * Dự án: 3D Endless Car Dodger (C++ & OpenGL / FreeGLUT)
 * 
 * Mô tả module:
 *  - Cung cấp cấu trúc dữ liệu Vector3 và thuật toán tính Vector Pháp Tuyến
 *    chuẩn xác bằng tích có hướng (Cross Product) và chuẩn hóa độ dài (Normalize).
 *  - Cài đặt các hàm dựng hình khối 3D nguyên thủy theo quy ước đỉnh ngược chiều
 *    kim đồng hồ (Counter-Clockwise - CCW) phục vụ Back-face Culling và Lighting:
 *    1. Khối hộp chữ nhật (Cuboid)
 *    2. Khối chóp tứ giác đều S.ABCD (Pyramid)
 *    3. Khối chóp cụt (Truncated Frustum)
 *    4. Khối hình trụ tròn (Cylinder)
 *    5. Khối hình nón (Cone)
 *  - Lắp ghép các đối tượng 3D phức hợp thông qua các phép biến đổi hình học
 *    (glTranslatef, glRotatef, glScalef kết hợp glPushMatrix/glPopMatrix):
 *    + Xe ô tô thể thao người chơi (bánh xe quay theo tốc độ, thân nghiêng khi rẽ)
 *    + Xe cản đường đối thủ
 *    + Chướng ngại vật (Thùng gỗ, Rào chắn phản quang)
 *    + Cảnh quan hai bên đường (Nhà phố, Cây cối, Cột đèn cao áp)
 *    + Mặt đường 3 làn xe vô tận và vỉa hè
 * ============================================================================
 */

#ifndef ALGO_3D_GEOMETRY_H
#define ALGO_3D_GEOMETRY_H

#include <cmath>

// ============================================================================
// 1. CẤU TRÚC VECTOR 3D VÀ TOÁN TỬ VECTOR
// ============================================================================
struct Vector3 {
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    Vector3 operator+(const Vector3& o) const { return Vector3(x + o.x, y + o.y, z + o.z); }
    Vector3 operator-(const Vector3& o) const { return Vector3(x - o.x, y - o.y, z - o.z); }
    Vector3 operator*(float s) const { return Vector3(x * s, y * s, z * s); }
};

/**
 * [THUẬT TOÁN ĐỒ HỌA]: Tính tích có hướng (Cross Product) của 2 vector 3D
 * C = A x B = (Ay*Bz - Az*By, Az*Bx - Ax*Bz, Ax*By - Ay*Bx)
 */
Vector3 crossProduct(const Vector3& a, const Vector3& b);

/**
 * [THUẬT TOÁN ĐỒ HỌA]: Chuẩn hóa vector thành vector đơn vị có độ dài = 1
 */
Vector3 normalize(const Vector3& v);

/**
 * [THUẬT TOÁN ĐỒ HỌA]: Tính vector pháp tuyến mặt phẳng từ 3 đỉnh tam giác CCW
 * Normal = Normalize((V1 - V0) x (V2 - V0))
 */
Vector3 calcNormal(const Vector3& v0, const Vector3& v1, const Vector3& v2);

// ============================================================================
// 2. CÁC HÀM DỰNG HÌNH KHỐI 3D NGUYÊN THỦY (PRIMITIVES)
// ============================================================================
void drawCuboid(float width, float height, float length);
void drawPyramid(float baseW, float baseL, float height);
void drawFrustum(float bW, float bL, float tW, float tL, float height);
void drawCylinder(float radius, float height, int slices);
void drawCone(float radius, float height, int slices);

// ============================================================================
// 3. CÁC HÀM LẮP GHÉP ĐỐI TƯỢNG PHỨC HỢP VỚI PHÉP BIẾN ĐỔI HÌNH HỌC
// ============================================================================
void drawWheel(float radius, float width, float rotationAngle);

// Xe ô tô thể thao của người chơi (với bánh lăn và thân xe nghiêng)
void drawCar(float x, float z, float rollAngle, float wheelAngle, bool isNight);

// Xe cản đường đối thủ
void drawTrafficCar(float x, float y, float z, float r, float g, float b, float wheelAngle);

// Chướng ngại vật
void drawObstacleCrate(float x, float y, float z, float size);
void drawObstacleBarrier(float x, float y, float z, float w, float h, float l);

// Cảnh quan hai bên đường
void drawHouse(float x, float y, float z, float scale, float r, float g, float b);
void drawTree(float x, float y, float z, float scale);
void drawLamppost(float x, float y, float z, bool onLeftSide, bool isNight);

// Mặt đường và môi trường xung quanh
void drawRoadAndEnvironment(float totalDistance, float roadWidth, float roadLength);

#endif // ALGO_3D_GEOMETRY_H
