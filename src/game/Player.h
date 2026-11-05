#ifndef PLAYER_H
#define PLAYER_H

#include "game/Entity.h"

class GameRules;
class Input;

class Player : public Entity {
    private:
        static constexpr int _laneCount = 3;

        const GameRules* _rules      = nullptr;
        int              _lane       = 1;
        float            _vy         = 0.0f;
        float            _jumpOffset = 0.0f;
        bool             _onGround   = true;
        bool             _sliding    = false;
        float            _slideTimer = 0.0f;

    public:
        Player() = default;

        void setRules(const GameRules& rules);
        void update(float dt, const Input& input);
        void reset();

        int   lane()     const;
        bool  onGround() const;
        bool  sliding()  const;
        float distance() const;
};

#endif