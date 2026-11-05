#include "game/Ground.h"

#include "gfx/VertexArray.h"
#include "math/Transform.h"
#include "math/Vec3.h"

#include <cstdint>
#include <vector>

namespace {

const std::vector<Vertex>& quadVerts() {
    static const std::vector<Vertex> v = {
        {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    };
    return v;
}

const std::vector<std::uint32_t>& quadIndices() {
    static const std::vector<std::uint32_t> i = { 0, 1, 2,  0, 2, 3 };
    return i;
}

}

Ground::Ground()
    : _shader("shaders/ground.vert", "shaders/ground.frag")
    , _mesh(quadVerts().data(), quadVerts().size() * sizeof(Vertex),
            quadIndices().data(), quadIndices().size(),
            makeVertexLayout())
{}

void Ground::render(const Mat4& viewProj, float cameraZ) {
    const float groundY      = -0.5f;
    const float groundWidth  = 10.0f;
    const float groundLength = 500.0f;
    const float tileSize     = 4.0f;

    const Vec3 color1 = Vec3(0.86f, 0.76f, 0.55f);
    const Vec3 color2 = Vec3(0.74f, 0.64f, 0.44f);

    Mat4 model = Transform::translate(
        Mat4::identity(),
        Vec3(0.0f, groundY, cameraZ - groundLength * 0.5f));
    model = Transform::scale(model, Vec3(groundWidth, 1.0f, groundLength));

    _shader.bind();
    _shader.setMat4 ("uMVP",          viewProj * model);
    _shader.setVec2 ("uTileCount",    Vec2(groundWidth  / tileSize,
                                           groundLength / tileSize));
    _shader.setFloat("uScrollOffset", cameraZ / tileSize);
    _shader.setVec3 ("uColor1",       color1);
    _shader.setVec3 ("uColor2",       color2);

    _mesh.draw();
}