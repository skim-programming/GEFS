#include "component.h"
#include "stb_image.h"

SpriteRenderer* SpriteRenderer::SetSprite(std::string path, int x_, int y_) {
    x = x_;
    y = y_;

    unsigned char* data = stbi_load(path.c_str(), &w, &h, nullptr, 4);
    if (!data) return this;

    pixels.assign(data, data + (w * h * 4));
    stbi_image_free(data);

    sw = 1.0f;
    sh = 1.0f;

    return this;
}

SpriteRenderer::SpriteRenderer(std::string path, int x_, int y_) {
    x = x_;
	y = y_;
	unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 4); 
	this->pixels.assign(data, data + w * h * 4); 
	stbi_image_free(data);
}

SpriteRenderer* SpriteRenderer::SetSpriteScale(float sw_, float sh_){
    this->sw = sw_;
    this->sh = sh_;
    return this;
}


static void ScaleImageNN(
    const unsigned char* src, int srcW, int srcH,
    unsigned char* dst, int dstW, int dstH)
{
    for (int y = 0; y < dstH; y++) {
        int sy = (y * srcH) / dstH;
        for (int x = 0; x < dstW; x++) {
            int sx = (x * srcW) / dstW;

            const unsigned char* sp = src + 4 * (sy * srcW + sx);
            unsigned char* dp = dst + 4 * (y * dstW + x);

            dp[0] = sp[0];
            dp[1] = sp[1];
            dp[2] = sp[2];
            dp[3] = sp[3];
        }
    }
}