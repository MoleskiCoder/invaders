#ifdef _MSC_VER
#pragma once
#endif

#include <memory>
#include <stdexcept>
#include <cstdint>
#include <algorithm>

#include <string>
#include <vector>
#include <array>
#include <map>

#include <iostream>

#include <Memory.h>
#include <InputOutput.h>
#include <Intel8080.h>
#include <Profiler.h>
#include <EventArgs.h>
#include <Disassembler.h>

#include <SDL.h>
#include <SDL_mixer.h>

#ifdef _MSC_VER
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_mixer.lib")
#endif
