#include "experiments/shadertoy.h"

#define LOG(x) std::cout << x << std::endl

int main()
{
    drawImage(width, height, "image.bmp", [](const vec2& fragCoord) -> vec4 {
        vec2 uv = fragCoord / iResolution.xy;
        vec3 col = 0.5 + 0.5 * cos(iTime + uv.xyx + vec3(0, 2, 4));
        return vec4(col, 1);
    });

    return 0;
}
