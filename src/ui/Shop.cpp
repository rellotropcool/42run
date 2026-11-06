#include "ui/Shop.h"

#include "core/Input.h"
#include "game/state/GameRules.h"
#include "game/state/Theme.h"
#include "game/state/UserProfile.h"
#include "ui/UIRenderer.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <utility>

Shop::Shop() {
    _items = {
        { "", PickupType::Magnet,      false },
        { "", PickupType::Shield,      false },
        { "", PickupType::DoubleCoins, false },
        { "", PickupType::None,        true  },
    };
}

void Shop::resetSelection() {
    _selected = 0;
}

PickupType Shop::selectedPickup() const {
    if (_selected < 0 || _selected >= static_cast<int>(_items.size()))
        return PickupType::None;
    return _items[_selected]._pickup;
}

Shop::Action Shop::update(const Input& input) {
    if (_items.empty())
        return Action::None;

    int n = static_cast<int>(_items.size());

    if (input.wasPressed(GLFW_KEY_UP) || input.wasPressed(GLFW_KEY_W))
        _selected = (_selected + n - 1) % n;

    if (input.wasPressed(GLFW_KEY_DOWN) || input.wasPressed(GLFW_KEY_S))
        _selected = (_selected + 1) % n;

    if (input.wasPressed(GLFW_KEY_ENTER) || input.wasPressed(GLFW_KEY_KP_ENTER)) {
        if (_items[_selected]._isBack)
            return Action::Back;
        return Action::Buy;
    }

    return Action::None;
}

std::string Shop::labelFor(const Item& item, const UserProfile& profile,
                            const GameRules& rules) const {
    if (item._isBack)
        return "Back";

    std::string name;
    if (item._pickup == PickupType::Magnet)
        name = "Magnet";
    else if (item._pickup == PickupType::Shield)
        name = "Shield";
    else
        name = "Double";

    int level    = profile.upgradeLevel(item._pickup);
    int maxLevel = rules.upgradeMaxLevel();

    if (level >= maxLevel)
        return name + " Lv " + std::to_string(level) + ": MAX";

    int cost = profile.upgradeCost(item._pickup,
                                    rules.upgradeBaseCost(),
                                    rules.upgradeCostStep());

    return name + " Lv " + std::to_string(level) + ": "
                 + std::to_string(cost) + "c";
}

void Shop::render(UIRenderer& ui, const Theme& theme,
                  const UserProfile& profile, const GameRules& rules,
                  float screenW, float screenH) const {
    float cx       = screenW * 0.5f;
    float padding  = theme.menuItemPadding();
    float spacing  = theme.menuItemSpacing();
    float titleH   = static_cast<float>(theme.titleSize());
    float fontSize = static_cast<float>(theme.fontSize());
    float infoLH   = fontSize * 1.4f;
    float itemLH   = fontSize * 1.4f;
    float gap      = spacing * 4.0f;

    std::string title = "SHOP";

    std::vector<std::pair<std::string, Vec3>> infoTexts;
    infoTexts.push_back({ "Coins: " + std::to_string(profile.coins()),
                          theme.accent() });

    std::vector<std::string> labels;
    for (const auto& item : _items)
        labels.push_back(labelFor(item, profile, rules));

    int infoCount = static_cast<int>(infoTexts.size());
    int itemCount = static_cast<int>(labels.size());

    float contentH = titleH;
    if (infoCount > 0) contentH += gap + static_cast<float>(infoCount) * infoLH;
    if (itemCount > 0) contentH += gap + static_cast<float>(itemCount) * itemLH;

    float panelH = contentH + padding * 2.0f;

    float maxW = ui.measureTitle(title);
    for (const auto& entry : infoTexts)
        maxW = std::max(maxW, ui.measureText(entry.first));
    for (const auto& lbl : labels)
        maxW = std::max(maxW, ui.measureText(lbl));

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
            float labelW = ui.measureText(labels[i]);
            float arrowW = ui.measureText(">");
            float arrowX = cx - labelW * 0.5f - barGap - arrowW;
            ui.drawText(">", arrowX, cursorY, theme.accent());
        }

        Vec3 color = selected ? theme.accent() : theme.foreground();

        if (!_items[i]._isBack &&
            profile.isMaxLevel(_items[i]._pickup, rules.upgradeMaxLevel()) &&
            !selected) {
            color = theme.muted();
        }

        ui.drawTextCentered(labels[i], cx, cursorY, color);
        cursorY += itemLH;
    }

    ui.drawHint("UP/DOWN to select  -  ENTER to confirm",
                cx, screenH - padding * 2.5f, theme.muted());
}