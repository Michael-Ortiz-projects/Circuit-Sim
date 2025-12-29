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

	bool operator==(const ElectricalConnection& other) const {
		return componentID == other.componentID && lead == other.lead;
	}
};

struct WireNodeReference {
	int wireID;
	int nodeID;

	bool isValid() const {
		return wireID >= 0 && nodeID >= 0;
	}
};

class ElectricalNode {
public:
	ElectricalNode();
	ElectricalNode(int ID);
	int id;  
	std::vector<ElectricalConnection> connections;
};