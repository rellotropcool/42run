#ifndef GAMERULES_H
#define GAMERULES_H

#include <string>

class GameRules {
    private:
        float _forwardSpeed        = 8.0f;
        float _jumpVelocity        = 7.0f;
        float _gravity             = 20.0f;
        float _laneLerpSpeed       = 12.0f;
        float _laneWidth           = 2.0f;
        float _slideDuration       = 0.5f;
        float _warmupDistance      = 30.0f;
        float _slotSpacing         = 4.0f;
        float _coinInterval        = 2.5f;
        float _powerupInterval     = 250.0f;
        float _powerupJitter       = 20.0f;
        float _powerupBaseDuration = 10.0f;
        float _powerupUpgradeStep  = 0.25f;
        int   _upgradeBaseCost     = 50;
        int   _upgradeCostStep     = 25;
        int   _upgradeMaxLevel     = 5;

    public:
        static GameRules load(const std::string& path);

        float forwardSpeed()        const;
        float jumpVelocity()        const;
        float gravity()             const;
        float laneLerpSpeed()       const;
        float laneWidth()           const;
        float slideDuration()       const;
        float warmupDistance()      const;
        float slotSpacing()         const;
        float coinInterval()        const;
        float powerupInterval()     const;
        float powerupJitter()       const;
        float powerupBaseDuration() const;
        float powerupUpgradeStep()  const;
        int   upgradeBaseCost()     const;
        int   upgradeCostStep()     const;
        int   upgradeMaxLevel()     const;
};

#endif