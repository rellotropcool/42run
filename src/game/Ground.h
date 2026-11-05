#ifndef GROUND_H
#define GROUND_H

#include "gfx/Mesh.h"
#include "gfx/Shader.h"
#include "math/Mat4.h"

class Ground {
    private:
        Shader _shader;
        Mesh   _mesh;
    public:
        Ground();
        ~Ground() = default;

        Ground(const Ground&)            = delete;
        Ground& operator=(const Ground&) = delete;

        void render(const Mat4& viewProj, float cameraZ);
};

#endif