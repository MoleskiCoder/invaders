#include "stdafx.h"
#include "ColourPalette.h"

#include <SDL.h>

ColourPalette::ColourPalette() noexcept
: m_colours(4) {
}

void ColourPalette::load(const SDL_PixelFormat* hardware) {

	auto black = ::SDL_MapRGBA(hardware, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
	auto white = ::SDL_MapRGBA(hardware, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
	auto red = ::SDL_MapRGBA(hardware, 0xff, 0x00, 0x00, SDL_ALPHA_OPAQUE);
	auto green = ::SDL_MapRGBA(hardware, 0x00, 0xff, 0x00, SDL_ALPHA_OPAQUE);

	m_colours[Black] = black;
	m_colours[White] = white;
	m_colours[Red] = red;
	m_colours[Green] = green;
}