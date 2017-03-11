#include "stdafx.h"
#include "ColourPalette.h"

#include <SDL.h>

ColourPalette::ColourPalette()
: m_colours(2) {
}

void ColourPalette::load(SDL_PixelFormat* hardware) {

	auto black = ::SDL_MapRGBA(hardware, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
	auto white = ::SDL_MapRGBA(hardware, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);

	m_colours[0] = black;
	m_colours[1] = white;
}