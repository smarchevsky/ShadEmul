#include "shader_lib.h"
#include <functional>

#define LOG(x) std::cout << x std::endl;

void drawImage(int w, int h, std::function<vec4(const vec2&)> shaderFunc)
{
    uint8_t* pixels = new uint8_t[w * h * 3];
    const vec2 invSize(1.f / w, 1.f / h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int index = (y * w + x) * 3;
            vec2 coord = vec2(x, y);
            vec4 res = shaderFunc(coord);
            res *= 255;
            res = clamp(res, 0, 255);
            pixels[index + 0] = res.b; // Blue
            pixels[index + 1] = res.g; // Green
            pixels[index + 2] = res.r; // Red
        }
    }

    WriteBMP("output.bmp", w, h, pixels);
    delete[] pixels;
}

namespace {
const float iTime = 0;
const int width = 256;
const int height = 256;
const vec2 iResolution(width, height);
}

int main()
{

    drawImage(width, height, [](const vec2& fragCoord) -> vec4 {
        vec2 uv = fragCoord / iResolution.xy;

        // Time varying pixel color
        vec3 col = 0.5 + 0.5 * cos(iTime + uv.xyx + vec3(0, 2, 4));

        // Output to screen
        return vec4(col, 1.0);
    });

    return 0;
}
