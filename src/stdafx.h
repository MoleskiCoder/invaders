#ifdef _MSC_VER
#pragma once
#endif

#include <string>
#include <cstdint>
#include <stdexcept>
#include <functional>
#include <algorithm>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <array>
#include <vector>
#include <bitset>

#include <SDL.h>
#include <SDL_mixer.h>

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define HOST_LITTLE_ENDIAN
#endif

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define HOST_BIG_ENDIAN
#endif

#ifdef _MSC_VER
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_mixer.lib")
#endif
