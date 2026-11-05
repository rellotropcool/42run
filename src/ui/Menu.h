#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

class Input;
class Stats;
class Theme;
class UIRenderer;

class Menu {
    public:
        enum class State {
            Main,
            Playing,
            Stats,
            GameOver,
        };
        enum class Action {
            None,
            Play,
            Stats,
            Back,
            Quit,
        };

    private:
        struct Item {
            std::string _label;
            Action      _action;
        };

        State             _state    = State::Main;
        std::vector<Item> _mainItems;
        std::vector<Item> _gameOverItems;
        std::vector<Item> _statsItems;
        int               _selected = 0;

    public:
        Menu();

        State state() const;
        void  setState(State state);

        int   selected() const;

        Action update(const Input& input);

        void render(UIRenderer& ui, const Theme& theme,
                    float screenW, float screenH,
                    int distance, const Stats& stats) const;

    private:
        const std::vector<Item>& itemsFor(State state) const;
        std::string              titleFor(State state) const;
};

#endif