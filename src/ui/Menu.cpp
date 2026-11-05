#include "ui/Menu.h"

#include "core/Input.h"
#include "game/Stats.h"
#include "game/Theme.h"
#include "ui/UIRenderer.h"

#include <GLFW/glfw3.h>

#include <algorithm>

Menu::Menu() {
    _mainItems = {
        { "Play",       Action::Play  },
        { "Statistics", Action::Stats },
        { "Quit",       Action::Quit  },
    };
    _gameOverItems = {
        { "Retry",      Action::Play  },
        { "Statistics", Action::Stats },
        { "Main Menu",  Action::Back  },
    };
    _statsItems = {
        { "Back",       Action::Back  },
    };
}

Menu::State Menu::state() const { return _state; }

void Menu::setState(State state) {
    _state    = state;
    _selected = 0;
}

int Menu::selected() const { return _selected; }

Menu::Action Menu::update(const Input& input) {
    if (_state == State::Playing)
        return Action::None;

    const std::vector<Item>& items = itemsFor(_state);
    if (items.empty())
        return Action::None;

    int n = static_cast<int>(items.size());

    if (input.wasPressed(GLFW_KEY_UP) || input.wasPressed(GLFW_KEY_W))
        _selected = (_selected + n - 1) % n;

    if (input.wasPressed(GLFW_KEY_DOWN) || input.wasPressed(GLFW_KEY_S))
        _selected = (_selected + 1) % n;

    if (input.wasPressed(GLFW_KEY_ENTER) || input.wasPressed(GLFW_KEY_KP_ENTER))
        return items[_selected]._action;

    return Action::None;
}

const std::vector<Menu::Item>& Menu::itemsFor(State state) const {
    static const std::vector<Item> empty;

    switch (state) {
        case State::Main:     return _mainItems;
        case State::GameOver: return _gameOverItems;
        case State::Stats:    return _statsItems;
        case State::Playing:  return empty;
    }
    return empty;
}

std::string Menu::titleFor(State state) const {
    switch (state) {
        case State::Main:     return "42RUN";
        case State::Stats:    return "STATISTICS";
        case State::GameOver: return "GAME OVER";
        case State::Playing:  return "";
    }
    return "";
}

void Menu::render(UIRenderer& ui, const Theme& theme,
                  float screenW, float screenH,
                  int distance, const Stats& stats) const {
    float cx       = screenW * 0.5f;
    float padding  = theme.menuItemPadding();
    float spacing  = theme.menuItemSpacing();
    float titleH   = static_cast<float>(theme.titleSize());
    float fontSize = static_cast<float>(theme.fontSize());
    float infoLH   = fontSize * 1.4f;
    float itemLH   = fontSize * 1.4f;
    float gap      = spacing * 4.0f;

    std::string title = titleFor(_state);

    std::vector<std::string> infoTexts;
    if (_state == State::GameOver) {
        infoTexts.push_back("Distance: " + std::to_string(distance));
        infoTexts.push_back("Best: " + std::to_string(stats.bestDistance()));
    } else if (_state == State::Stats) {
        infoTexts.push_back("Games played: " + std::to_string(stats.gamesPlayed()));
        infoTexts.push_back("Best distance: " + std::to_string(stats.bestDistance()));
        infoTexts.push_back("Total distance: " + std::to_string(stats.totalDistance()));
    }

    const std::vector<Item>& items = itemsFor(_state);
    int infoCount = static_cast<int>(infoTexts.size());
    int itemCount = static_cast<int>(items.size());

    float contentH = titleH;
    if (infoCount > 0) contentH += gap + static_cast<float>(infoCount) * infoLH;
    if (itemCount > 0) contentH += gap + static_cast<float>(itemCount) * itemLH;

    float panelH = contentH + padding * 2.0f;

    float maxW = ui.measureTitle(title);
    for (const auto& t : infoTexts)
        maxW = std::max(maxW, ui.measureText(t));
    for (const auto& item : items)
        maxW = std::max(maxW, ui.measureText(item._label));

    float panelW = maxW + padding * 4.0f;
    float panelX = (screenW - panelW) * 0.5f;
    float panelY = (screenH - panelH) * 0.5f;

    ui.drawPanel(panelX, panelY, panelW, panelH,
                 theme.panelFill(), theme.panelAlpha(),
                 theme.panelBorder(), 2.0f,
                 theme.panelCornerCut());

    float cursorY = panelY + padding;

    ui.drawTitleCentered(title, cx, cursorY, theme.accent());
    cursorY += titleH + gap;

    for (const auto& t : infoTexts) {
        Vec3 color = (_state == State::GameOver) ? theme.muted() : theme.foreground();
        ui.drawTextCentered(t, cx, cursorY, color);
        cursorY += infoLH;
    }
    if (infoCount > 0) cursorY += gap;

    for (int i = 0; i < itemCount; ++i) {
        Vec3 color = (i == _selected) ? theme.accent() : theme.foreground();
        ui.drawTextCentered(items[i]._label, cx, cursorY, color);
        cursorY += itemLH;
    }

    if (ui.hintVisible())
        ui.drawTextCentered("UP/DOWN to select  -  ENTER to confirm",
                            cx, screenH - padding * 2.5f, theme.muted());
}