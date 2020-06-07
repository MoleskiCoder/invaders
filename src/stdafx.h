#ifdef _MSC_VER
#pragma once
#endif

#include <array>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <Bus.h>
#include <EventArgs.h>
#include <InputOutput.h>
#include <Intel8080.h>
#include <Register.h>
#include <Ram.h>
#include <Rom.h>
#include <Signal.h>

////

#include <SDL.h>
#include <SDL_mixer.h>

#ifdef _MSC_VER
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_mixer.lib")
#endif
