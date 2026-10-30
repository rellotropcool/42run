#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "gfx/VertexArray.h"

#include <cstdint>
#include <string>
#include <vector>

class ObjLoader {
    public:
        struct Result {
            std::vector<Vertex>        vertices;
            std::vector<std::uint32_t> indices;
        };
        static Result load(const std::string& path);
};

#endif