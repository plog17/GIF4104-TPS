#include "PngImage.h"

PngImage::PngImage(const char* filename){
    unsigned int lError = 0;
    this->imageData = new std::vector<unsigned char>;
    lError = lodepng::decode(*this->imageData, this->width, this->height, filename);

    if(lError)
        throw std::runtime_error(lodepng_error_text(lError));
}

PngImage::PngImage(std::vector<unsigned char> data, unsigned int width, unsigned int height):
        width(width), height(height) {
    this->imageData = new std::vector<unsigned char>(data);
}

PngImage::~PngImage(){
    delete this->imageData;
}

void PngImage::writeToDisk(const char* filename){
    unsigned lError = lodepng::encode(filename, *this->imageData, this->width, this->height);

    if(lError)
        throw std::runtime_error(lodepng_error_text(lError));
}