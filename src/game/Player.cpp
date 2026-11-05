#include "game/Player.h"

#include "core/Input.h"
#include "game/GameRules.h"

#include <GLFW/glfw3.h>

#include <algorithm>

void Player::setRules(const GameRules& rules) {
    _rules = &rules;
}

void Player::update(float dt, const Input& input) {
    if (!_rules)
        return;

    if (input.wasPressed(GLFW_KEY_A) || input.wasPressed(GLFW_KEY_LEFT))
        if (_lane > 0) --_lane;

    if (input.wasPressed(GLFW_KEY_D) || input.wasPressed(GLFW_KEY_RIGHT))
        if (_lane < _laneCount - 1) ++_lane;

    if (input.wasPressed(GLFW_KEY_S) || input.wasPressed(GLFW_KEY_DOWN)) {
        _sliding    = true;
        _slideTimer = _rules->slideDuration();
        _vy         = 0.0f;
        _jumpOffset = 0.0f;
        _onGround   = true;
    }

    if ((input.wasPressed(GLFW_KEY_SPACE) ||
         input.wasPressed(GLFW_KEY_W) ||
         input.wasPressed(GLFW_KEY_UP)) && _onGround) {
        _sliding    = false;
        _slideTimer = 0.0f;
        _vy         = _rules->jumpVelocity();
        _onGround   = false;
    }

    if (_sliding) {
        _slideTimer -= dt;
        if (_slideTimer <= 0.0f)
            _sliding = false;
    }

    if (!_onGround)
        _vy -= _rules->gravity() * dt;

    _jumpOffset += _vy * dt;
    if (_jumpOffset <= 0.0f) {
        _jumpOffset = 0.0f;
        _vy         = 0.0f;
        _onGround   = true;
    }

    if (_sliding)
        setScale(Vec3(1.0f, 0.5f, 1.0f));
    else
        setScale(Vec3(1.0f, 1.0f, 1.0f));

    float halfHeight = scale().y() * 0.5f;
    float y          = -0.5f + halfHeight + _jumpOffset;

    Vec3  current = position();
    float targetX = static_cast<float>(_lane - 1) * _rules->laneWidth();
    float x       = current.x() + (targetX - current.x()) *
                    std::min(1.0f, _rules->laneLerpSpeed() * dt);
    float z       = current.z() - _rules->forwardSpeed() * dt;

    setPosition(Vec3(x, y, z));
}

void Player::reset() {
    setPosition(Vec3(0.0f, 0.0f, 0.0f));
    setRotation(Vec3(0.0f));
    setScale(Vec3(1.0f));
    _lane       = 1;
    _vy         = 0.0f;
    _jumpOffset = 0.0f;
    _onGround   = true;
    _sliding    = false;
    _slideTimer = 0.0f;
}

int   Player::lane()     const { return _lane; }
bool  Player::onGround() const { return _onGround; }
bool  Player::sliding()  const { return _sliding; }
float Player::distance() const { return -position().z(); }