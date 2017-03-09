#pragma once

#include <string>

class Configuration {
public:
	Configuration();

	std::string getRomDirectory() const { return m_romDirectory; }

private:
	std::string m_romDirectory;
};

