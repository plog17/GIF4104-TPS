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

    std::vector<unsigned char>::iterator begin();
    std::vector<unsigned char>::const_iterator begin() const;
    std::vector<unsigned char>::iterator end();
    std::vector<unsigned char>::const_iterator end() const;

    unsigned char operator[](int i) const;

    unsigned char & operator[](int i);

private:

    unsigned int width;
    unsigned int height;
    std::vector<unsigned char>  *imageData;
};

inline unsigned char &PngImage::operator[](int i) {return this->imageData->operator[](i);}

inline unsigned char PngImage::operator[](int i) const {return this->imageData->operator[](i);}

inline std::vector<unsigned char>::iterator PngImage::begin() { return this->imageData->begin(); }

inline std::vector<unsigned char>::const_iterator PngImage::begin() const { return this->imageData->begin(); }

inline std::vector<unsigned char>::iterator PngImage::end() { return this->imageData->end(); }

inline std::vector<unsigned char>::const_iterator PngImage::end() const { return this->imageData->end(); }

inline unsigned int PngImage::getHeight() const{return this->height;}

inline unsigned int PngImage::getWidth() const{return this->width;}

inline std::vector<unsigned char>* PngImage::getData() const{return this->imageData;}
