#pragma once
#include <vector>

enum class Lead {
	A,
	B
};

struct ElectricalConnection {
	int componentID;
	Lead lead;
};

struct ElectricalNode {
	int id;  // stable, unique

	std::vector<ElectricalConnection> connections;
};