#pragma once
#include "../Core/Circuit.h"
#include <fstream>
#include "sstream"


class SaveManager {
public:
	SaveManager();

	bool save(Circuit& circuit, const std::string& filename);
	bool load(const std::string& filename, CircuitData& outData);
	int toInt(ComponentType t);
	ComponentType toComponentType(int v);

private:
    void saveCircuitState(std::ostream& out, const Circuit& circuit);
    void saveComponents(std::ostream& out, const std::vector<Component>& components);
    void saveWires(std::ostream& out, std::unordered_map<int, Wire>& wires);
    void saveElectricalNodes(std::ostream& out, const std::unordered_map<int, ElectricalNode>& nodes);
	void saveComponentIndexMap(std::ostream& out, const std::unordered_map<int, int>& map);

	bool parseComponents(std::istream& in, std::vector<Component>& outComponents);
	bool parseWires(std::istream& in, std::unordered_map<int, Wire>& outWires);
	bool parseElectricalNodes(std::istream& in, std::unordered_map<int, ElectricalNode>& outNodes);
	bool parseComponentIndexMap(std::istream& in, std::unordered_map<int, int>& outMap);

	void trim(std::string& s);
	bool getlineNonEmpty(std::istream& in, std::string& line);

};