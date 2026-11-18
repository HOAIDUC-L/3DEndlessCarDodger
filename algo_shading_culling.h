/**
 * ============================================================================
 * MODULE 3: KHỬ MẶT KHUẤT, TÔ BÓNG & CHIẾU SÁNG (ALGO_SHADING_CULLING.H)
 * Dự án: 3D Endless Car Dodger (C++ & OpenGL / FreeGLUT)
 * 
 * Mô tả module:
 *  - Cài đặt và tích hợp 4 chế độ thuật toán đồ họa máy tính quan trọng:
 *    1. Khung dây (Wireframe - GL_LINE): quan sát cấu trúc lưới đa giác nguyên thủy.
 *    2. Khử mặt sau Back-face Culling (GL_CULL_FACE, GL_BACK, GL_CCW): loại bỏ
 *       các mặt hướng ra xa camera dựa trên dấu tích vô hướng (N . V < 0).
 *    3. Khử mặt khuất bằng Depth-Buffer (Z-Buffer) kết hợp Tô bóng phẳng (Flat Shading - GL_FLAT).
 *    4. Khử mặt khuất Z-Buffer kết hợp Tô bóng mượt (Gouraud Shading - GL_SMOOTH)
 *       và mô hình chiếu sáng Phong đầy đủ.
 *  - Quản lý chu kỳ Ngày / Đêm:
 *    + Ban ngày: Nguồn sáng mặt trời định hướng GL_LIGHT0.
 *    + Ban đêm: Nguồn sáng đèn pha ô tô dạng nón Spotlight GL_LIGHT1
 *      với các tham số góc mở GL_SPOT_CUTOFF, hướng chiếu GL_SPOT_DIRECTION,
 *      và suy giảm khoảng cách Attenuation.
 * ============================================================================
 */

#ifndef ALGO_SHADING_CULLING_H
#define ALGO_SHADING_CULLING_H

// Các chế độ thuật toán hiển thị
enum RenderMode {
    MODE_WIREFRAME = 1,     // [Thuật toán 1]: Khung dây, tắt khử khuất và chiếu sáng
    MODE_BACKFACE_CULL = 2, // [Thuật toán 2]: Khử mặt sau Back-face Culling (không Z-buffer)
    MODE_DEPTH_FLAT = 3,    // [Thuật toán 3]: Z-Buffer + Tô bóng phẳng (Flat Shading)
    MODE_GOURAUD_FULL = 4   // [Thuật toán 4]: Z-Buffer + Tô bóng mượt (Gouraud) + Phong
};

/**
 * [THUẬT TOÁN ĐỒ HỌA]: Kích hoạt chế độ khử khuất và tô bóng theo phím bấm 1/2/3/4
 * @param mode: Giá trị từ 1 đến 4 tương ứng với RenderMode
 */
void applyRenderMode(int mode);

/**
 * [THUẬT TOÁN ĐỒ HỌA]: Cấu hình hệ thống chiếu sáng và nguồn sáng Spotlight
 * @param isNight: true = chế độ Ban đêm (bật đèn pha Spotlight), false = Ban ngày (Mặt trời)
 * @param carX, carZ: Tọa độ xe người chơi để định vị nguồn sáng đèn pha ô tô (GL_LIGHT1)
 */
void setupLighting(bool isNight, float carX, float carZ);

/**
 * Thiết lập vật liệu phản xạ theo mô hình ánh sáng Phong (Ambient, Diffuse, Specular)
 */
void applyMaterial(float r, float g, float b, float shininess = 32.0f, float specVal = 0.4f);

/**
 * Thiết lập vật liệu tự phát quang (cho đèn xe và đèn đường ban đêm)
 */
void applyEmissiveMaterial(float r, float g, float b, float glow = 1.0f);

#endif // ALGO_SHADING_CULLING_H
