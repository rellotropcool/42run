#ifndef APPLICATION_H
#define APPLICATION_H

#include "core/Input.h"
#include "core/Window.h"
#include "game/Entity.h"
#include "game/GameRules.h"
#include "game/Ground.h"
#include "game/ObstacleSpawner.h"
#include "game/Player.h"
#include "game/Stats.h"
#include "game/Theme.h"
#include "gfx/Camera.h"
#include "gfx/Font.h"
#include "gfx/Shader.h"
#include "gfx/Texture.h"
#include "math/Mat4.h"
#include "math/Vec3.h"
#include "ui/Menu.h"
#include "ui/UIRenderer.h"

#include <vector>

class Mesh;

class Application {
    private:
        Window              _window;
        Input               _input;
        Shader              _shader;
        Ground              _ground;
        Camera              _camera;
        GameRules           _rules;
        Theme               _theme;
        Stats               _stats;
        Font                _font;
        Font                _titleFont;
        Texture             _backgroundTexture;
        bool                _hasBackground = false;
        Player              _player;
        ObstacleSpawner     _obstacles;
        Menu                _menu;
        UIRenderer          _ui;
        std::vector<Entity> _entities;
        float               _elapsed = 0.0f;

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
        void resetWorld();
        void drawEntity(const Entity& entity, const Mat4& viewProj);
        bool collides(const Player& player, const Obstacle& obs) const;
};

#endif