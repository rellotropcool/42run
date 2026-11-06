#ifndef PICKUP_H
#define PICKUP_H

#include "game/actors/Entity.h"

enum class PickupType {
    None,
    Coin,
    Magnet,
    Shield,
    DoubleCoins,
};

class Pickup : public Entity {
    private:
        PickupType _type = PickupType::Coin;
        float      _spin = 0.0f;

    public:
        Pickup() = default;
        Pickup(Mesh& mesh, const Vec3& position,
               const Vec3& scale, PickupType type);

        PickupType type() const;

        void update(float dt);
};

#endif