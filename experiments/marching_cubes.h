#ifndef MARCHING_CUBES_H
#define MARCHING_CUBES_H

#include "shader_lib.h"
#include <functional>

class MarchingCubes {
public:
    static void march(vec3 resolution, vec3 bMin, vec3 bMax, const char* filePath, std::function<float(vec3)> func);
};

#endif // MARCHING_CUBES_H
