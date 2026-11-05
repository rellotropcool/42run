#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>

struct ImageData {
    int                        width  = 0;
    int                        height = 0;
    std::vector<unsigned char> pixels;
};

class Image {
    public:
        static ImageData load(const std::string& path);
};

#endif