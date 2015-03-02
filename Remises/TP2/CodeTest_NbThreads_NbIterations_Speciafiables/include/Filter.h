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
    double operator[](int i) const;
    double & operator[](int i);

private:
    int loadSize();
    void loadFilterData();

    int filterSize;
    double *data;
    PACC::Tokenizer *token;

};

inline double &Filter::operator[](int i) {return this->data[i];}
inline double Filter::operator[](int i) const {return this->data[i];}

