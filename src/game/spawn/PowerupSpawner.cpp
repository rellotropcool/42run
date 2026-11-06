#include "game/spawn/PowerupSpawner.h"

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

float uniformFloat(float lo, float hi) {
    std::uniform_real_distribution<float> dist(lo, hi);
    return dist(rng());
}

}

void PowerupSpawner::load(Mesh& mesh, const GameRules& rules) {
    _mesh       = &mesh;
    _rules      = &rules;
    _pickups.clear();
    _nextSpawnZ = -rules.warmupDistance() - rules.powerupInterval();
}

void PowerupSpawner::reset() {
    _pickups.clear();
    if (_rules)
        _nextSpawnZ = -_rules->warmupDistance() - _rules->powerupInterval();
}

PickupType PowerupSpawner::pickType() const {
    std::uniform_int_distribution<int> dist(0, 2);
    int roll = dist(rng());

    if (roll == 0)
        return PickupType::Magnet;
    if (roll == 1)
        return PickupType::Shield;
    return PickupType::DoubleCoins;
}

bool PowerupSpawner::laneClear(int lane, float z,
                               const std::vector<Obstacle>& obstacles) const {
    if (!_rules)
        return false;

    float laneX = static_cast<float>(lane - 1) * _rules->laneWidth();

    for (const Obstacle& obs : obstacles) {
        if (std::abs(obs.position().z() - z) > 4.0f)
            continue;
        if (std::abs(obs.position().x() - laneX) < 0.8f)
            return false;
    }
    return true;
}

bool PowerupSpawner::alreadyVisible(float playerZ) const {
    for (const Pickup& p : _pickups) {
        if (p.position().z() > playerZ - _oneVisibleRadius &&
            p.position().z() < playerZ + _oneVisibleRadius)
            return true;
    }
    return false;
}

bool PowerupSpawner::trySpawn(float playerZ,
                              const std::vector<Obstacle>& obstacles) {
    if (!_rules || !_mesh)
        return false;

    if (alreadyVisible(playerZ))
        return false;

    int lanes[3] = { 0, 1, 2 };
    std::uniform_int_distribution<int> shuffle(0, 2);
    for (int i = 2; i > 0; --i) {
        int j   = shuffle(rng()) % (i + 1);
        int tmp = lanes[i];
        lanes[i] = lanes[j];
        lanes[j] = tmp;
    }

    for (int i = 0; i < 3; ++i) {
        int lane = lanes[i];
        if (!laneClear(lane, _nextSpawnZ, obstacles))
            continue;

        float laneX = static_cast<float>(lane - 1) * _rules->laneWidth();

        _pickups.emplace_back(*_mesh,
                              Vec3(laneX, 0.6f, _nextSpawnZ),
                              Vec3(0.5f, 0.5f, 0.5f),
                              pickType());

        float jitter = uniformFloat(-_rules->powerupJitter(),
                                     _rules->powerupJitter());
        _nextSpawnZ -= _rules->powerupInterval() + jitter;
        return true;
    }

    _nextSpawnZ -= 4.0f;
    return false;
}

void PowerupSpawner::update(float playerZ, float dt,
                            const std::vector<Obstacle>& obstacles) {
    if (!_mesh || !_rules)
        return;

    while (_nextSpawnZ > playerZ - _spawnAhead) {
        if (!trySpawn(playerZ, obstacles))
            break;
    }

    float cutoff = playerZ + _pruneBehind;
    auto  it     = _pickups.begin();
    while (it != _pickups.end()) {
        if (it->position().z() > cutoff)
            it = _pickups.erase(it);
        else
            ++it;
    }

    for (Pickup& p : _pickups)
        p.update(dt);
}

const std::vector<Pickup>& PowerupSpawner::pickups() const {
    return _pickups;
}

void PowerupSpawner::removePickup(std::size_t index) {
    if (index < _pickups.size())
        _pickups.erase(_pickups.begin() + static_cast<std::ptrdiff_t>(index));
}