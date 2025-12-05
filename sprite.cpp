#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "sprite.h"
#include <cmath>
#include <stdint.h>

static uint32_t PackColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (a << 24) | (r << 16) | (g << 8) | b;
}

void DrawSprite(uint32_t* fb, int fbW, int fbH,
    const unsigned char* px, int w, int h,
    int x0, int y0,
    float scaleX, float scaleY)
{
    int sw = int(w * scaleX);
    int sh = int(h * scaleY);

    for (int y = 0; y < sh; y++) {
        int fbY = y0 + y;
        if (fbY < 0 || fbY >= fbH) continue;

        int srcY = std::min(int(y / scaleY), h - 1); // nearest-neighbor

        for (int x = 0; x < sw; x++) {
            int fbX = x0 + x;
            if (fbX < 0 || fbX >= fbW) continue;

            int srcX = std::min(int(x / scaleX), w - 1);

            const unsigned char* p = px + 4 * (srcY * w + srcX);

            uint8_t r = p[0];
            uint8_t g = p[1];
            uint8_t b = p[2];
            uint8_t a = p[3];

            uint32_t& dst = fb[fbY * fbW + fbX];

            if (a == 255) {
                dst = PackColor(r, g, b, a);
                continue;
            }

            uint8_t dr = (dst >> 16) & 0xFF;
            uint8_t dg = (dst >> 8) & 0xFF;
            uint8_t db = dst & 0xFF;

            float af = a / 255.0f;

            uint8_t nr = dr * (1 - af) + r * af;
            uint8_t ng = dg * (1 - af) + g * af;
            uint8_t nb = db * (1 - af) + b * af;

            dst = PackColor(nr, ng, nb, 255);
        }
    }
}