#include "game/GameRules.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {

std::string trim(const std::string& s) {
    auto a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    auto b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

float parseFloat(const std::string& s, const std::string& key, int lineNum) {
    try {
        return std::stof(s);
    } catch (const std::exception&) {
        throw std::runtime_error("GameRules: invalid float for '" + key +
                                 "' on line " + std::to_string(lineNum) +
                                 ", got '" + s + "'");
    }
}

}

GameRules GameRules::load(const std::string& path) {
    GameRules rules;

    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("GameRules: cannot open " + path);

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

        auto eq = line.find('=');
        if (eq == std::string::npos)
            throw std::runtime_error("GameRules: missing '=' on line " +
                                     std::to_string(lineNum) + ": '" + line + "'");

        std::string key      = trim(line.substr(0, eq));
        std::string valueStr = trim(line.substr(eq + 1));

        if (key.empty())
            throw std::runtime_error("GameRules: empty key on line " +
                                     std::to_string(lineNum));
        if (valueStr.empty())
            throw std::runtime_error("GameRules: empty value for '" + key +
                                     "' on line " + std::to_string(lineNum));

        float value = parseFloat(valueStr, key, lineNum);

        if      (key == "forwardSpeed")   rules._forwardSpeed   = value;
        else if (key == "jumpVelocity")   rules._jumpVelocity   = value;
        else if (key == "gravity")        rules._gravity        = value;
        else if (key == "laneLerpSpeed")  rules._laneLerpSpeed  = value;
        else if (key == "laneWidth")      rules._laneWidth      = value;
        else if (key == "slideDuration")  rules._slideDuration  = value;
        else if (key == "warmupDistance") rules._warmupDistance = value;
        else if (key == "slotSpacing")    rules._slotSpacing    = value;
        else
            throw std::runtime_error("GameRules: unknown key '" + key +
                                     "' on line " + std::to_string(lineNum));
    }

    return rules;
}

float GameRules::forwardSpeed()   const { return _forwardSpeed; }
float GameRules::jumpVelocity()   const { return _jumpVelocity; }
float GameRules::gravity()        const { return _gravity; }
float GameRules::laneLerpSpeed()  const { return _laneLerpSpeed; }
float GameRules::laneWidth()      const { return _laneWidth; }
float GameRules::slideDuration()  const { return _slideDuration; }
float GameRules::warmupDistance() const { return _warmupDistance; }
float GameRules::slotSpacing()    const { return _slotSpacing; }