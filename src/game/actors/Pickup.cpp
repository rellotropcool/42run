#include "game/actors/Pickup.h"

Pickup::Pickup(Mesh& mesh, const Vec3& position,
               const Vec3& scale, PickupType type)
    : Entity(mesh, position, Vec3(0.0f), scale), _type(type) {}

PickupType Pickup::type() const { return _type; }

void Pickup::update(float dt) {
    _spin += dt * 3.0f;
    setRotation(Vec3(0.0f, _spin, 0.0f));
}