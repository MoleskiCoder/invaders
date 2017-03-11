#pragma once

#include <vector>
#include <cstdint>

struct SDL_PixelFormat;

class ColourPalette {
public:
	ColourPalette();

	const std::vector<uint32_t>& getColours() const {
		return m_colours;
	}

	uint32_t getColour(size_t index) const {
		return m_colours[index];
	}

	void load(SDL_PixelFormat* hardware);

private:
	std::vector<uint32_t> m_colours;
};