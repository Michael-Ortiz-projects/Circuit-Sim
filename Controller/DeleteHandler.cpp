#include "DeleteHandler.h"
#include <algorithm>

DeleteHandler::DeleteHandler(Circuit& c, Selection& s)
	: circuit(c), selection(s) { }

void DeleteHandler::onKeyPress(const sf::Event::KeyEvent& event) {
	if (event.code != sf::Keyboard::Delete) return;

	deleteSelection();
	// order for deletion components -> wires -> segments -> nodes -> then cleanup wires and check for splits and all that
	// delete all your elements in the selection, then do dfs to check for a split then split wires by doing dfs on every node in each wire's remaining graph
}

bool DeleteHandler::shouldRelease() const {
	return release;
}

void DeleteHandler::deleteSelection() {
	Selection simplified_selection = simplifySelection();

	deleteComponents(simplified_selection.componentIDs);
	deleteSegments(simplified_selection.segments);
	deleteNodes(simplified_selection.nodes);
	deleteWires(simplified_selection.wireIDs);
	//updateWires(); still need to update the wires after deletion, but deletion works
}

void DeleteHandler::deleteComponents(std::unordered_set<int> component_IDs) {
	for (const auto ID : component_IDs) {
		Component& c = *circuit.getComponent(ID);
		ElectricalConnection connection_A(ID, Lead::A);
		ElectricalConnection connection_B(ID, Lead::B);

		if (c.nodeA != -1) {
			circuit.removeConnectionFromNode(ID, connection_A);
			circuit.getWire(c.A_WireNodeReference.wireID)->getNode(c.A_WireNodeReference.nodeID).isAnchor = false;
		}

		if (c.nodeB != -1) {
			circuit.removeConnectionFromNode(ID, connection_B);
			circuit.getWire(c.B_WireNodeReference.wireID)->getNode(c.B_WireNodeReference.nodeID).isAnchor = false;
		}
		circuit.removeComponent(ID);
	}
}

void DeleteHandler::deleteSegments(std::unordered_set<WireSegmentReference> segments) {
	for (const WireSegmentReference segment : segments) {
		circuit.getWire(segment.wireID)->disconnectNodes(segment.nodeA, segment.nodeB);
	}
}

void DeleteHandler::deleteNodes(std::unordered_set<WireNodeReference> nodes) {
	for (const WireNodeReference wire_node : nodes) {
		circuit.getWire(wire_node.wireID)->removeNode(wire_node.nodeID);
	}
}

void DeleteHandler::deleteWires(std::unordered_set<int> wire_IDs) {
	for (const int wire_ID : wire_IDs) {
		circuit.eraseWire(wire_ID);
	}
}

Selection DeleteHandler::simplifySelection() {
	Selection simplified_selection = selection;

	// bucket nodes by wire
	std::unordered_map<int, std::unordered_set<int>> nodesByWire;
	for (const auto& n : simplified_selection.nodes) {
		if (!n.isValid()) continue;
		nodesByWire[n.wireID].insert(n.nodeID);
	}

	// remove segments whose both endpoints are selected
	std::erase_if(simplified_selection.segments, [&](const WireSegmentReference& s) {
		auto it = nodesByWire.find(s.wireID);
		if (it == nodesByWire.end()) return false;
		const auto& selectedNodes = it->second;
		return selectedNodes.contains(s.nodeA) && selectedNodes.contains(s.nodeB);
		});

	// promote fully selected wires
	for (const auto& [wireID, nodeSet] : nodesByWire) {
		Wire& wire = *circuit.getWire(wireID);
		if (nodeSet.size() == wire.getGraph().size()) {

			simplified_selection.wireIDs.insert(wireID);

			// remove all nodes and segments for this wire
			std::erase_if(simplified_selection.nodes, [&](const WireNodeReference& n) {
				return n.wireID == wireID;
				});
			std::erase_if(simplified_selection.segments, [&](const WireSegmentReference& s) {
				return s.wireID == wireID;
				});
		}
	}

	return simplified_selection;
}
