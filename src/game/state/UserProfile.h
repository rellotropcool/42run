#ifndef USERPROFILE_H
#define USERPROFILE_H

#include "game/actors/Pickup.h"

#include <string>

class UserProfile {
    private:
        int _coins              = 0;
        int _magnetUpgradeLevel = 0;
        int _shieldUpgradeLevel = 0;
        int _doubleUpgradeLevel = 0;

    public:
        static UserProfile load(const std::string& path);
        void save(const std::string& path) const;

        int  coins() const;
        void addCoins(int amount);

        int magnetUpgradeLevel() const;
        int shieldUpgradeLevel() const;
        int doubleUpgradeLevel() const;

        int  upgradeLevel(PickupType type) const;
        int  upgradeCost(PickupType type, int baseCost, int costStep) const;
        bool isMaxLevel(PickupType type, int maxLevel) const;

        float effectiveDuration(PickupType type,
                                float baseDuration, float upgradeStep) const;

        bool tryUpgrade(PickupType type, int cost, int maxLevel);
};

#endif