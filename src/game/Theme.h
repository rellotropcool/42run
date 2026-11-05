#ifndef THEME_H
#define THEME_H

#include "math/Vec3.h"

#include <string>

class Theme {
    private:
        Vec3        _background;
        Vec3        _foreground;
        Vec3        _accent;
        Vec3        _muted;
        Vec3        _danger;
        std::string _fontName;
        int         _fontSize        = 32;
        float       _menuItemSpacing = 12.0f;
        float       _menuItemPadding = 24.0f;

    public:
        static Theme load(const std::string& path);

        const Vec3& background() const;
        const Vec3& foreground() const;
        const Vec3& accent()     const;
        const Vec3& muted()      const;
        const Vec3& danger()     const;

        const std::string& fontName()        const;
        int                fontSize()        const;
        float              menuItemSpacing() const;
        float              menuItemPadding() const;

        std::string resolveFontPath() const;
};

#endif