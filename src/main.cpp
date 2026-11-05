#include "core/Application.h"

#include "game/GameRules.h"
#include "game/Theme.h"
#include "gfx/Mesh.h"
#include "gfx/ObjLoader.h"
#include "gfx/VertexArray.h"

#include <exception>
#include <iostream>

int main() {
    try {
        GameRules rules = GameRules::load("assets/config/gamerules.txt");
        Theme     theme = Theme::load("save/theme.txt");

        ObjLoader::Result obj = ObjLoader::load("assets/models/cube.obj");

        Application app(rules, theme);

        Mesh playerMesh(obj.vertices.data(), obj.vertices.size() * sizeof(Vertex),
                        obj.indices.data(),  obj.indices.size(),
                        makeVertexLayout());
        Mesh obstacleMesh(obj.vertices.data(), obj.vertices.size() * sizeof(Vertex),
                          obj.indices.data(),  obj.indices.size(),
                          makeVertexLayout());

        app.setPlayerMesh(playerMesh);
        app.setObstacleMesh(obstacleMesh, "assets/config/chunks.txt");
        return app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}