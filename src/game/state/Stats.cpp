#include "game/state/Stats.h"

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

int parseInt(const std::string& s, const std::string& key, int lineNum) {
    try {
        return std::stoi(s);
    } catch (const std::exception&) {
        throw std::runtime_error("Stats: invalid integer for '" + key +
                                 "' on line " + std::to_string(lineNum) +
                                 ", got '" + s + "'");
    }
}

}

Stats Stats::load(const std::string& path) {
    Stats stats;

    std::ifstream file(path);
    if (!file.is_open())
        return stats;

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
            throw std::runtime_error("Stats: missing '=' on line " +
                                     std::to_string(lineNum) + ": '" + line + "'");

        std::string key      = trim(line.substr(0, eq));
        std::string valueStr = trim(line.substr(eq + 1));

        if (key.empty() || valueStr.empty())
            throw std::runtime_error("Stats: empty key or value on line " +
                                     std::to_string(lineNum));

        int value = parseInt(valueStr, key, lineNum);

        if      (key == "gamesPlayed")   stats._gamesPlayed   = value;
        else if (key == "bestDistance")  stats._bestDistance  = value;
        else if (key == "totalDistance") stats._totalDistance = value;
        else if (key == "totalCoins")    stats._totalCoins    = value;
        else
            throw std::runtime_error("Stats: unknown key '" + key +
                                     "' on line " + std::to_string(lineNum));
    }

    return stats;
}

void Stats::recordGame(int distance, int coins) {
    if (distance < 0) distance = 0;
    if (coins    < 0) coins    = 0;

    ++_gamesPlayed;
    _totalDistance += distance;
    _totalCoins    += coins;
    if (distance > _bestDistance)
        _bestDistance = distance;
}

void Stats::save(const std::string& path) const {
    std::ofstream file(path);
    if (!file.is_open())
        return;

    file << "gamesPlayed   = " << _gamesPlayed   << "\n";
    file << "bestDistance  = " << _bestDistance  << "\n";
    file << "totalDistance = " << _totalDistance << "\n";
    file << "totalCoins    = " << _totalCoins    << "\n";
}

int Stats::gamesPlayed()   const { return _gamesPlayed; }
int Stats::bestDistance()  const { return _bestDistance; }
int Stats::totalDistance() const { return _totalDistance; }
int Stats::totalCoins()    const { return _totalCoins; }