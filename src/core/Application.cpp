#include "core/Application.h"

#include "gfx/Image.h"
#include "gfx/Mesh.h"
#include "math/Transform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <chrono>
#include <cmath>
#include <string>

namespace {

constexpr float _playerHalfWidth = 0.4f;
constexpr float _playerHalfDepth = 0.4f;
constexpr float _obstacleHalfW   = 0.4f;
constexpr float _obstacleHalfD   = 0.4f;

}

Application::Application(const GameRules& rules, const Theme& theme)
    : _window(1280, 720, "42run")
    , _input(_window)
    , _shader("shaders/basic.vert", "shaders/basic.frag")
    , _rules(rules)
    , _theme(theme)
    , _stats(Stats::load("save/stats.txt"))
    , _font(_theme.resolveFontPath(), _theme.fontSize())
    , _titleFont(_theme.resolveFontPath(), _theme.titleSize())
    , _ui(_font, _titleFont)
{
    glViewport(0, 0, _window.width(), _window.height());
    glEnable(GL_DEPTH_TEST);
    _player.setRules(_rules);

    if (!_theme.backgroundImage().empty()) {
        std::string path = _theme.resolveBackgroundPath();
        ImageData   img  = Image::load(path);
        _backgroundTexture.setData(img.width, img.height,
                                   GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE,
                                   img.pixels.data());
        _hasBackground = true;
    }
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
        _elapsed += dt;

        update(dt);
        render();

        _window.swapBuffers();
    }
    return 0;
}

void Application::update(float dt) {
    if (_input.wasPressed(GLFW_KEY_ESCAPE)) {
        switch (_menu.state()) {
            case Menu::State::Playing:
            case Menu::State::GameOver:
            case Menu::State::Stats:
                _menu.setState(Menu::State::Main);
                break;
            case Menu::State::Main:
                _window.close();
                break;
        }
        return;
    }

    Menu::Action action = _menu.update(_input);

    switch (action) {
        case Menu::Action::Play:
            resetWorld();
            _menu.setState(Menu::State::Playing);
            break;
        case Menu::Action::Stats:
            _menu.setState(Menu::State::Stats);
            break;
        case Menu::Action::Back:
            _menu.setState(Menu::State::Main);
            break;
        case Menu::Action::Quit:
            _window.close();
            return;
        case Menu::Action::None:
            break;
    }

    if (_menu.state() != Menu::State::Playing)
        return;

    _player.update(dt, _input);
    _obstacles.update(_player.position().z());

    for (const Obstacle& obs : _obstacles.obstacles()) {
        if (collides(_player, obs)) {
            int distance = static_cast<int>(_player.distance());
            _stats.recordGame(distance);
            _stats.save("save/stats.txt");
            _menu.setState(Menu::State::GameOver);
            break;
        }
    }

    float z = _player.position().z();
    _camera.setTarget(Vec3(0.0f, 0.0f, z));
    _camera.setPosition(Vec3(0.0f, 2.0f, z + 5.0f));
}

void Application::resetWorld() {
    _player.reset();
    _obstacles.reset();
    _camera.setTarget(Vec3(0.0f, 0.0f, 0.0f));
    _camera.setPosition(Vec3(0.0f, 2.0f, 5.0f));
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
    bool showWorld = (_menu.state() == Menu::State::Playing ||
                      _menu.state() == Menu::State::GameOver);

    if (showWorld) {
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
    }

    float w = static_cast<float>(_window.width());
    float h = static_cast<float>(_window.height());

    _ui.beginFrame(w, h, _elapsed);

    if (_menu.state() == Menu::State::Playing) {
        std::string score = std::to_string(static_cast<int>(_player.distance()));
        _ui.drawText(score, 20.0f, 20.0f, _theme.foreground());
    } else {
        if (!showWorld) {
            if (_hasBackground)
                _ui.drawBackground(_backgroundTexture);
            else
                _ui.drawRect(0.0f, 0.0f, w, h, _theme.background());
        }

        _menu.render(_ui, _theme, w, h,
                     static_cast<int>(_player.distance()), _stats);
    }

    _ui.endFrame();
}