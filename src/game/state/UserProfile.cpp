#include "game/state/UserProfile.h"

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
        throw std::runtime_error("UserProfile: invalid integer for '" + key +
                                 "' on line " + std::to_string(lineNum) +
                                 ", got '" + s + "'");
    }
}

}

UserProfile UserProfile::load(const std::string& path) {
    UserProfile profile;

    std::ifstream file(path);
    if (!file.is_open())
        return profile;

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
            throw std::runtime_error("UserProfile: missing '=' on line " +
                                     std::to_string(lineNum) + ": '" + line + "'");

        std::string key      = trim(line.substr(0, eq));
        std::string valueStr = trim(line.substr(eq + 1));

        if (key.empty() || valueStr.empty())
            throw std::runtime_error("UserProfile: empty key or value on line " +
                                     std::to_string(lineNum));

        int value = parseInt(valueStr, key, lineNum);

        if      (key == "coins")              profile._coins              = value;
        else if (key == "magnetUpgradeLevel") profile._magnetUpgradeLevel = value;
        else if (key == "shieldUpgradeLevel") profile._shieldUpgradeLevel = value;
        else if (key == "doubleUpgradeLevel") profile._doubleUpgradeLevel = value;
        else
            throw std::runtime_error("UserProfile: unknown key '" + key +
                                     "' on line " + std::to_string(lineNum));
    }

    return profile;
}

void UserProfile::save(const std::string& path) const {
    std::ofstream file(path);
    if (!file.is_open())
        return;

    file << "coins              = " << _coins              << "\n";
    file << "magnetUpgradeLevel = " << _magnetUpgradeLevel << "\n";
    file << "shieldUpgradeLevel = " << _shieldUpgradeLevel << "\n";
    file << "doubleUpgradeLevel = " << _doubleUpgradeLevel << "\n";
}

int  UserProfile::coins() const { return _coins; }

void UserProfile::addCoins(int amount) {
    if (amount > 0)
        _coins += amount;
}

int UserProfile::magnetUpgradeLevel() const { return _magnetUpgradeLevel; }
int UserProfile::shieldUpgradeLevel() const { return _shieldUpgradeLevel; }
int UserProfile::doubleUpgradeLevel() const { return _doubleUpgradeLevel; }

int UserProfile::upgradeLevel(PickupType type) const {
    if (type == PickupType::Magnet)      return _magnetUpgradeLevel;
    if (type == PickupType::Shield)      return _shieldUpgradeLevel;
    if (type == PickupType::DoubleCoins) return _doubleUpgradeLevel;
    return 0;
}

int UserProfile::upgradeCost(PickupType type, int baseCost, int costStep) const {
    return baseCost + upgradeLevel(type) * costStep;
}

bool UserProfile::isMaxLevel(PickupType type, int maxLevel) const {
    return upgradeLevel(type) >= maxLevel;
}

float UserProfile::effectiveDuration(PickupType type,
                                     float baseDuration,
                                     float upgradeStep) const {
    if (type == PickupType::None || type == PickupType::Coin)
        return 0.0f;

    int level = upgradeLevel(type);
    return baseDuration * (1.0f + static_cast<float>(level) * upgradeStep);
}

bool UserProfile::tryUpgrade(PickupType type, int cost, int maxLevel) {
    if (isMaxLevel(type, maxLevel))
        return false;
    if (_coins < cost)
        return false;

    _coins -= cost;

    if      (type == PickupType::Magnet)      ++_magnetUpgradeLevel;
    else if (type == PickupType::Shield)      ++_shieldUpgradeLevel;
    else if (type == PickupType::DoubleCoins) ++_doubleUpgradeLevel;
    else return false;

    return true;
}