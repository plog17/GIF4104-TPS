SOURCES= main.cpp lodepng.cpp Tokenizer.cpp

OBJECTS=$(SOURCES:.cpp=.o)

CC=pgc++
LD=pgc++
LIBS=-lstdc++
LIB_PATHS=-L/software6/compilers/gcc/4.8/lib64
INCLUDE_PATHS=
CFLAGS=-acc
LDFLAGS=-acc

EXECUTABLE=tp4-openacc

MAKE_CMD=$(LD) $(LDFLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LIB_PATHS) $(LIBS)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(MAKE_CMD)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c -o $@ $<

remake:
	rm $(EXECUTABLE)
	$(MAKE_CMD)

clean:
	rm -rf $(OBJECTS)
	rm -rf $(EXECUTABLE)