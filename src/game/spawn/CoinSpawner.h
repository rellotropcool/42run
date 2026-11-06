#ifndef COINSPAWNER_H
#define COINSPAWNER_H

#include "game/actors/Obstacle.h"
#include "game/actors/Pickup.h"

#include <vector>

class GameRules;
class Mesh;

class CoinSpawner {
    private:
        static constexpr float _spawnAhead    = 60.0f;
        static constexpr float _pruneBehind   = 10.0f;
        static constexpr float _yLevel        = 0.6f;
        static constexpr int   _segmentLength = 5;

        const GameRules*    _rules = nullptr;
        Mesh*               _mesh  = nullptr;
        std::vector<Pickup> _coins;

        float _nextSpawnZ   = 0.0f;
        int   _currentLane  = 1;
        int   _targetLane   = 1;
        int   _straightLeft = _segmentLength;

    public:
        CoinSpawner() = default;

        void load(Mesh& mesh, const GameRules& rules);
        void update(float playerZ, float dt,
                    const std::vector<Obstacle>& obstacles);
        void reset();

        const std::vector<Pickup>& coins() const;
        void removeCoin(std::size_t index);

    private:
        bool advanceSnake();
        void chooseTargetLane();
        bool blockedAt(int lane, float z,
                       const std::vector<Obstacle>& obstacles) const;
};

#endif