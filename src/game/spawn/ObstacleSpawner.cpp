#include "game/spawn/ObstacleSpawner.h"

#include "game/state/GameRules.h"
#include "gfx/Mesh.h"
#include "math/Vec3.h"

#include <cassert>
#include <fstream>
#include <random>
#include <sstream>
#include <stdexcept>

namespace {

std::mt19937& rng() {
    static std::mt19937 engine(std::random_device{}());
    return engine;
}

std::string trim(const std::string& s) {
    auto a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    auto b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

int parseInt(const std::string& s, const std::string& what, int lineNum) {
    try {
        return std::stoi(s);
    } catch (const std::exception&) {
        throw std::runtime_error("ObstacleSpawner: invalid " + what +
                                 " on line " + std::to_string(lineNum) +
                                 ", got '" + s + "'");
    }
}

bool parseObstacleType(const std::string& s, ObstacleType& out) {
    if (s == "f") { out = ObstacleType::Full;  return true; }
    if (s == "s") { out = ObstacleType::Semi;  return true; }
    if (s == "t") { out = ObstacleType::Top;   return true; }
    return false;
}

}

void ObstacleSpawner::load(Mesh& mesh, const std::string& chunksPath,
                           const GameRules& rules) {
    _mesh    = &mesh;
    _rules   = &rules;
    _chunks.clear();
    _obstacles.clear();
    _cursorZ = -_rules->warmupDistance();

    std::ifstream file(chunksPath);
    if (!file.is_open())
        throw std::runtime_error("ObstacleSpawner: cannot open " + chunksPath);

    std::string line;
    int         lineNum = 0;

    while (std::getline(file, line)) {
        ++lineNum;

        auto hash = line.find('#');
        if (hash != std::string::npos)
            line.erase(hash);

        line = trim(line);
        if (line.empty())
            continue;

        std::vector<std::string> tokens;
        std::istringstream       ss(line);
        std::string              token;
        while (ss >> token)
            tokens.push_back(token);

        bool hasComma = false;
        for (const auto& t : tokens)
            if (t.find(',') != std::string::npos) {
                hasComma = true;
                break;
            }

        Chunk chunk;

        if (!hasComma) {
            if (tokens.size() != 1)
                throw std::runtime_error(
                    "ObstacleSpawner: breather must be a single number on line " +
                    std::to_string(lineNum));

            int len = parseInt(tokens[0], "breather length", lineNum);
            if (len < 1)
                throw std::runtime_error(
                    "ObstacleSpawner: breather length must be >= 1 on line " +
                    std::to_string(lineNum));

            chunk.lengthInSlots = len;
        } else {
            int maxSlot = 0;

            for (const auto& t : tokens) {
                std::vector<std::string> fields;
                std::istringstream       ts(t);
                std::string              field;
                while (std::getline(ts, field, ','))
                    fields.push_back(trim(field));

                if (fields.size() != 3)
                    throw std::runtime_error(
                        "ObstacleSpawner: placement must be "
                        "'lane,type,zSlot' on line " +
                        std::to_string(lineNum) + ", got '" + t + "'");

                Placement p;
                p.lane  = parseInt(fields[0], "lane",  lineNum);
                p.zSlot = parseInt(fields[2], "zSlot", lineNum);

                ObstacleType obs;
                if (!parseObstacleType(fields[1], obs)) {
                    throw std::runtime_error(
                        "ObstacleSpawner: unknown type '" + fields[1] +
                        "' on line " + std::to_string(lineNum) +
                        " (valid: f, s, t)");
                }
                p.obsType = obs;

                if (p.lane < 0 || p.lane > 2)
                    throw std::runtime_error(
                        "ObstacleSpawner: lane must be 0..2 on line " +
                        std::to_string(lineNum));
                if (p.zSlot < 0)
                    throw std::runtime_error(
                        "ObstacleSpawner: zSlot must be >= 0 on line " +
                        std::to_string(lineNum));

                if (p.zSlot > maxSlot)
                    maxSlot = p.zSlot;

                chunk.placements.push_back(p);
            }

            chunk.lengthInSlots = maxSlot + 1;
        }

        _chunks.push_back(chunk);
    }

    if (_chunks.empty())
        throw std::runtime_error("ObstacleSpawner: no chunks loaded from " +
                                 chunksPath);
}

const ObstacleSpawner::Chunk& ObstacleSpawner::pickChunk() const {
    std::uniform_int_distribution<std::size_t> dist(0, _chunks.size() - 1);
    return _chunks[dist(rng())];
}

void ObstacleSpawner::update(float playerZ) {
    if (!_mesh || !_rules || _chunks.empty())
        return;

    while (_cursorZ > playerZ - _spawnAhead)
        spawnChunk(pickChunk());

    prune(playerZ);
}

void ObstacleSpawner::reset() {
    _obstacles.clear();
    if (_rules)
        _cursorZ = -_rules->warmupDistance();
}

void ObstacleSpawner::spawnChunk(const Chunk& chunk) {
    assert(_mesh && _rules && "ObstacleSpawner: not loaded");

    constexpr float groundY    = -0.5f;
    constexpr float topBottomY =  0.4f;

    for (const Placement& p : chunk.placements) {
        float laneX   = static_cast<float>(p.lane - 1) * _rules->laneWidth();
        float z       = _cursorZ - static_cast<float>(p.zSlot) * _rules->slotSpacing();
        float h       = Obstacle::heightFor(p.obsType);
        float bottomY = (p.obsType == ObstacleType::Top) ? topBottomY : groundY;
        float y       = bottomY + h * 0.5f;

        _obstacles.emplace_back(*_mesh, Vec3(laneX, y, z),
                                Vec3(0.8f, h, 0.8f), p.obsType);
    }

    _cursorZ -= static_cast<float>(chunk.lengthInSlots) * _rules->slotSpacing();
}

void ObstacleSpawner::prune(float playerZ) {
    float cutoff = playerZ + _pruneBehind;
    auto  it     = _obstacles.begin();

    while (it != _obstacles.end()) {
        if (it->position().z() > cutoff)
            it = _obstacles.erase(it);
        else
            ++it;
    }
}

const std::vector<Obstacle>& ObstacleSpawner::obstacles() const {
    return _obstacles;
}

void ObstacleSpawner::removeObstacle(std::size_t index) {
    if (index < _obstacles.size())
        _obstacles.erase(_obstacles.begin() + static_cast<std::ptrdiff_t>(index));
}