#include "game/state/Theme.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {

std::string trim(const std::string& s) {
    auto a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    auto b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

void stripComments(std::string& line) {
    auto firstNonSpace = line.find_first_not_of(" \t");
    if (firstNonSpace != std::string::npos && line[firstNonSpace] == '#') {
        line.clear();
        return;
    }

    auto slash = line.find("//");
    if (slash != std::string::npos)
        line.erase(slash);

    size_t pos = 0;
    while ((pos = line.find('#', pos)) != std::string::npos) {
        bool isHexColor = (pos + 6 < line.size());
        if (isHexColor) {
            for (size_t i = 1; i <= 6; ++i) {
                char c = line[pos + i];
                if (!((c >= '0' && c <= '9') ||
                      (c >= 'a' && c <= 'f') ||
                      (c >= 'A' && c <= 'F'))) {
                    isHexColor = false;
                    break;
                }
            }
        }
        if (isHexColor) {
            pos += 7;
        } else {
            line.erase(pos);
            break;
        }
    }
}

int hexDigit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

Vec3 parseHexColor(const std::string& s, const std::string& key, int lineNum) {
    std::string h = s;
    if (!h.empty() && h[0] == '#')
        h.erase(0, 1);

    if (h.size() != 6)
        throw std::runtime_error("Theme: color '" + key +
                                 "' must be #RRGGBB on line " +
                                 std::to_string(lineNum) + ", got '" + s + "'");

    int channels[3] = { 0, 0, 0 };
    for (int i = 0; i < 3; ++i) {
        int hi = hexDigit(h[i * 2]);
        int lo = hexDigit(h[i * 2 + 1]);
        if (hi < 0 || lo < 0)
            throw std::runtime_error("Theme: invalid hex digit in '" + key +
                                     "' on line " + std::to_string(lineNum));
        channels[i] = hi * 16 + lo;
    }

    return Vec3(channels[0] / 255.0f,
                channels[1] / 255.0f,
                channels[2] / 255.0f);
}

int parseInt(const std::string& s, const std::string& key, int lineNum) {
    try {
        return std::stoi(s);
    } catch (const std::exception&) {
        throw std::runtime_error("Theme: invalid integer for '" + key +
                                 "' on line " + std::to_string(lineNum) +
                                 ", got '" + s + "'");
    }
}

float parseFloat(const std::string& s, const std::string& key, int lineNum) {
    try {
        return std::stof(s);
    } catch (const std::exception&) {
        throw std::runtime_error("Theme: invalid float for '" + key +
                                 "' on line " + std::to_string(lineNum) +
                                 ", got '" + s + "'");
    }
}

}

Theme Theme::load(const std::string& path) {
    Theme theme;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Theme: " << path << " not found, using defaults\n";
        return theme;
    }

    std::string line;
    int         lineNum = 0;

    while (std::getline(file, line)) {
        ++lineNum;

        stripComments(line);
        line = trim(line);
        if (line.empty())
            continue;

        auto eq = line.find('=');
        if (eq == std::string::npos)
            throw std::runtime_error("Theme: missing '=' on line " +
                                     std::to_string(lineNum) + ": '" + line + "'");

        std::string key      = trim(line.substr(0, eq));
        std::string valueStr = trim(line.substr(eq + 1));

        if (key.empty())
            throw std::runtime_error("Theme: empty key on line " +
                                     std::to_string(lineNum));
        if (valueStr.empty())
            throw std::runtime_error("Theme: empty value for '" + key +
                                     "' on line " + std::to_string(lineNum));

        if      (key == "background")      theme._background      = parseHexColor(valueStr, key, lineNum);
        else if (key == "foreground")      theme._foreground      = parseHexColor(valueStr, key, lineNum);
        else if (key == "accent")          theme._accent          = parseHexColor(valueStr, key, lineNum);
        else if (key == "muted")           theme._muted           = parseHexColor(valueStr, key, lineNum);
        else if (key == "danger")          theme._danger          = parseHexColor(valueStr, key, lineNum);
        else if (key == "panelFill")       theme._panelFill       = parseHexColor(valueStr, key, lineNum);
        else if (key == "panelBorder")     theme._panelBorder     = parseHexColor(valueStr, key, lineNum);
        else if (key == "panelAlpha")      theme._panelAlpha      = parseFloat(valueStr, key, lineNum);
        else if (key == "panelCornerCut")  theme._panelCornerCut  = parseFloat(valueStr, key, lineNum);
        else if (key == "backgroundImage") theme._backgroundImage = valueStr;
        else if (key == "fontName")        theme._fontName        = valueStr;
        else if (key == "fontSize")        theme._fontSize        = parseInt(valueStr, key, lineNum);
        else if (key == "titleSize")       theme._titleSize       = parseInt(valueStr, key, lineNum);
        else if (key == "menuItemSpacing") theme._menuItemSpacing = parseFloat(valueStr, key, lineNum);
        else if (key == "menuItemPadding") theme._menuItemPadding = parseFloat(valueStr, key, lineNum);
        else
            throw std::runtime_error("Theme: unknown key '" + key +
                                     "' on line " + std::to_string(lineNum));
    }

    return theme;
}

const Vec3& Theme::background() const { return _background; }
const Vec3& Theme::foreground() const { return _foreground; }
const Vec3& Theme::accent()     const { return _accent; }
const Vec3& Theme::muted()      const { return _muted; }
const Vec3& Theme::danger()     const { return _danger; }
const Vec3& Theme::panelFill()   const { return _panelFill; }
const Vec3& Theme::panelBorder() const { return _panelBorder; }
float       Theme::panelAlpha()  const { return _panelAlpha; }
float       Theme::panelCornerCut() const { return _panelCornerCut; }

const std::string& Theme::fontName()        const { return _fontName; }
const std::string& Theme::backgroundImage() const { return _backgroundImage; }
int                Theme::fontSize()        const { return _fontSize; }
int                Theme::titleSize()       const { return _titleSize; }
float              Theme::menuItemSpacing() const { return _menuItemSpacing; }
float              Theme::menuItemPadding() const { return _menuItemPadding; }

std::string Theme::resolveFontPath() const {
    if (_fontName.empty())
        throw std::runtime_error("Theme: fontName is empty");

    const std::string base = "assets/fonts/" + _fontName;

    for (const char* ext : { ".otf", ".ttf" }) {
        std::string candidate = base + ext;
        std::ifstream probe(candidate);
        if (probe.good())
            return candidate;
    }

    throw std::runtime_error("Theme: font '" + _fontName +
                             "' not found in assets/fonts/ (tried .otf and .ttf)");
}

std::string Theme::resolveBackgroundPath() const {
    if (_backgroundImage.empty())
        return "";

    std::string path = "assets/" + _backgroundImage;
    std::ifstream probe(path);
    if (!probe.good())
        throw std::runtime_error("Theme: background '" + _backgroundImage +
                                 "' not found (expected at " + path + ")");
    return path;
}