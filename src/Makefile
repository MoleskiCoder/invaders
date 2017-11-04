EXE = invaders

CXXFLAGS = -Wall `sdl2-config --cflags` -std=c++14 -pipe -I ../libraries/EightBit/Intel8080/inc -I ../libraries/EightBit/inc

LDFLAGS  = `sdl2-config --libs` -lSDL2_mixer -L ../libraries/EightBit/Intel8080/src -L ../libraries/EightBit/src -lintel8080 -leightbit

CXXFILES = Board.cpp ColourPalette.cpp Configuration.cpp GameController.cpp Game.cpp invaders.cpp SoundEffects.cpp

CXXOBJECTS = $(CXXFILES:.cpp=.o)

SOURCES = $(CXXFILES)
OBJECTS = $(CXXOBJECTS)

PCH = stdafx.h.gch

all: opt

opt: CXXFLAGS += -DNDEBUG -march=native -O3 -flto
opt: LDFLAGS += -s -flto
opt: $(EXE)

debug: CXXFLAGS += -g -D_DEBUG
debug: LDFLAGS += -g
debug: $(EXE)

coverage: CXXFLAGS += -g -D_DEBUG -fprofile-arcs -ftest-coverage
coverage: LDFLAGS += -g -lgcov
coverage: $(EXE)

$(PCH): stdafx.h
	$(CXX) $(CXXFLAGS) -x c++-header $<

$(EXE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXE) $(LDFLAGS)

%.o: %.cpp $(PCH)
	$(CXX) $(CXXFLAGS) $< -c -o $@

.PHONY: clean
clean:
	-rm -f $(EXE) $(OBJECTS) $(PCH) *.gcov *.gcda *.gcno
