#include <iostream>
#include <stdexcept>
#include "lodepng.h"
#include <vector>

class PngImage {
public:
    PngImage(const char* filename);
    PngImage(std::vector<unsigned char> data, unsigned int width, unsigned int height);
    ~PngImage();

    unsigned int getWidth() const;
    unsigned int getHeight() const;
    std::vector<unsigned char>* getData() const;

    void writeToDisk(const char* filename);

private:
    unsigned int width;
    unsigned int height;
    std::vector<unsigned char>  *imageData;
};

inline unsigned int PngImage::getHeight() const{
    return this->height;
}

inline unsigned int PngImage::getWidth() const{
    return this->width;
}

inline std::vector<unsigned char>* PngImage::getData() const{
    return this->imageData;
}