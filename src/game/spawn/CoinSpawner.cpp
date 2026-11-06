#include "game/spawn/CoinSpawner.h"

#include "game/state/GameRules.h"
#include "gfx/Mesh.h"
#include "math/Vec3.h"

#include <cmath>
#include <random>

namespace {

std::mt19937& rng() {
    static std::mt19937 engine(std::random_device{}());
    return engine;
}

}

void CoinSpawner::load(Mesh& mesh, const GameRules& rules) {
    _mesh         = &mesh;
    _rules        = &rules;
    _currentLane  = 1;
    _targetLane   = 1;
    _straightLeft = _segmentLength;
    _coins.clear();
    _nextSpawnZ = -rules.warmupDistance();
}

void CoinSpawner::reset() {
    _coins.clear();
    _currentLane  = 1;
    _targetLane   = 1;
    _straightLeft = _segmentLength;
    if (_rules)
        _nextSpawnZ = -_rules->warmupDistance();
}

void CoinSpawner::chooseTargetLane() {
    std::uniform_int_distribution<int> dist(0, 3);
    int roll = dist(rng());

    int candidate = _currentLane;
    if (roll == 0)      candidate = _currentLane - 1;
    else if (roll == 1) candidate = _currentLane + 1;

    if (candidate < 0) candidate = _currentLane;
    if (candidate > 2) candidate = _currentLane;

    _targetLane = candidate;

    if (_targetLane == _currentLane)
        _straightLeft = _segmentLength;
}

bool CoinSpawner::advanceSnake() {
    if (_currentLane != _targetLane) {
        _currentLane  = _targetLane;
        _straightLeft = _segmentLength;
        return false;
    }

    --_straightLeft;
    if (_straightLeft <= 0)
        chooseTargetLane();

    return true;
}

bool CoinSpawner::blockedAt(int lane, float z,
                            const std::vector<Obstacle>& obstacles) const {
    if (!_rules)
        return true;

    float laneX = static_cast<float>(lane - 1) * _rules->laneWidth();

    for (const Obstacle& o : obstacles) {
        if (std::abs(o.position().z() - z) > 0.8f)
            continue;
        if (std::abs(o.position().x() - laneX) >= 0.7f)
            continue;

        float obsHalfH  = o.scale().y() * 0.5f;
        float obsBottom = o.position().y() - obsHalfH;
        float obsTop    = o.position().y() + obsHalfH;

        if (_yLevel > obsBottom && _yLevel < obsTop)
            return true;
    }
    return false;
}

void CoinSpawner::update(float playerZ, float dt,
                         const std::vector<Obstacle>& obstacles) {
    if (!_mesh || !_rules)
        return;

    while (_nextSpawnZ > playerZ - _spawnAhead) {
        bool placeCoin = advanceSnake();

        if (placeCoin && !blockedAt(_currentLane, _nextSpawnZ, obstacles)) {
            float laneX = static_cast<float>(_currentLane - 1) * _rules->laneWidth();
            _coins.emplace_back(*_mesh,
                                Vec3(laneX, _yLevel, _nextSpawnZ),
                                Vec3(0.4f, 0.4f, 0.1f),
                                PickupType::Coin);
        }

        _nextSpawnZ -= _rules->coinInterval();
    }

    float cutoff = playerZ + _pruneBehind;
    auto  it     = _coins.begin();
    while (it != _coins.end()) {
        if (it->position().z() > cutoff)
            it = _coins.erase(it);
        else
            ++it;
    }

    for (Pickup& c : _coins)
        c.update(dt);
}

const std::vector<Pickup>& CoinSpawner::coins() const {
    return _coins;
}

void CoinSpawner::removeCoin(std::size_t index) {
    if (index < _coins.size())
        _coins.erase(_coins.begin() + static_cast<std::ptrdiff_t>(index));
}