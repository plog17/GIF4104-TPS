#include <iostream>
#include <stdlib.h>
#include <fstream>

#include "Chrono.hpp"
#include "Tokenizer.hpp"
#include "lodepng.h"

class SequentialExecuter {
public:
  SequentialExecuter(char* inName);


private:
  void decode(const char* inFilename,  vector<unsigned char>& outImage, unsigned int& outWidth, unsigned int& outHeight);
  void encode(const char* inFilename, vector<unsigned char>& inImage, unsigned inWidth, unsigned inHeight);
  void runSequential();

  char* inFilename;
  vector<unsigned char>& inImage;

}
