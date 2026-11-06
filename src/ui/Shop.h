#ifndef SHOP_H
#define SHOP_H

#include "game/actors/Pickup.h"

#include <string>
#include <vector>

class GameRules;
class Input;
class Theme;
class UIRenderer;
class UserProfile;

class Shop {
    public:
        enum class Action {
            None,
            Buy,
            Back,
        };

    private:
        struct Item {
            std::string _label;
            PickupType  _pickup;
            bool        _isBack;
        };

        std::vector<Item> _items;
        int               _selected = 0;

    public:
        Shop();

        Action update(const Input& input);

        void render(UIRenderer& ui, const Theme& theme,
                    const UserProfile& profile, const GameRules& rules,
                    float screenW, float screenH) const;

        PickupType selectedPickup() const;
        void       resetSelection();

    private:
        std::string labelFor(const Item& item, const UserProfile& profile,
                             const GameRules& rules) const;
};

#endif