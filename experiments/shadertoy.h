#ifndef SHADERTOY_H
#define SHADERTOY_H
#include "shader_lib.h"
#include "utils.h"

namespace {
const float iTime = 0;
const int width = 256;
const int height = 256;
const vec2 iResolution(width, height);
}

void drawImage(int w, int h, const char* path, std::function<vec4(const vec2&)> shaderFunc)
{
    uint8_t* pixels = new uint8_t[w * h * 3];
    const vec2 invSize(1.f / w, 1.f / h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int index = (y * w + x) * 3;
            // vec2 coord = vec2(x, y);
            vec2 coord = vec2(x, h - y - 1);
            vec4 res = shaderFunc(coord);
            res = clamp(res * 255, 0, 255);
            pixels[index + 0] = res.b;
            pixels[index + 1] = res.g;
            pixels[index + 2] = res.r;
        }
    }

    Utils::WriteBMP(path, w, h, pixels);
    delete[] pixels;
}

#endif // SHADERTOY_H
