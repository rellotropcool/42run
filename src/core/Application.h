#ifndef APPLICATION_H
#define APPLICATION_H

#include "core/Audio.h"
#include "core/Input.h"
#include "core/Window.h"
#include "game/actors/Entity.h"
#include "game/actors/Player.h"
#include "game/spawn/CoinSpawner.h"
#include "game/spawn/ObstacleSpawner.h"
#include "game/spawn/PowerupSpawner.h"
#include "game/state/GameRules.h"
#include "game/state/Stats.h"
#include "game/state/Theme.h"
#include "game/state/UserProfile.h"
#include "game/world/Ground.h"
#include "gfx/Camera.h"
#include "gfx/Font.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "math/Mat4.h"
#include "math/Vec3.h"
#include "ui/Menu.h"
#include "ui/Shop.h"
#include "ui/UIRenderer.h"

#include <vector>

class Mesh;

class Application {
    private:
        Window              _window;
        Audio               _audio;
        Input               _input;
        Shader              _shader;
        Ground              _ground;
        Camera              _camera;
        GameRules           _rules;
        Theme               _theme;
        Stats               _stats;
        UserProfile         _userProfile;
        Font                _font;
        Font                _titleFont;
        Texture             _backgroundTexture;
        bool                _hasBackground   = false;
        Player              _player;
        ObstacleSpawner     _obstacles;
        CoinSpawner         _coins;
        PowerupSpawner      _powerups;
        Menu                _menu;
        Shop                _shop;
        UIRenderer          _ui;
        std::vector<Entity> _entities;
        float               _elapsed         = 0.0f;
        bool                _newRecord       = false;
        float               _resumeCountdown = 0.0f;

    public:
        Application(const GameRules& rules, const Theme& theme);
        ~Application() = default;

        Application(const Application&)            = delete;
        Application& operator=(const Application&) = delete;

        void setPlayerMesh(Mesh& mesh);
        void setObstacleMesh(Mesh& mesh, const char* chunksPath);
        void addEntity(Mesh& mesh, const Vec3& position,
                       const Vec3& rotation = Vec3(0.0f),
                       const Vec3& scale    = Vec3(1.0f));
        int  run();

    private:
        void update(float dt);
        void render();
        void drawCountdown(float screenW, float screenH);
        void resetWorld();
        void drawEntity(const Entity& entity, const Mat4& viewProj);
        bool collides(const Player& player, const Obstacle& obs) const;
};

#endif