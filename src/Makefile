EXE = invaders

CXXFLAGS = -Wall `sdl2-config --cflags` -std=c++11 -pipe
LDFLAGS  = `sdl2-config --libs` -lSDL2_mixer

CXXFILES = Board.cpp ColourPalette.cpp Configuration.cpp CpuEventArgs.cpp Disassembler.cpp Game.cpp GameController.cpp InputOutput.cpp Intel8080.cpp invaders.cpp Memory.cpp Profiler.cpp SoundEffects.cpp Processor.cpp

CXXOBJECTS = $(CXXFILES:.cpp=.o)

SOURCES = $(CXXFILES)
OBJECTS = $(CXXOBJECTS)

PCH = stdafx.h.gch

all: opt

opt: CXXFLAGS += -DNDEBUG -march=native -O2
opt: LDFLAGS += -s
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
