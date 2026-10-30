#include "gfx/ObjLoader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

namespace {

struct FaceIndex {
    int vertex   = 0;
    int texCoord = 0;
    int normal   = 0;

    bool operator==(const FaceIndex& other) const {
        return vertex   == other.vertex
            && texCoord == other.texCoord
            && normal   == other.normal;
    }
};

struct FaceIndexHash {
    std::size_t operator()(const FaceIndex& f) const {
        std::size_t h = std::hash<int>{}(f.vertex);
        h ^= std::hash<int>{}(f.texCoord) + 0x9e3779b9u + (h << 6) + (h >> 2);
        h ^= std::hash<int>{}(f.normal)   + 0x9e3779b9u + (h << 6) + (h >> 2);
        return h;
    }
};

int parseInt(const std::string& token, const std::string& path) {
    try {
        return std::stoi(token);
    } catch (const std::exception&) {
        throw std::runtime_error("ObjLoader: invalid integer '" + token +
                                 "' in " + path);
    }
}

std::vector<std::string> splitSlashes(const std::string& token) {
    std::vector<std::string> parts;
    std::size_t start = 0;
    while (true) {
        std::size_t slash = token.find('/', start);
        if (slash == std::string::npos) {
            parts.push_back(token.substr(start));
            return parts;
        }
        parts.push_back(token.substr(start, slash - start));
        start = slash + 1;
    }
}

int resolveIndex(int raw, int count, const std::string& path) {
    if (raw == 0)
        return 0;
    if (raw > 0) {
        if (raw > count)
            throw std::runtime_error("ObjLoader: index out of range in " + path);
        return raw;
    }
    int resolved = count + raw + 1;
    if (resolved < 1)
        throw std::runtime_error("ObjLoader: negative index out of range in " + path);
    return resolved;
}

FaceIndex parseFaceToken(const std::string& token,
                         int vertexCount,
                         int texCoordCount,
                         int normalCount,
                         const std::string& path)
{
    std::vector<std::string> parts = splitSlashes(token);
    if (parts.empty() || parts.size() > 3 || parts[0].empty())
        throw std::runtime_error("ObjLoader: invalid face token '" + token +
                                 "' in " + path);

    FaceIndex index;
    index.vertex = resolveIndex(parseInt(parts[0], path), vertexCount, path);
    if (parts.size() >= 2 && !parts[1].empty())
        index.texCoord = resolveIndex(parseInt(parts[1], path), texCoordCount, path);
    if (parts.size() >= 3 && !parts[2].empty())
        index.normal   = resolveIndex(parseInt(parts[2], path), normalCount, path);
    return index;
}

}

ObjLoader::Result ObjLoader::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("ObjLoader: cannot open " + path);

    std::vector<Vec3> positions;
    std::vector<Vec2> texCoords;
    std::vector<Vec3> normals;

    Result result;
    std::unordered_map<FaceIndex, std::uint32_t, FaceIndexHash> cache;

    std::string line;
    while (std::getline(file, line)) {
        std::size_t comment = line.find('#');
        if (comment != std::string::npos)
            line.erase(comment);

        std::istringstream stream(line);
        std::string tag;
        if (!(stream >> tag))
            continue;

        if (tag == "v") {
            float x = 0.0f, y = 0.0f, z = 0.0f;
            if (!(stream >> x >> y >> z))
                throw std::runtime_error("ObjLoader: malformed vertex in " + path);
            positions.push_back(Vec3(x, y, z));
        } else if (tag == "vt") {
            float u = 0.0f, v = 0.0f;
            if (!(stream >> u >> v))
                throw std::runtime_error("ObjLoader: malformed texcoord in " + path);
            texCoords.push_back(Vec2(u, v));
        } else if (tag == "vn") {
            float x = 0.0f, y = 0.0f, z = 0.0f;
            if (!(stream >> x >> y >> z))
                throw std::runtime_error("ObjLoader: malformed normal in " + path);
            normals.push_back(Vec3(x, y, z));
        } else if (tag == "f") {
            std::vector<std::uint32_t> faceIndices;
            std::string token;
            while (stream >> token) {
                FaceIndex key = parseFaceToken(token,
                                               static_cast<int>(positions.size()),
                                               static_cast<int>(texCoords.size()),
                                               static_cast<int>(normals.size()),
                                               path);
                auto it = cache.find(key);
                if (it != cache.end()) {
                    faceIndices.push_back(it->second);
                    continue;
                }

                Vertex vertex;
                vertex.position = positions[key.vertex - 1];
                vertex.uv       = (key.texCoord != 0) ? texCoords[key.texCoord - 1]
                                                      : Vec2(0.0f, 0.0f);
                vertex.normal   = (key.normal   != 0) ? normals[key.normal - 1]
                                                      : Vec3(0.0f);

                std::uint32_t index = static_cast<std::uint32_t>(result.vertices.size());
                result.vertices.push_back(vertex);
                cache.emplace(key, index);
                faceIndices.push_back(index);
            }

            if (faceIndices.size() < 3)
                throw std::runtime_error("ObjLoader: face with fewer than 3 "
                                         "vertices in " + path);

            for (std::size_t i = 1; i + 1 < faceIndices.size(); ++i) {
                result.indices.push_back(faceIndices[0]);
                result.indices.push_back(faceIndices[i]);
                result.indices.push_back(faceIndices[i + 1]);
            }
        }
    }

    return result;
}