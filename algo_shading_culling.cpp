/**
 * ============================================================================
 * MODULE 3: KHỬ MẶT KHUẤT, TÔ BÓNG & CHIẾU SÁNG (ALGO_SHADING_CULLING.CPP)
 * Dự án: 3D Endless Car Dodger (C++ & OpenGL / FreeGLUT)
 * ============================================================================
 */

#include "algo_shading_culling.h"
#include <GL/gl.h>

// ============================================================================
// 1. THUẬT TOÁN KHỬ MẶT KHUẤT VÀ TÔ BÓNG (4 CHẾ ĐỘ DEMO)
// ============================================================================
/**
 * Giải thích nguyên lý thuật toán phục vụ thuyết trình:
 * 
 * 1. Wireframe (Chế độ 1):
 *    - Vẽ đối tượng chỉ bằng các cạnh khung dây (GL_LINE).
 *    - Tắt cả Depth-Buffer và Back-face culling để quan sát hình dạng lưới đa giác nguyên thủy.
 * 
 * 2. Back-face Culling (Chế độ 2):
 *    - Kiểm tra hướng của mặt phẳng đối với hướng nhìn của Camera: (N . V < 0).
 *    - Nếu mặt quay ra xa, loại bỏ ngay ở giai đoạn đầu của đồ họa, giảm 50% khối lượng xử lý.
 *    - Trong demo này, ta cố tình TẮT Z-Buffer để giảng viên quan sát thấy: Dù các mặt sau đã
 *      bị loại bỏ, nhưng các mặt trước vẫn có thể vẽ đè lên nhau sai thứ tự (Painter's problem),
 *      từ đó làm nổi bật sự cần thiết của Depth-Buffer.
 * 
 * 3. Depth-Buffer (Z-Buffer) + Flat Shading (Chế độ 3):
 *    - Z-Buffer so sánh độ sâu từng điểm ảnh z-value lưu trong bộ đệm depth. Điểm ảnh nào
 *      ở gần mắt hơn (z nhỏ hơn) sẽ được ghi đè, giải quyết triệt để vấn đề khử mặt khuất.
 *    - Tô bóng phẳng (Flat Shading - GL_FLAT): Tính toán ánh sáng duy nhất một lần tại một đỉnh
 *      đại diện của đa giác và gán màu đồng nhất cho toàn bộ bề mặt, làm lộ rõ các đường vân facet.
 * 
 * 4. Gouraud Shading + Phong Lighting (Chế độ 4 - Mặc định):
 *    - Tô bóng mượt Gouraud (GL_SMOOTH): Tính toán cường độ sáng tại TỪNG ĐỈNH dựa trên
 *      vector pháp tuyến đỉnh (Vertex Normal), sau đó nội suy song tuyến (Bilinear Interpolation)
 *      màu sắc qua bề mặt tam giác.
 *    - Kết hợp mô hình phản xạ Phong đầy đủ gồm 3 thành phần: Ambient (Môi trường),
 *      Diffuse (Khuếch tán Lambertian) và Specular (Phản chiếu bóng gương).
 */
void applyRenderMode(int mode) {
    switch (mode) {
        case MODE_WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glDisable(GL_LIGHTING);
            break;

        case MODE_BACKFACE_CULL:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            break;

        case MODE_DEPTH_FLAT:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            glDisable(GL_CULL_FACE);
            glEnable(GL_LIGHTING);
            glShadeModel(GL_FLAT); // Tô bóng phẳng
            break;

        case MODE_GOURAUD_FULL:
        default:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            glEnable(GL_LIGHTING);
            glShadeModel(GL_SMOOTH); // Tô bóng mượt Gouraud
            break;
    }
}

// ============================================================================
// 2. HỆ THỐNG CHIẾU SÁNG & CHU KỲ NGÀY / ĐÊM (SPOTLIGHT)
// ============================================================================
/**
 * Giải thích nguyên lý nguồn sáng Spotlight cho báo cáo:
 * - Nguồn sáng đèn pha ô tô (GL_LIGHT1) là nguồn sáng vị trí có hướng (Positional Cutoff Light).
 * - Tham số GL_SPOT_CUTOFF = 42 độ: Xác định góc mở của hình nón chiếu sáng.
 * - Tham số GL_SPOT_DIRECTION = (0, -0.1, -1): Hướng trục nón chúc nhẹ xuống mặt đường.
 * - Tham số GL_SPOT_EXPONENT = 12.0: Cường độ sáng tập trung mạnh nhất ở trục tim nón và giảm dần ra mép.
 * - Hệ số suy giảm Attenuation: I = 1.0 / (kc + kl*d + kq*d^2) mô phỏng ánh sáng yếu dần khi chiếu ra xa.
 */
void setupLighting(bool isNight, float carX, float carZ) {
    if (!isNight) {
        // [BAN NGÀY]: Bầu trời xanh dịu, Ánh sáng Mặt Trời (GL_LIGHT0)
        glClearColor(0.52f, 0.78f, 0.95f, 1.0f);

        GLfloat sunPos[4]  = { 25.0f, 50.0f, 35.0f, 0.0f }; // W = 0: Nguồn sáng định hướng vô tận
        GLfloat sunAmb[4]  = { 0.38f, 0.38f, 0.40f, 1.0f };
        GLfloat sunDiff[4] = { 0.95f, 0.92f, 0.85f, 1.0f };
        GLfloat sunSpec[4] = { 0.70f, 0.70f, 0.70f, 1.0f };

        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, sunPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiff);
        glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpec);

        // Ban ngày tắt đèn pha ô tô
        glDisable(GL_LIGHT1);
    } else {
        // [BAN ĐÊM]: Bầu trời đêm thẫm, Ánh trăng mờ + Đèn pha Spotlight (GL_LIGHT1)
        glClearColor(0.04f, 0.04f, 0.09f, 1.0f);

        // Ánh trăng khuếch tán dịu nhẹ
        GLfloat moonPos[4]  = { 20.0f, 40.0f, 20.0f, 0.0f };
        GLfloat moonAmb[4]  = { 0.08f, 0.08f, 0.14f, 1.0f };
        GLfloat moonDiff[4] = { 0.15f, 0.15f, 0.25f, 1.0f };
        GLfloat moonSpec[4] = { 0.10f, 0.10f, 0.15f, 1.0f };

        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, moonPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, moonAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, moonDiff);
        glLightfv(GL_LIGHT0, GL_SPECULAR, moonSpec);

        // Đèn pha ô tô dạng nón Spotlight (GL_LIGHT1)
        GLfloat spotPos[4]  = { carX, 0.6f, carZ - 1.9f, 1.0f }; // W = 1: Nguồn sáng vị trí tại đầu xe
        GLfloat spotDir[3]  = { 0.0f, -0.1f, -1.0f };            // Hướng chiếu dọc trục Z âm
        GLfloat spotDiff[4] = { 1.0f, 0.98f, 0.85f, 1.0f };      // Màu vàng trắng sáng rực
        GLfloat spotSpec[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

        glEnable(GL_LIGHT1);
        glLightfv(GL_LIGHT1, GL_POSITION, spotPos);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 42.0f);
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 12.0f);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, spotDiff);
        glLightfv(GL_LIGHT1, GL_SPECULAR, spotSpec);

        // Hệ số suy giảm theo khoảng cách
        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.015f);
        glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0006f);
    }
}

// ============================================================================
// 3. THIẾT LẬP VẬT LIỆU THEO MÔ HÌNH CHIẾU SÁNG PHONG
// ============================================================================
void applyMaterial(float r, float g, float b, float shininess, float specVal) {
    glColor3f(r, g, b); // Màu trực tiếp cho Wireframe / Unlit

    GLfloat ambDiff[4]  = { r, g, b, 1.0f };
    GLfloat specular[4] = { specVal, specVal, specVal, 1.0f };
    GLfloat emission[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ambDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
}

void applyEmissiveMaterial(float r, float g, float b, float glow) {
    glColor3f(r, g, b);
    GLfloat ambDiff[4]  = { r, g, b, 1.0f };
    GLfloat specular[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat emission[4] = { r * glow, g * glow, b * glow, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ambDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
}
