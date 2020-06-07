EXE = invaders

CXXFLAGS = -Wall `sdl2-config --cflags` -std=c++14 -pipe -I ../libraries/EightBit/Gaming/inc -I ../libraries/EightBit/Intel8080/inc -I ../libraries/EightBit/inc

LDFLAGS  = `sdl2-config --libs` -lSDL2_mixer -L ../libraries/EightBit/Gaming/src -L ../libraries/EightBit/Intel8080/src -L ../libraries/EightBit/src -lgaming -lintel8080 -leightbit

CXXFILES = Board.cpp ColourPalette.cpp Game.cpp invaders.cpp SoundEffects.cpp

include ../libraries/EightBit/compile.mk
include ../libraries/EightBit/exe_build.mk
