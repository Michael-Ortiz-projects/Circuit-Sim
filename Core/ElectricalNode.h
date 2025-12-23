#pragma once
#include <vector>

enum class Lead {
	A,
	B,
	Null
};

struct ElectricalConnection {
	int componentID;
	Lead lead;
};

class ElectricalNode {
public:
	ElectricalNode();
	ElectricalNode(int ID);
	int id;  
	std::vector<ElectricalConnection> connections;
};