#include "sdf_function.h"

namespace {
float sdTorus(vec3 p, vec2 t)
{
    vec2 q = vec2(length(p.xy) - t.x, p.z);
    return length(q) - t.y;
}
}

float map(const vec3& p)
{
    return sdTorus(p, vec2(.37, .1));
}
