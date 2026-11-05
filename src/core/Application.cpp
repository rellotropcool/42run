#include "core/Application.h"

#include "gfx/Mesh.h"
#include "math/Transform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>

namespace {

constexpr float _playerHalfWidth = 0.4f;
constexpr float _playerHalfDepth = 0.4f;
constexpr float _obstacleHalfW   = 0.4f;
constexpr float _obstacleHalfD   = 0.4f;

}

Application::Application(const GameRules& rules)
    : _window(1280, 720, "42run")
    , _input(_window)
    , _shader("shaders/basic.vert", "shaders/basic.frag")
    , _rules(rules)
{
    glViewport(0, 0, _window.width(), _window.height());
    glEnable(GL_DEPTH_TEST);
    _player.setRules(_rules);
}

void Application::setPlayerMesh(Mesh& mesh) {
    _player.setMesh(mesh);
}

void Application::setObstacleMesh(Mesh& mesh, const char* chunksPath) {
    _obstacles.load(mesh, chunksPath, _rules);
}

void Application::addEntity(Mesh& mesh, const Vec3& position,
                            const Vec3& rotation, const Vec3& scale) {
    _entities.emplace_back(mesh, position, rotation, scale);
}

int Application::run() {
    using clock = std::chrono::steady_clock;

    auto  start = clock::now();
    float prev  = 0.0f;

    while (!_window.shouldClose()) {
        _window.pollEvents();
        _input.update();
        _window.clear();

        float now = std::chrono::duration<float>(clock::now() - start).count();
        float dt  = now - prev;
        prev      = now;

        update(dt);
        render();

        _window.swapBuffers();
    }
    return 0;
}

void Application::update(float dt) {
    if (_input.wasPressed(GLFW_KEY_ESCAPE)) {
        _window.close();
        return;
    }

    if (_dead && _input.wasPressed(GLFW_KEY_R)) {
        _player.reset();
        _obstacles.reset();
        _dead = false;
    }

    if (_dead)
        return;

    _player.update(dt, _input);
    _obstacles.update(_player.position().z());

    for (const Obstacle& obs : _obstacles.obstacles()) {
        if (collides(_player, obs)) {
            _dead = true;
            std::cout << "Game over — distance: "
                      << static_cast<int>(_player.distance())
                      << " units. Press R to restart." << std::endl;
            break;
        }
    }

    float z = _player.position().z();
    _camera.setTarget(Vec3(0.0f, 0.0f, z));
    _camera.setPosition(Vec3(0.0f, 2.0f, z + 5.0f));
}

bool Application::collides(const Player& player, const Obstacle& obs) const {
    const Vec3& p = player.position();
    const Vec3& o = obs.position();

    if (std::abs(p.x() - o.x()) >= _playerHalfWidth + _obstacleHalfW)
        return false;
    if (std::abs(p.z() - o.z()) >= _playerHalfDepth + _obstacleHalfD)
        return false;

    float playerHalfH = player.scale().y() * 0.5f;
    float obsHalfH    = obs.scale().y() * 0.5f;

    float playerBottom = p.y() - playerHalfH;
    float playerTop    = p.y() + playerHalfH;
    float obsBottom    = o.y() - obsHalfH;
    float obsTop       = o.y() + obsHalfH;

    if (playerBottom >= obsTop) return false;
    if (playerTop    <= obsBottom) return false;

    return true;
}

void Application::drawEntity(const Entity& entity, const Mat4& viewProj) {
    assert(entity.mesh() && "drawEntity: null mesh");

    Mat4 model = Transform::translate(Mat4::identity(), entity.position());
    model = Transform::rotate(model, entity.rotation().y(), Vec3(0.0f, 1.0f, 0.0f));
    model = Transform::rotate(model, entity.rotation().x(), Vec3(1.0f, 0.0f, 0.0f));
    model = Transform::rotate(model, entity.rotation().z(), Vec3(0.0f, 0.0f, 1.0f));
    model = Transform::scale(model, entity.scale());

    _shader.setMat4("uMVP",   viewProj * model);
    _shader.setMat4("uModel", model);
    entity.mesh()->draw();
}

void Application::render() {
    float aspect = static_cast<float>(_window.width()) /
                   static_cast<float>(_window.height());

    Mat4 viewProj = _camera.viewProjection(aspect);

    _ground.render(viewProj, _camera.position().z());

    _shader.bind();
    _shader.setVec3("uLightDir", Vec3(0.5f, 1.0f, 0.7f));

    _shader.setVec3("uBaseColor", Vec3(0.9f, 0.6f, 0.2f));
    drawEntity(_player, viewProj);

    for (const Obstacle& obs : _obstacles.obstacles()) {
        Vec3 color(0.90f, 0.40f, 0.40f);
        if (obs.type() == ObstacleType::Full)
            color = Vec3(0.55f, 0.10f, 0.10f);
        else if (obs.type() == ObstacleType::Top)
            color = Vec3(0.90f, 0.15f, 0.25f);

        _shader.setVec3("uBaseColor", color);
        drawEntity(obs, viewProj);
    }

    for (const Entity& e : _entities)
        drawEntity(e, viewProj);

    _ui.beginFrame(static_cast<float>(_window.width()),
                   static_cast<float>(_window.height()));

    Vec3 uiColor(1.0f, 1.0f, 1.0f);
    if (_dead)
        uiColor = Vec3(1.0f, 0.3f, 0.3f);

    _ui.drawNumber(static_cast<int>(_player.distance()),
                   20.0f, 20.0f, 30.0f, 50.0f, uiColor);
    _ui.endFrame();
}