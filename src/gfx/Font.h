#ifndef FONT_H
#define FONT_H

#include "gfx/Texture.h"

#include <string>

class Font {
    public:
        struct Glyph {
            float u0 = 0.0f, v0 = 0.0f;
            float u1 = 0.0f, v1 = 0.0f;
            float xoff  = 0.0f, yoff  = 0.0f;
            float xoff2 = 0.0f, yoff2 = 0.0f;
            float advance = 0.0f;
        };

    private:
        static constexpr int _firstChar = 32;
        static constexpr int _lastChar  = 126;
        static constexpr int _charCount = _lastChar - _firstChar + 1;
        static constexpr int _atlasSize = 1024;

        Texture _atlas;
        int     _pixelHeight = 0;
        float   _ascent      = 0.0f;
        float   _descent     = 0.0f;
        float   _lineGap     = 0.0f;
        float   _lineHeight  = 0.0f;
        Glyph   _glyphs[_charCount];

    public:
        Font(const std::string& path, int pixelHeight);
        ~Font() = default;

        Font(const Font&)            = delete;
        Font& operator=(const Font&) = delete;

        const Glyph&   glyph(char c)    const;
        const Texture& atlas()          const;

        int   pixelHeight() const;
        float ascent()      const;
        float descent()     const;
        float lineHeight()  const;

        float measureText(const std::string& text) const;
};

#endif