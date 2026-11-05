#include "ui/Menu.h"

#include "core/Input.h"
#include "game/Stats.h"
#include "game/Theme.h"
#include "gfx/Font.h"
#include "ui/UIRenderer.h"

#include <GLFW/glfw3.h>

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
    _elapsed  = 0.0f;
}

int Menu::selected() const { return _selected; }

Menu::Action Menu::update(float dt, const Input& input) {
    _elapsed += dt;

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

void Menu::render(UIRenderer& ui, const Theme& theme, const Font& font,
                  float screenW, float screenH,
                  int distance, const Stats& stats) const {
    float cx = screenW * 0.5f;

    ui.drawTextCentered(titleFor(_state), cx, 100.0f, theme.accent());

    float itemY = 320.0f;
    float lineH = font.lineHeight() + theme.menuItemSpacing();

    if (_state == State::GameOver) {
        ui.drawTextCentered("Distance: " + std::to_string(distance),
                            cx, 220.0f, theme.muted());
        ui.drawTextCentered("Best: " + std::to_string(stats.bestDistance()),
                            cx, 220.0f + font.lineHeight(), theme.muted());
    }

    if (_state == State::Stats) {
        ui.drawTextCentered("Games played: " + std::to_string(stats.gamesPlayed()),
                            cx, 220.0f, theme.foreground());
        ui.drawTextCentered("Best distance: " + std::to_string(stats.bestDistance()),
                            cx, 220.0f + font.lineHeight(), theme.foreground());
        ui.drawTextCentered("Total distance: " + std::to_string(stats.totalDistance()),
                            cx, 220.0f + font.lineHeight() * 2.0f, theme.foreground());
    }

    const std::vector<Item>& items = itemsFor(_state);

    for (std::size_t i = 0; i < items.size(); ++i) {
        Vec3 color = (static_cast<int>(i) == _selected)
                     ? theme.accent()
                     : theme.foreground();
        ui.drawTextCentered(items[i]._label, cx, itemY, color);
        itemY += lineH;
    }

    ui.drawTextCentered("UP/DOWN to select  -  ENTER to confirm",
                        cx, screenH - 60.0f, theme.muted());
}