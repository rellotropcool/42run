#ifndef POWERUPSPAWNER_H
#define POWERUPSPAWNER_H

#include "game/actors/Obstacle.h"
#include "game/actors/Pickup.h"

#include <vector>

class GameRules;
class Mesh;

class PowerupSpawner {
    private:
        static constexpr float _spawnAhead       = 60.0f;
        static constexpr float _pruneBehind      = 10.0f;
        static constexpr float _oneVisibleRadius = 40.0f;

        const GameRules*    _rules = nullptr;
        Mesh*               _mesh  = nullptr;
        std::vector<Pickup> _pickups;
        float               _nextSpawnZ = 0.0f;

    public:
        PowerupSpawner() = default;

        void load(Mesh& mesh, const GameRules& rules);
        void update(float playerZ, float dt,
                    const std::vector<Obstacle>& obstacles);
        void reset();

        const std::vector<Pickup>& pickups() const;
        void removePickup(std::size_t index);

    private:
        PickupType pickType() const;
        bool       trySpawn(float playerZ,
                            const std::vector<Obstacle>& obstacles);
        bool       laneClear(int lane, float z,
                             const std::vector<Obstacle>& obstacles) const;
        bool       alreadyVisible(float playerZ) const;
};

#endif