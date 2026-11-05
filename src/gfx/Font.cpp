#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "gfx/Font.h"

#include <fstream>
#include <stdexcept>
#include <vector>

Font::Font(const std::string& path, int pixelHeight)
    : _pixelHeight(pixelHeight)
{
    if (pixelHeight <= 0)
        throw std::runtime_error("Font: pixelHeight must be > 0");

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        throw std::runtime_error("Font: cannot open " + path);

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> fontData(static_cast<std::size_t>(fileSize));
    if (!file.read(reinterpret_cast<char*>(fontData.data()), fileSize))
        throw std::runtime_error("Font: cannot read " + path);

    int offset = stbtt_GetFontOffsetForIndex(fontData.data(), 0);
    if (offset < 0)
        throw std::runtime_error("Font: no font found in " + path);

    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, fontData.data(), offset))
        throw std::runtime_error("Font: stbtt_InitFont failed for " + path);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

    float scale = stbtt_ScaleForPixelHeight(&info, static_cast<float>(pixelHeight));
    _ascent     = ascent * scale;
    _descent    = -descent * scale;
    _lineGap    = lineGap * scale;
    _lineHeight = _ascent + _descent + _lineGap;

    std::vector<unsigned char> bitmap(
        static_cast<std::size_t>(_atlasSize * _atlasSize), 0);

    stbtt_pack_context pack;
    if (!stbtt_PackBegin(&pack, bitmap.data(), _atlasSize, _atlasSize, 0, 1, nullptr))
        throw std::runtime_error("Font: stbtt_PackBegin failed");

    std::vector<stbtt_packedchar> packed(_charCount);
    if (!stbtt_PackFontRange(&pack, fontData.data(), 0,
                             static_cast<float>(pixelHeight),
                             _firstChar, _charCount, packed.data())) {
        stbtt_PackEnd(&pack);
        throw std::runtime_error("Font: stbtt_PackFontRange failed for " + path);
    }
    stbtt_PackEnd(&pack);

    for (int i = 0; i < _charCount; ++i) {
        const stbtt_packedchar& src = packed[i];
        Glyph& dst = _glyphs[i];

        dst.xoff    = src.xoff;
        dst.yoff    = src.yoff;
        dst.xoff2   = src.xoff2;
        dst.yoff2   = src.yoff2;
        dst.advance = src.xadvance;

        dst.u0 = static_cast<float>(src.x0) / static_cast<float>(_atlasSize);
        dst.v0 = static_cast<float>(src.y0) / static_cast<float>(_atlasSize);
        dst.u1 = static_cast<float>(src.x1) / static_cast<float>(_atlasSize);
        dst.v1 = static_cast<float>(src.y1) / static_cast<float>(_atlasSize);
    }

    _atlas.setData(_atlasSize, _atlasSize,
                   GL_R8, GL_RED, GL_UNSIGNED_BYTE, bitmap.data());
}

const Font::Glyph& Font::glyph(char c) const {
    int idx = static_cast<int>(static_cast<unsigned char>(c)) - _firstChar;
    if (idx < 0 || idx >= _charCount)
        idx = static_cast<int>('?') - _firstChar;
    return _glyphs[idx];
}

const Texture& Font::atlas() const { return _atlas; }

int   Font::pixelHeight() const { return _pixelHeight; }
float Font::ascent()      const { return _ascent; }
float Font::descent()     const { return _descent; }
float Font::lineHeight()  const { return _lineHeight; }

float Font::measureText(const std::string& text) const {
    float width = 0.0f;
    for (char c : text)
        width += glyph(c).advance;
    return width;
}