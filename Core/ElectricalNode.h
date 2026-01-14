#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <unordered_set>
#include <cstddef> 

struct WireNodeReference {
	int wireID;
	int nodeID;

	bool isValid() const {
		return wireID >= 0 && nodeID >= 0;
	}

	bool operator==(const WireNodeReference& other) const {
		return wireID == other.wireID && nodeID == other.nodeID;
	}
};

struct WireSegmentReference {
	int wireID;
	int nodeA;
	int nodeB;

	bool isValid() const {
		return wireID >= 0 && nodeA >= 0 && nodeB >= 0;
	}

	bool operator==(const WireSegmentReference& other) const {
		return wireID == other.wireID &&
			((nodeA == other.nodeA && nodeB == other.nodeB) ||
				(nodeA == other.nodeB && nodeB == other.nodeA));
	}
};

namespace std {

	template <>
	struct hash<WireNodeReference> {
		size_t operator()(const WireNodeReference& n) const noexcept {
			size_t h1 = std::hash<int>{}(n.wireID);
			size_t h2 = std::hash<int>{}(n.nodeID);
			return h1 ^ (h2 << 1);
		}
	};

	template <>
	struct hash<WireSegmentReference> {
		size_t operator()(const WireSegmentReference& s) const noexcept {
			int a = std::min(s.nodeA, s.nodeB);
			int b = std::max(s.nodeA, s.nodeB);

			size_t h1 = std::hash<int>{}(s.wireID);
			size_t h2 = std::hash<int>{}(a);
			size_t h3 = std::hash<int>{}(b);

			return h1 ^ (h2 << 1) ^ (h3 << 2);
		}
	};

}
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




struct Selection {
	std::unordered_set<int> componentIDs;
	std::unordered_set<int> wireIDs;
	std::unordered_set<WireNodeReference> nodes;
	std::unordered_set<WireSegmentReference> segments;

	void clear() {
		componentIDs.clear();
		wireIDs.clear();
		nodes.clear();
		segments.clear();
	}

	bool empty() const {
		return componentIDs.empty()
			&& wireIDs.empty()
			&& nodes.empty()
			&& segments.empty();
	}
};

struct Wirecoverage {
	int segmentCount = 0;
	int coveredSegments = 0;
};
class ElectricalNode {
public:
	ElectricalNode();
	ElectricalNode(int ID);
	int id;  
	std::vector<ElectricalConnection> connections;
};