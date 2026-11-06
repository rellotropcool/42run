#include "ui/Menu.h"

#include "core/Input.h"
#include "game/state/Stats.h"
#include "game/state/Theme.h"
#include "ui/UIRenderer.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <utility>

Menu::Menu() {
    _mainItems = {
        { "Play",       Action::Play     },
        { "Shop",       Action::OpenShop },
        { "Statistics", Action::Stats    },
        { "Quit",       Action::Quit     },
    };
    _gameOverItems = {
        { "Retry",      Action::Play     },
        { "Statistics", Action::Stats    },
        { "Main Menu",  Action::Back     },
    };
    _statsItems = {
        { "Back",       Action::Back     },
    };
    _pausedItems = {
        { "Resume",     Action::Resume   },
        { "Restart",    Action::Play     },
        { "Main Menu",  Action::Back     },
        { "Quit",       Action::Quit     },
    };
}

Menu::State Menu::state() const { return _state; }

void Menu::setState(State state) {
    _state    = state;
    _selected = 0;
}

int Menu::selected() const { return _selected; }

Menu::Action Menu::update(const Input& input) {
    if (_state == State::Playing || _state == State::Shop)
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
        case State::Paused:   return _pausedItems;
        case State::Playing:  return empty;
        case State::Shop:     return empty;
    }
    return empty;
}

std::string Menu::titleFor(State state) const {
    switch (state) {
        case State::Main:     return "42RUN";
        case State::Stats:    return "STATISTICS";
        case State::GameOver: return "GAME OVER";
        case State::Paused:   return "PAUSED";
        case State::Playing:  return "";
        case State::Shop:     return "";
    }
    return "";
}

void Menu::render(UIRenderer& ui, const Theme& theme,
                  float screenW, float screenH,
                  int distance, int coins,
                  const Stats& stats, bool newRecord) const {
    float cx       = screenW * 0.5f;
    float padding  = theme.menuItemPadding();
    float spacing  = theme.menuItemSpacing();
    float titleH   = static_cast<float>(theme.titleSize());
    float fontSize = static_cast<float>(theme.fontSize());
    float infoLH   = fontSize * 1.4f;
    float itemLH   = fontSize * 1.4f;
    float gap      = spacing * 4.0f;

    std::string title = titleFor(_state);

    std::vector<std::pair<std::string, Vec3>> infoTexts;
    if (_state == State::GameOver) {
        infoTexts.push_back({ "Distance: " + std::to_string(distance),
                              theme.muted() });
        infoTexts.push_back({ "Coins: " + std::to_string(coins),
                              theme.accent() });
        infoTexts.push_back({ "Best: " + std::to_string(stats.bestDistance()),
                              theme.muted() });
        if (newRecord)
            infoTexts.push_back({ "NEW RECORD!", theme.accent() });
    } else if (_state == State::Stats) {
        infoTexts.push_back({ "Games played: " + std::to_string(stats.gamesPlayed()),
                              theme.foreground() });
        infoTexts.push_back({ "Best distance: " + std::to_string(stats.bestDistance()),
                              theme.foreground() });
        infoTexts.push_back({ "Total distance: " + std::to_string(stats.totalDistance()),
                              theme.foreground() });
    } else if (_state == State::Paused) {
        infoTexts.push_back({ "Distance: " + std::to_string(distance),
                              theme.muted() });
        infoTexts.push_back({ "Coins: " + std::to_string(coins),
                              theme.accent() });
    }

    const std::vector<Item>& items = itemsFor(_state);
    int infoCount = static_cast<int>(infoTexts.size());
    int itemCount = static_cast<int>(items.size());

    float contentH = titleH;
    if (infoCount > 0) contentH += gap + static_cast<float>(infoCount) * infoLH;
    if (itemCount > 0) contentH += gap + static_cast<float>(itemCount) * itemLH;

    float panelH = contentH + padding * 2.0f;

    float maxW = ui.measureTitle(title);
    for (const auto& entry : infoTexts)
        maxW = std::max(maxW, ui.measureText(entry.first));
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

    ui.drawTitleCentered(title, cx, cursorY, theme.foreground());
    cursorY += titleH + gap;

    for (const auto& entry : infoTexts) {
        ui.drawTextCentered(entry.first, cx, cursorY, entry.second);
        cursorY += infoLH;
    }
    if (infoCount > 0) cursorY += gap;

    float barGap = spacing * 1.5f;

    for (int i = 0; i < itemCount; ++i) {
        bool selected = (i == _selected);

        if (selected) {
            float labelW = ui.measureText(items[i]._label);
            float arrowW = ui.measureText(">");
            float arrowX = cx - labelW * 0.5f - barGap - arrowW;
            ui.drawText(">", arrowX, cursorY, theme.accent());
        }

        Vec3 color = selected ? theme.accent() : theme.foreground();
        ui.drawTextCentered(items[i]._label, cx, cursorY, color);
        cursorY += itemLH;
    }

    ui.drawHint("UP/DOWN to select  -  ENTER to confirm",
                cx, screenH - padding * 2.5f, theme.muted());
}