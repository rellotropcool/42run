#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "game/actors/Entity.h"

enum class ObstacleType {
    Full,
    Semi,
    Top,
};

class Obstacle : public Entity {
    private:
        static constexpr float _fullHeight = 1.8f;
        static constexpr float _semiHeight = 0.5f;
        static constexpr float _topHeight  = 1.6f;

        ObstacleType _type = ObstacleType::Full;

    public:
        Obstacle() = default;
        Obstacle(Mesh& mesh, const Vec3& position,
                 const Vec3& scale, ObstacleType type);

        ObstacleType type() const;

        static float heightFor(ObstacleType type);
};

#endif