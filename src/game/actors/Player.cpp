#include "game/actors/Player.h"

#include "core/Input.h"
#include "game/state/GameRules.h"

#include <GLFW/glfw3.h>

#include <algorithm>

void Player::setRules(const GameRules& rules) {
    _rules = &rules;
}

void Player::update(float dt, const Input& input) {
    if (!_rules)
        return;

    _magnetTimer      = std::max(0.0f, _magnetTimer      - dt);
    _shieldTimer      = std::max(0.0f, _shieldTimer      - dt);
    _doubleCoinsTimer = std::max(0.0f, _doubleCoinsTimer - dt);

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
    _lane             = 1;
    _vy               = 0.0f;
    _jumpOffset       = 0.0f;
    _onGround         = true;
    _sliding          = false;
    _slideTimer       = 0.0f;
    _coins            = 0;
    _magnetTimer      = 0.0f;
    _shieldTimer      = 0.0f;
    _doubleCoinsTimer = 0.0f;
}

int   Player::lane()             const { return _lane; }
bool  Player::onGround()         const { return _onGround; }
bool  Player::sliding()          const { return _sliding; }
float Player::distance()         const { return -position().z(); }
int   Player::coins()            const { return _coins; }
float Player::magnetTimer()      const { return _magnetTimer; }
float Player::shieldTimer()      const { return _shieldTimer; }
float Player::doubleCoinsTimer() const { return _doubleCoinsTimer; }

bool Player::hasMagnet()      const { return _magnetTimer      > 0.0f; }
bool Player::hasShield()      const { return _shieldTimer      > 0.0f; }
bool Player::hasDoubleCoins() const { return _doubleCoinsTimer > 0.0f; }

void Player::addCoins(int amount) {
    if (amount > 0)
        _coins += amount;
}

void Player::activatePowerup(PickupType type, float duration) {
    if (duration <= 0.0f)
        return;

    if (type == PickupType::Magnet)
        _magnetTimer = duration;
    else if (type == PickupType::Shield)
        _shieldTimer = duration;
    else if (type == PickupType::DoubleCoins)
        _doubleCoinsTimer = duration;
}

bool Player::consumeShield() {
    if (_shieldTimer > 0.0f) {
        _shieldTimer = 0.0f;
        return true;
    }
    return false;
}