#include "game/Obstacle.h"

Obstacle::Obstacle(Mesh& mesh, const Vec3& position,
                   const Vec3& scale, ObstacleType type)
    : Entity(mesh, position, Vec3(0.0f), scale), _type(type) {}

ObstacleType Obstacle::type() const { return _type; }

float Obstacle::heightFor(ObstacleType type) {
    if (type == ObstacleType::Full) return _fullHeight;
    if (type == ObstacleType::Semi) return _semiHeight;
    return _topHeight;
}