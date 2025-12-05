#include <stdint.h>
void DrawSprite(uint32_t* fb, int fbW, int fbH,
    const unsigned char* px, int w, int h,
    int x0, int y0,
    float scaleX = 1.0f, float scaleY = 1.0f);