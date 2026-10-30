#include "core/Window.h"
#include "gfx/Mesh.h"
#include "gfx/ObjLoader.h"
#include "gfx/Shader.h"
#include "math/Mat4.h"
#include "math/Transform.h"
#include "math/Vec3.h"

#include <glad/glad.h>

#include <chrono>
#include <exception>
#include <iostream>

int main() {
    try {
        Window window(1280, 720, "aule-bre - 42Run");

        glViewport(0, 0, window.width(), window.height());
        glEnable(GL_DEPTH_TEST);

        Shader shader("shaders/basic.vert", "shaders/basic.frag");

        ObjLoader::Result obj = ObjLoader::load("assets/models/cube.obj");
        Mesh cube(obj.vertices.data(), obj.vertices.size() * sizeof(Vertex),
                  obj.indices.data(),  obj.indices.size(),
                  makeVertexLayout());

        auto start = std::chrono::steady_clock::now();

        while (!window.shouldClose()) {
            window.pollEvents();
            window.clear();

            float time = std::chrono::duration<float>(
                std::chrono::steady_clock::now() - start).count();

            float aspect = static_cast<float>(window.width()) /
                           static_cast<float>(window.height());

            Mat4 model = Transform::rotate(Mat4::identity(), time * 0.7f,
                                           Vec3(0.0f, 1.0f, 0.0f));
            model = Transform::rotate(model, time * 0.4f,
                                      Vec3(1.0f, 0.0f, 0.0f));

            Mat4 view = Transform::lookAt(Vec3(0.0f, 0.0f, 2.5f),
                                          Vec3(0.0f),
                                          Vec3(0.0f, 1.0f, 0.0f));
            Mat4 proj = Transform::perspective(Transform::radians(60.0f),
                                               aspect, 0.1f, 100.0f);

            shader.bind();
            shader.setMat4("uMVP",       proj * view * model);
            shader.setMat4("uModel",     model);
            shader.setVec3("uLightDir",  Vec3(0.5f, 1.0f, 0.7f));
            shader.setVec3("uBaseColor", Vec3(0.9f, 0.6f, 0.2f));
            cube.draw();

            window.swapBuffers();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}