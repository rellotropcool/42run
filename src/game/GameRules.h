#ifndef GAMERULES_H
#define GAMERULES_H

#include <string>

class GameRules {
    private:
        float _forwardSpeed   = 8.0f;
        float _jumpVelocity   = 7.0f;
        float _gravity        = 20.0f;
        float _laneLerpSpeed  = 12.0f;
        float _laneWidth      = 2.0f;
        float _slideDuration  = 0.5f;
        float _warmupDistance = 30.0f;
        float _slotSpacing    = 4.0f;

    public:
        static GameRules load(const std::string& path);

        float forwardSpeed()   const;
        float jumpVelocity()   const;
        float gravity()        const;
        float laneLerpSpeed()  const;
        float laneWidth()      const;
        float slideDuration()  const;
        float warmupDistance() const;
        float slotSpacing()    const;
};

#endif