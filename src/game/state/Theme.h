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
        Vec3        _panelFill   = Vec3(0.12f, 0.30f, 0.18f);
        Vec3        _panelBorder = Vec3(0.0f, 1.0f, 0.24f);
        float       _panelAlpha     = 0.72f;
        float       _panelCornerCut = 20.0f;
        std::string _fontName;
        std::string _backgroundImage;
        int         _fontSize        = 32;
        int         _titleSize       = 56;
        float       _menuItemSpacing = 12.0f;
        float       _menuItemPadding = 24.0f;

    public:
        static Theme load(const std::string& path);

        const Vec3& background()  const;
        const Vec3& foreground()  const;
        const Vec3& accent()      const;
        const Vec3& muted()       const;
        const Vec3& danger()      const;
        const Vec3& panelFill()   const;
        const Vec3& panelBorder() const;
        float       panelAlpha()     const;
        float       panelCornerCut() const;

        const std::string& fontName()        const;
        const std::string& backgroundImage() const;
        int                fontSize()        const;
        int                titleSize()       const;
        float              menuItemSpacing() const;
        float              menuItemPadding() const;

        std::string resolveFontPath()       const;
        std::string resolveBackgroundPath() const;
};

#endif