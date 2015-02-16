#include "Filter.h"

Filter::Filter(std::string path) {
    std::ifstream lConfig;
    lConfig.open(path);
    if (!lConfig.is_open()) {
        throw std::runtime_error("Le fichier du Filtre entré est invalide");
    }

    this->token = new PACC::Tokenizer(lConfig);
    this->token->setDelimiters(" \n","");
    this->data = nullptr;
    this->filterSize = loadSize();
    this->loadFilterData();

}

Filter::~Filter(){
    delete this->token;
    delete[] this->data;
}

int Filter::loadSize() {
    std::string lToken;
    this->token->getNextToken(lToken);
    int size = atoi(lToken.c_str());
    return size;
}

void Filter::loadFilterData() {
    std::string lToken;
    if(this->data != nullptr)
        delete[] this->data;
    this->data = new double[this->filterSize * this->filterSize];
    for (unsigned int i = 0; i < this->filterSize; ++i) {
        for (unsigned int j = 0; j < this->filterSize; ++j) {
            this->token->getNextToken(lToken);
            this->data[i*this->filterSize+j] = atof(lToken.c_str());
        }
    }
}

int Filter::size() {
    return this->filterSize;
}
