#include "core/Application.h"

#include "gfx/Image.h"
#include "gfx/Mesh.h"
#include "math/Transform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdio>
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
    , _userProfile(UserProfile::load("save/user.txt"))
    , _font(_theme.resolveFontPath(), _theme.fontSize())
    , _titleFont(_theme.resolveFontPath(), _theme.titleSize())
    , _ui(_font, _titleFont)
{
    glViewport(0, 0, _window.width(), _window.height());
    glEnable(GL_DEPTH_TEST);
    _player.setRules(_rules);

    const std::string audioBase = "assets/audio/" + _rules.soundPack() + "/";

    _audio.load(Audio::Sound::Jump,    audioBase + "jump.wav");
    _audio.load(Audio::Sound::Land,    audioBase + "land.wav");
    _audio.load(Audio::Sound::Coin,    audioBase + "coin.wav");
    _audio.load(Audio::Sound::Powerup, audioBase + "powerup.wav");
    _audio.load(Audio::Sound::Hit,     audioBase + "hit.wav");

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
    _coins.load(mesh, _rules);
    _powerups.load(mesh, _rules);
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
                _menu.setState(Menu::State::Paused);
                break;
            case Menu::State::Paused:
                _menu.setState(Menu::State::Playing);
                _resumeCountdown = 3.5f;
                break;
            case Menu::State::GameOver:
            case Menu::State::Stats:
            case Menu::State::Shop:
                _menu.setState(Menu::State::Main);
                break;
            case Menu::State::Main:
                _window.close();
                break;
        }
        return;
    }

    if (_menu.state() == Menu::State::Shop) {
        Shop::Action shopAction = _shop.update(_input);

        if (shopAction == Shop::Action::Buy) {
            PickupType type = _shop.selectedPickup();
            if (type != PickupType::None) {
                int cost = _userProfile.upgradeCost(
                    type,
                    _rules.upgradeBaseCost(),
                    _rules.upgradeCostStep());

                if (_userProfile.tryUpgrade(type, cost, _rules.upgradeMaxLevel()))
                    _userProfile.save("save/user.txt");
            }
        } else if (shopAction == Shop::Action::Back) {
            _menu.setState(Menu::State::Main);
        }
        return;
    }

    Menu::Action action = _menu.update(_input);

    switch (action) {
        case Menu::Action::Play:
            resetWorld();
            _menu.setState(Menu::State::Playing);
            _audio.startAmbient("assets/audio/" + _rules.soundPack() + "/ambient.wav");
            break;
        case Menu::Action::Resume:
            _menu.setState(Menu::State::Playing);
            _resumeCountdown = 3.5f;
            break;
        case Menu::Action::Stats:
            _menu.setState(Menu::State::Stats);
            break;
        case Menu::Action::OpenShop:
            _shop.resetSelection();
            _menu.setState(Menu::State::Shop);
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

    if (_resumeCountdown > 0.0f) {
        _resumeCountdown -= dt;
        if (_resumeCountdown < 0.0f)
            _resumeCountdown = 0.0f;
        return;
    }

    Player::UpdateEvents ev = _player.update(dt, _input);

    if (ev.jumped)
        _audio.play(Audio::Sound::Jump);
    if (ev.landed)
        _audio.play(Audio::Sound::Land);

    _obstacles.update(_player.position().z());
    _coins.update(_player.position().z(), dt, _obstacles.obstacles());
    _powerups.update(_player.position().z(), dt, _obstacles.obstacles());

    const Vec3& pp = _player.position();

    float coinRadiusXZ = _player.hasMagnet() ? 5.0f : 0.8f;
    float coinRadiusY  = _player.hasMagnet() ? 5.0f : 1.2f;

    for (std::size_t i = 0; i < _coins.coins().size(); ) {
        const Pickup& c = _coins.coins()[i];
        const Vec3&  cp = c.position();

        bool near = (std::abs(pp.x() - cp.x()) < coinRadiusXZ)
                 && (std::abs(pp.z() - cp.z()) < coinRadiusXZ)
                 && (std::abs(pp.y() - cp.y()) < coinRadiusY);

        if (near) {
            _player.addCoins(_player.hasDoubleCoins() ? 2 : 1);
            _coins.removeCoin(i);
            _audio.play(Audio::Sound::Coin);
        } else {
            ++i;
        }
    }

    for (std::size_t i = 0; i < _powerups.pickups().size(); ) {
        const Pickup& p  = _powerups.pickups()[i];
        const Vec3&   kp = p.position();

        bool near = (std::abs(pp.x() - kp.x()) < 1.0f)
                 && (std::abs(pp.z() - kp.z()) < 1.0f)
                 && (std::abs(pp.y() - kp.y()) < 1.5f);

        if (near) {
            float duration = _userProfile.effectiveDuration(
                p.type(),
                _rules.powerupBaseDuration(),
                _rules.powerupUpgradeStep());
            _player.activatePowerup(p.type(), duration);
            _powerups.removePickup(i);
            _audio.play(Audio::Sound::Powerup);
        } else {
            ++i;
        }
    }

    for (std::size_t i = 0; i < _obstacles.obstacles().size(); ++i) {
        if (collides(_player, _obstacles.obstacles()[i])) {
            if (_player.consumeShield()) {
                _obstacles.removeObstacle(i);
                _audio.play(Audio::Sound::Hit);
                break;
            }

            int distance     = static_cast<int>(_player.distance());
            int coins        = _player.coins();
            int previousBest = _stats.bestDistance();

            _stats.recordGame(distance, coins);
            _stats.save("save/stats.txt");

            _userProfile.addCoins(coins);
            _userProfile.save("save/user.txt");

            _newRecord = (distance > previousBest);
            _menu.setState(Menu::State::GameOver);
            _audio.play(Audio::Sound::Hit);
            _audio.stopAmbient();
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
    _coins.reset();
    _powerups.reset();
    _newRecord       = false;
    _resumeCountdown = 0.0f;
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
    float w = static_cast<float>(_window.width());
    float h = static_cast<float>(_window.height());

    float wobble = 0.5f + 0.5f * std::sin(_elapsed * 9.0f) * std::sin(_elapsed * 5.7f);
    float flicker     = 0.92f + 0.06f * wobble;
    bool  hintVisible = (std::fmod(_elapsed, 1.2f) < 0.6f);

    bool showWorld = (_menu.state() == Menu::State::Playing ||
                      _menu.state() == Menu::State::GameOver ||
                      _menu.state() == Menu::State::Paused);

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

        _shader.setVec3("uBaseColor", Vec3(1.0f, 0.85f, 0.2f));
        for (const Pickup& c : _coins.coins())
            drawEntity(c, viewProj);

        for (const Pickup& p : _powerups.pickups()) {
            Vec3 color(1.0f, 1.0f, 1.0f);
            if (p.type() == PickupType::Magnet)
                color = Vec3(0.3f, 0.6f, 1.0f);
            else if (p.type() == PickupType::Shield)
                color = Vec3(0.3f, 1.0f, 0.9f);
            else if (p.type() == PickupType::DoubleCoins)
                color = Vec3(1.0f, 0.4f, 0.9f);

            _shader.setVec3("uBaseColor", color);
            drawEntity(p, viewProj);
        }

        for (const Entity& e : _entities)
            drawEntity(e, viewProj);
    }

    _ui.beginFrame(w, h);
    _ui.setFlicker(flicker);
    _ui.setHintVisible(hintVisible);

    if (_menu.state() == Menu::State::Playing ||
        _menu.state() == Menu::State::Paused) {
        std::string score = std::to_string(static_cast<int>(_player.distance()));
        _ui.drawText(score, 20.0f, 20.0f, _theme.foreground());

        std::string coinText = "Coins: " + std::to_string(_player.coins());
        _ui.drawText(coinText, 20.0f, 60.0f, _theme.accent());

        float hudY = 100.0f;
        char  buf[64];

        if (_player.hasMagnet()) {
            std::snprintf(buf, sizeof(buf), "MAGNET %.1f", _player.magnetTimer());
            _ui.drawText(buf, 20.0f, hudY, _theme.accent());
            hudY += 36.0f;
        }
        if (_player.hasShield()) {
            std::snprintf(buf, sizeof(buf), "SHIELD %.1f", _player.shieldTimer());
            _ui.drawText(buf, 20.0f, hudY, _theme.accent());
            hudY += 36.0f;
        }
        if (_player.hasDoubleCoins()) {
            std::snprintf(buf, sizeof(buf), "DOUBLE %.1f", _player.doubleCoinsTimer());
            _ui.drawText(buf, 20.0f, hudY, _theme.accent());
            hudY += 36.0f;
        }
    }

    if (_menu.state() == Menu::State::Main) {
        std::string balance = "Coins: " + std::to_string(_userProfile.coins());
        float textW = _ui.measureText(balance);
        _ui.drawText(balance, w - textW - 20.0f, 20.0f, _theme.accent());
    }

    if (_resumeCountdown > 0.0f)
        drawCountdown(w, h);

    if (_menu.state() != Menu::State::Playing) {
        if (!showWorld) {
            if (_hasBackground)
                _ui.drawBackground(_backgroundTexture);
            else
                _ui.drawRect(0.0f, 0.0f, w, h, _theme.background());
        }

        if (_menu.state() == Menu::State::Shop) {
            _shop.render(_ui, _theme, _userProfile, _rules, w, h);
        } else {
            _menu.render(_ui, _theme, w, h,
                         static_cast<int>(_player.distance()),
                         _player.coins(),
                         _stats,
                         _newRecord);
        }
    }

    _ui.endFrame();
}

void Application::drawCountdown(float screenW, float screenH) {
    static const char* labels[] = { "3", "2", "1", "GO" };

    float elapsed     = 3.5f - _resumeCountdown;
    int   index       = std::min(static_cast<int>(elapsed), 3);
    float timeInLabel = elapsed - static_cast<float>(index);

    float popT    = std::min(1.0f, timeInLabel / 0.15f);
    float easeOut = 1.0f - (1.0f - popT) * (1.0f - popT);

    bool isGo = (index == 3);

    float startScale = isGo ? 1.8f : 1.5f;
    float baseScale  = startScale - (startScale - 1.0f) * easeOut;
    float offsetY    = -60.0f * (1.0f - easeOut);

    float pulse = 1.0f;
    if (popT >= 1.0f)
        pulse = 1.0f + 0.02f * std::sin(timeInLabel * 8.0f);

    float scale = baseScale * pulse;
    float cx    = screenW * 0.5f;
    float cy    = screenH * 0.5f + offsetY;

    Vec3 accent = _theme.accent();

    _ui.drawTitleCenteredScaled(labels[index], cx, cy, accent, scale);

    float textW = _ui.measureTitle(labels[index]) * scale;
    float textH = _ui.measureTitleHeight()        * scale;

    float halfW = textW * 0.70f + 16.0f;
    float halfH = textH * 0.70f + 16.0f;

    float brW = 3.0f;
    float brL = 18.0f + 6.0f * std::sin(timeInLabel * 6.0f);
    Vec3  brColor(accent.x() * 0.6f, accent.y() * 0.6f, accent.z() * 0.6f);

    _ui.drawRect(cx - halfW,          cy - halfH,          brL, brW, brColor);
    _ui.drawRect(cx - halfW,          cy - halfH,          brW, brL, brColor);

    _ui.drawRect(cx + halfW - brL,    cy - halfH,          brL, brW, brColor);
    _ui.drawRect(cx + halfW - brW,    cy - halfH,          brW, brL, brColor);

    _ui.drawRect(cx - halfW,          cy + halfH - brW,    brL, brW, brColor);
    _ui.drawRect(cx - halfW,          cy + halfH - brL,    brW, brL, brColor);

    _ui.drawRect(cx + halfW - brL,    cy + halfH - brW,    brL, brW, brColor);
    _ui.drawRect(cx + halfW - brW,    cy + halfH - brL,    brW, brL, brColor);
}