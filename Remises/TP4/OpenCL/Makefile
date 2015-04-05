CC=g++ -std=c++11
SOURCES=src/lodepng.cpp src/seq_tp2.cpp src/PngImage.cpp src/Filter.cpp src/Tokenizer.cpp
LIBS=-lOpenCL
CFLAGS=-c -Wall
INCLUDES=-I include
EXECUTABLE=test


all:
	$(CC) $(SOURCES) $(INCLUDES) $(LIBS) -o $(EXECUTABLE)


