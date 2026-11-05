#ifndef OBSTACLESPAWNER_H
#define OBSTACLESPAWNER_H

#include "game/Obstacle.h"

#include <string>
#include <vector>

class GameRules;
class Mesh;

class ObstacleSpawner {
    private:
        struct Placement {
            int          lane;
            ObstacleType type;
            int          zSlot;
        };
        struct Chunk {
            std::vector<Placement> placements;
            int                    lengthInSlots;
        };

        static constexpr float _spawnAhead  = 80.0f;
        static constexpr float _pruneBehind = 10.0f;

        const GameRules*      _rules = nullptr;
        Mesh*                 _mesh  = nullptr;
        std::vector<Obstacle> _obstacles;
        std::vector<Chunk>    _chunks;
        float                 _cursorZ = 0.0f;

    public:
        ObstacleSpawner() = default;

        void load(Mesh& mesh, const std::string& chunksPath,
                  const GameRules& rules);
        void update(float playerZ);
        void reset();

        const std::vector<Obstacle>& obstacles() const;

    private:
        const Chunk& pickChunk() const;
        void         spawnChunk(const Chunk& chunk);
        void         prune(float playerZ);
};

#endif