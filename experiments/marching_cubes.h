#ifndef MARCHING_CUBES_H
#define MARCHING_CUBES_H

#include "shader_lib.h"
#include <functional>

class MarchingCubes {
public:
    struct Triangle {
        vec3 p[3];
    };

    static void march(std::function<float(vec3)> func);
};

#endif // MARCHING_CUBES_H
