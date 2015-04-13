#include <iostream>
#include <fstream>
#include <stdexcept>
#include "Tokenizer.hpp"
#include <vector>

class Filter {
public:
    Filter(std::string path);
    ~Filter();

    int size();
    float * getData();
    float operator[](int i) const;
    float & operator[](int i);

private:
    int loadSize();
    void loadFilterData();

    int filterSize;
    float *data;
    PACC::Tokenizer *token;

};

inline float &Filter::operator[](int i) {return this->data[i];}
inline float Filter::operator[](int i) const {return this->data[i];}
inline float* Filter::getData() { return data;}
