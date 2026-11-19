/**
 * ============================================================================
 * MODULE 2: MÔ HÌNH HÓA ĐỐI TƯỢNG 3D & VECTOR PHÁP TUYẾN (ALGO_3D_GEOMETRY.H)
 * Dự án: 3D Endless Car Dodger (C++ & OpenGL / FreeGLUT)
 * 
 * Nâng cấp đồ họa:
 *  - Siêu xe thể thao người chơi (Supercar): Thiết kế khí động học, gương chiếu hậu,
 *    cánh gió thể thao kép, buồng lái kính cong trong suốt, cùm phanh thể thao đỏ.
 *  - Đa dạng hóa các phương tiện giao thông cản đường:
 *    + Xe tải Container khổng lồ (Semi-Trailer Truck)
 *    + Xe buýt thành phố (City Transit Bus)
 *    + Xe bán tải địa hình (Pickup Truck / SUV)
 *    + Xe Sedan / Taxi đô thị (City Sedan / Taxi)
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

Vector3 crossProduct(const Vector3& a, const Vector3& b);
Vector3 normalize(const Vector3& v);
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
// Bánh xe thể thao với cùm phanh đĩa (Brake Caliper) màu đỏ nổi bật
void drawSportsWheel(float radius, float width, float rotationAngle, bool isLeft);

// Đổ bóng đen bán trong suốt dưới gầm xe (Planar Shadow)
void drawVehicleShadow(float x, float z, float width, float length);

// Siêu xe thể thao của người chơi (Supercar với kính chắn gió trong suốt, gương chiếu hậu, cánh gió kép)
void drawSuperCar(float x, float z, float rollAngle, float wheelAngle, bool isNight);

// ============================================================================
// 4. CÁC PHƯƠNG TIỆN GIAO THÔNG CẢN ĐƯỜNG (TRAFFIC VEHICLES)
// ============================================================================
// 1. Xe tải Container cỡ lớn (Semi-Trailer Truck)
void drawSemiTruck(float x, float y, float z, float r, float g, float b, float wheelAngle);

// 2. Xe buýt thành phố (City Transit Bus)
void drawCityBus(float x, float y, float z, float r, float g, float b, float wheelAngle);

// 3. Xe bán tải địa hình (Pickup Truck / SUV)
void drawPickupTruck(float x, float y, float z, float r, float g, float b, float wheelAngle);

// 4. Xe Sedan / Taxi đô thị (City Sedan / Taxi)
void drawSedanCar(float x, float y, float z, float r, float g, float b, float wheelAngle);

// Cảnh quan hai bên đường
void drawHouse(float x, float y, float z, float scale, float r, float g, float b);
void drawTree(float x, float y, float z, float scale);
void drawLamppost(float x, float y, float z, bool onLeftSide, bool isNight);

// Dãy núi hình chóp ở xa chân trời
void drawMountains(float roadLength);

// Mặt đường chia lưới (Subdivided Grid) đón đèn pha Spotlight và phong cảnh
void drawRoadAndEnvironment(float totalDistance, float roadWidth, float roadLength);

#endif // ALGO_3D_GEOMETRY_H
