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
void applyRenderMode(int mode) {
    switch (mode) {
        case MODE_WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glDisable(GL_LIGHTING);
            glDisable(GL_FOG); // Tắt sương mù để thấy rõ khung dây thuần túy
            break;

        case MODE_BACKFACE_CULL:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_LIGHTING);
            glEnable(GL_FOG);
            break;

        case MODE_DEPTH_FLAT:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
            glDisable(GL_CULL_FACE);
            glEnable(GL_LIGHTING);
            glShadeModel(GL_FLAT); // Tô bóng phẳng
            glEnable(GL_FOG);
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
            glEnable(GL_FOG);
            break;
    }
}

// ============================================================================
// 2. HỆ THỐNG CHIẾU SÁNG & CHU KỲ NGÀY / ĐÊM (SPOTLIGHT)
// ============================================================================
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
        GLfloat spotPos[4]  = { carX, 0.65f, carZ - 1.9f, 1.0f }; // W = 1: Vị trí tại đầu xe
        GLfloat spotDir[3]  = { 0.0f, -0.12f, -1.0f };            // Hướng chiếu chếch xuống mặt đường
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
// 3. THUẬT TOÁN SƯƠNG MÙ CHIỀU SÂU (GL_FOG)
// ============================================================================
/**
 * Giải thích nguyên lý toán học cho báo cáo:
 * - Phương trình hòa trộn sương mù: C = f * C_object + (1 - f) * C_fog
 * - Với mô hình tuyến tính GL_LINEAR: f = (z_end - z) / (z_end - z_start)
 * - Khi khoảng cách z tiến gần z_end, đối tượng mờ dần và hòa quyện hoàn toàn
 *   vào màu nền bầu trời, che đi vết cắt đột ngột tại mặt phẳng cắt xa zFar.
 */
void setupFog(bool isNight) {
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, 35.0f);
    glFogf(GL_FOG_END, 160.0f);

    if (!isNight) {
        // Sương mù ban ngày màu xanh da trời nhạt đồng bộ với glClearColor
        GLfloat fogColor[4] = { 0.52f, 0.78f, 0.95f, 1.0f };
        glFogfv(GL_FOG_COLOR, fogColor);
    } else {
        // Sương mù ban đêm màu xanh đen thẫm
        GLfloat fogColor[4] = { 0.04f, 0.04f, 0.09f, 1.0f };
        glFogfv(GL_FOG_COLOR, fogColor);
    }
}

// ============================================================================
// 4. THIẾT LẬP VẬT LIỆU THEO MÔ HÌNH CHIẾU SÁNG PHONG
// ============================================================================
void applyMaterial(float r, float g, float b, float shininess, float specVal) {
    glColor3f(r, g, b);

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
