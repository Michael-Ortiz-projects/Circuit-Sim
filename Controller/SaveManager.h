#pragma once
#include "../Core/Circuit.h"
#include <fstream>
#include "sstream"


class SaveManager {
public:
	SaveManager();

	bool saveCircuitToFile(Circuit& circuit, const std::string& filePath);
	bool loadCircuitFromFile(CircuitData& circuit, const std::string& filePath, AssetManager& assets);
	bool expectToken(std::istream& in, const std::string& expected);

};
