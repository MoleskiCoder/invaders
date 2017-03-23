#pragma once

#include <vector>
#include <cstdint>

struct SDL_PixelFormat;

class ColourPalette {
public:
	enum {
		Black,
		White,
		Red,
		Green
	};

	static int calculateColour(int x, int y) {
		if (y < 32)
			return White;
		if (y < (32 + 32))
			return Red;
		if (y < (32 + 32 + 120))
			return White;
		if (y < (32 + 32 + 120 + 56))
			return Green;
		if (x < 16)
			return White;
		if (x < (16 + 118))
			return Green;
		return White;
	}

	ColourPalette();

	uint32_t getColour(size_t index) const {
		return m_colours[index];
	}

	void load(SDL_PixelFormat* hardware);

private:
	std::vector<uint32_t> m_colours;
};